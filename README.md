# 51Degrees Device Detection VMOD

![51Degrees](https://51degrees.com/img/logo.png?utm_source=github&utm_medium=repository&utm_campaign=varnish_open_source&utm_content=readme_main "Data rewards the curious") **Device Detection in C** Varnish module

[Developer documentation](https://51degrees.com/documentation/_other_integrations__varnish.html)

## Introduction

The project contains source code for the Varnish integration of 51Degrees' on-premise Device Detection engine.

The module is supported for Varnish 6.0.6 (LTS).

## Pre-requisites

- gcc
- autoconf
- automake
- libvarnishapi-dev
- Varnish source
  
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

This will also clone the dependent sub-module `device-detection-cxx` and any of its' recursive sub-modules. 

Move to the Varnish directory with

```
$ cd device-detection-varnish
```

and install the module with (**NOTE**: if Varnish source was installed using the package manager, its' files will be located under `/usr/include/varnish` directory rather than `/usr/local/include/varnish`. The environment variable `VARNISHSRC` will need to be set to point to this location for the installation to succeed).

```
$ .autogen.sh

$ ./configure --with-config=release|test --with-datafile=(optional)

$ make

$ sudo make install
```

Where `--with-config`  [optional] sets the version that will be built. `release` and `test` versions are identical, except that the `test` version expose some additional functions to support the testing process. Only `release` version is recommended for production. By default `release` version is built.

`--with-datafile` [optional] sets the data file used by the unit tests. This data file will also be copied on installation.

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
