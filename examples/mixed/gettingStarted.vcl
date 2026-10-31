/**
@example mixed/gettingStarted.vcl

Shows how to load the device detection and IP Intelligence modules together, and
use both for the same request.

This example is available in full on [GitHub](https://github.com/51Degrees/51degrees-varnish/blob/main/examples/mixed/gettingStarted.vcl).

The example uses the Lite device detection data file and the ASN IP Intelligence
data file, which come with the 51degrees-cxx submodule at
`cxx/device-detection-data/51Degrees-LiteV4.1.hash` and
`cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi`. The paths to the data
files need to be updated before running the example.

In a Linux environment, the following command:
```
$ curl localhost:8080 -I -H "X-Real-IP: 212.58.224.22" -A "Mozilla/5.0 (iPhone; CPU iPhone OS 11_2 like Mac OS X) AppleWebKit/604.4.7 (KHTML, like Gecko) Mobile/15C114"
```
Expected output:
```
HTTP/1.1 200 OK
...
x-ismobile: True
x-asn: \"BBC\":1
...
```
*/
vcl 4.0;

import fiftyonedegrees;
import fiftyonedegrees_ipi;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	set req.http.x-ismobile = fiftyonedegrees.match_all("IsMobile");
	set req.http.x-asn = fiftyonedegrees_ipi.match("AsnName", req.http.X-Real-IP);
}

sub vcl_deliver {
	set resp.http.x-ismobile = req.http.x-ismobile;
	set resp.http.x-asn = req.http.x-asn;
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
	fiftyonedegrees_ipi.start("/etc/varnish/51Degrees-IPIV4AsnIpiV41.ipi");
}

/**
Find out more

The engine this example uses:
https://github.com/51Degrees/51degrees-cxx
The Varnish modules:
https://github.com/51Degrees/51degrees-varnish

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-mixed-gettingstarted-vcl&utm_term=device-detection
IP Intelligence: https://51degrees.com/products/ip-intelligence?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-mixed-gettingstarted-vcl&utm_term=ip-intelligence
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-mixed-gettingstarted-vcl&utm_term=pricing
*/
