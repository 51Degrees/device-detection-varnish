# 51Degrees Device Detection VMOD

![51Degrees](https://51degrees.com/img/logo.png?utm_source=github&utm_medium=readme&utm_campaign=device-detection-varnish&utm_content=readme.md&utm_term=51degrees-device-detection-vmod "Data rewards the curious") **Device Detection in C** Varnish module

[Developer documentation](https://51degrees.com/documentation/_other_integrations__varnish.html?utm_source=github&utm_medium=readme&utm_campaign=device-detection-varnish&utm_content=readme.md&utm_term=51degrees-device-detection-vmod)

## Introduction

The project contains source code for the Varnish integration of 51Degrees' on-premise Device Detection and IP Intelligence engines, as two modules that can be used together or on their own. The `fiftyonedegrees` module provides device detection, and the `fiftyonedegrees_ipi` module provides [IP Intelligence](#ip-intelligence).

The module is supported for Varnish 6.0 (LTS), 7.7 and 8.0, and is tested on 6.0.6, 6.0.8, 6.0.17, 7.7.3 and 8.0.1.

## Pre-requisites

- gcc
- autoconf
- automake
- libvarnishapi-dev
- Varnish source
- libatomic
  
For Ubuntu based distributions gcc can be found on apt, use

```
$ sudo apt-get install gcc autoconf automake libvarnishapi-dev varnish
```

## Install

### Linux

#### For an existing Varnish deployment

##### Static Module

To install the module into an existing Varnish deployment,

first clone 51Degrees/device-detection-varnish repository with

```
$ git clone --recursive https://github.com/51Degrees/device-detection-varnish.git
```

This will also clone the dependent sub-module `cxx`, which holds the [51Degrees C and C++ code](https://github.com/51Degrees/51degrees-cxx), and any of its' recursive sub-modules.

Move to the Varnish directory with

```
$ cd device-detection-varnish
```

and install the module with (**NOTE**: if Varnish source was installed using the package manager, its' files will be located under `/usr/include/varnish` directory rather than `/usr/local/include/varnish`. The environment variable `VARNISHSRC` will need to be set to point to this location for the installation to succeed).

```
$ ./autogen.sh

$ ./configure --with-config=release|test --with-datafile=(optional)

$ make

$ sudo make install
```

Where `--with-config`  [optional] sets the version that will be built. `release` and `test` versions are identical, except that the `test` version expose some additional functions to support the testing process. Only `release` version is recommended for production. By default `release` version is built.

`--with-datafile` [optional] sets the data file used by the unit tests. This data file will also be copied on installation.

`--with-datafile-ipi` [optional] sets the IP Intelligence data file used by the unit tests.

When varnish is installed in a non standard directory, please set the value of environment variable `VARNISHSRC` to point to where the varnish source resides before running the `configure` (e.g. ``export VARNISHSRC=/usr/include/varnish``).

The location of where the module will be installed can also be customised by setting the environment variable `VMOD_DIR` before running the `configure`.

Then run the included tests with the following command. A `test` version is required for this to succeed:

```
$ make check
```

This will all pass if the local installation was successful.

## Configure

Before start matching user agents, you may wish to configure the solution to use a different database for example.

### Settings

#### General Settings

These settings should be set before calling `fiftyonedegrees.start`.
- ``set_performance_profile`` (defaults to ``DEFAULT``). Set the performance profile.
- ``set_drift`` (defaults to ``0``). Set the drift value.
- ``set_difference`` (defaults to ``0``). Set the difference value.
- ``set_max_concurrency`` (defaults is determined by the performance profile). Set the expected concurrent detection requests.
- ``set_allow_unmatched`` (defaults is ``NO``). Set whether unmatched node should be allowed.
- ``set_use_performance_graph`` (defaults is determined by the performance profile). Set whether the performance optimized graph should be used.
- ``set_use_predictive_graph`` (defaults is determined by the performance profile). Set whether the predictive optimized graph should be used.
-  ``set_delimiter`` (defaults to ``','``). Sets the delimiter to separate values with.
-  ``set_properties`` (defaults to all properties). Sets the properties to initialise.

## Usage

The easiest way to view full usage information is to run the command:

```
man vmod_fiftyonedegrees
```

to display all the functions with examples.

### The VCL File

An example configuration file is included in this repository. It shows how to add device information to HTTP headers.

#### Initialising the Resource Manager

In the init block is where you should set any settings and initialise the resource manager.

```
import fiftyonedegrees;

sub vcl_init {
    # Initialise the resource manager with the data file.
    fiftyonedegrees.start("/home/51Degrees/data/51Degrees.dat");
}
```

#### Matching

Matching can be done is two ways.

##### User-Agent match

To get properties using the device's User-Agent use:

```
sub vcl_deliver {
    # This sets resp.http.X-IsMobile to "True"/"False".
    set resp.http.X-IsMobile = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");
}
```

##### Multiple HTTP header matches

To get properties from all the relevant HTTP headers from the device use:

```
sub vcl_deliver {
    # This sets resp.http.X-IsMobile to "True"/"False".
    set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
}
```

##### Client-Hints

To enable client-hint matching, add the ``set_resp_headers`` to the ``vcl_deliver`` block. This adds the ``Accept-CH`` header to the response if
it is supported by the browser. The client-hint headers sent by the browser
are then automatically used by the ``match_all`` method.

```
sub vcl_deliver {
    # Enable client-hints
    fiftyonedegrees.set_resp_headers();
    # Use the match_all as above.
    set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
}
```

##### Output Format

The value of the header is set to a comma separated list of values (comma delimited is the default behaviour, but the delimiter can be set explicitly with ``set_delimiter``), these are in the same order the properties are listed in the config file. So setting a header with the line:

```
set resp.http.X-Device = fiftyonedegrees.match_all("HardwareName,BrowserName,PlatformName");
```

will give a header named ``X-Device`` with a value like ``Desktop,Firefox,Ubuntu``. Alternatively, headers can be set individually like:

```
set resp.http.X-HardwareName = fiftyonedegrees.match_all("HardwareName");
set resp.http.X-BrowserName = fiftyonedegrees.match_all("BrowserName");
set resp.http.X-PlatformName = fiftyonedegrees.match_all("PlatformName");
```

giving three separate headers.

## Example

Start Varnish using the example VCLs within the `device-detection-varnish/examples/hash` directory with (**NOTE**: The data file path used by the `start` API in the `vcl_init` block will need to be updated before proceed):

```
$ varnishd -f /path/to/examples/hash/gettingStarted.vcl -a localhost:8080 -d
```

Then, type `start` to launch the worker process.

In a Linux environment, the headers can be viewed with the command:

```
$ curl localhost:8080 -I -A "Mozilla/5.0 (iPhone; CPU iPhone OS 11_2 like Mac OS X) AppleWebKit/604.4.7 (KHTML, like Gecko) Mobile/15C114"
```

which will give the following response:

```
HTTP/1.1 200 OK
...
X-IsMobile: True
...
```

## IP Intelligence

The `fiftyonedegrees_ipi` module looks up an IP address and returns the properties of the network it belongs to. It is a separate module because IP Intelligence needs the common C code compiled differently from device detection, so one library cannot hold both. `make install` installs both modules, and each VCL imports the ones it uses.

### Data files

The module reads version 4.6 data files. A data file of another version is refused when the VCL loads.

| Data file | Where to get it | Properties |
| --- | --- | --- |
| ASN | `cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi`, included with the `cxx` sub-module | `AsnName` and `Asn` |
| Lite and Enterprise | See the [data file options](https://51degrees.com/pricing?utm_source=github&utm_medium=readme&utm_campaign=device-detection-varnish&utm_content=readme.md&utm_term=ip-intelligence-data-files) | Many more, such as `RegisteredName` and `RegisteredCountry` |

### The VCL file

```
import fiftyonedegrees_ipi;

sub vcl_init {
	fiftyonedegrees_ipi.set_properties("AsnName,Asn");
	fiftyonedegrees_ipi.start("/path/to/51Degrees-IPIV4AsnIpiV41.ipi");
}

sub vcl_recv {
	set req.http.x-asn = fiftyonedegrees_ipi.match("AsnName");
}
```

| Function | What it does |
| --- | --- |
| `start(filePath)` | Loads the data file. If the file cannot be used the VCL does not load, and Varnish carries on with the VCL it had. |
| `reload(filePath)` | Reloads the data, from the original file if no path is given. If that fails the data already loaded stays in use. |
| `match(properties, ip)` | Returns the values of the comma separated properties for an IP address. |
| `set_properties(properties)` | Before `start`, sets the properties to load. Loading only the ones needed reduces memory use and makes each match faster. By default all of them are loaded. |
| `set_delimiter(delimiter)` | Before `start`, sets what separates the values of different properties. By default this is a comma. |
| `set_performance_profile(profile)` | Before `start`, sets how much of the data is held in memory, one of `DEFAULT`, `IN_MEMORY`, `HIGH_PERFORMANCE`, `LOW_MEMORY` or `BALANCED`. |
| `set_max_concurrency(concurrency)` | Before `start`, sets the maximum number of expected concurrent requests. |
| `get_dataset_name()`, `get_dataset_format()`, `get_dataset_published_date()` | Describe the data file loaded. |

### Choosing the address

With no second argument `match` looks up `client.ip`. When Varnish sits behind a proxy or a load balancer, pass the address to use instead, for example `fiftyonedegrees_ipi.match("AsnName", req.http.X-Real-IP)`.

Pass one address. Given a list such as the value of `X-Forwarded-For`, the module looks up the first address in the string and ignores the rest, and the first address is not always one you can trust, so choose the address in VCL.

### Values

Each value takes the form `"value":weight`, where the weight is a number between 0 and 1 showing the confidence in the value. A property with several values has them separated by a pipe. The double quotes are escaped with a backslash, so the string can be used as a header value, in the same way as the [Nginx module](https://github.com/51Degrees/device-detection-nginx). For example, `fiftyonedegrees_ipi.match("AsnName,Asn", "212.58.224.22")` returns `\"BBC\":1,\"AS2818\":1` with the ASN data file.

`N/A` is returned in place of a value when the property was not loaded, the property has no value, or the address is missing or malformed. The reason is logged as a `VCL_Error` record, and the request carries on. If the values do not fit in the workspace, the result is not set.

### Examples

`examples/ipi/gettingStarted.vcl` uses the module on its own, and `examples/mixed/gettingStarted.vcl` uses it alongside device detection for the same request.
