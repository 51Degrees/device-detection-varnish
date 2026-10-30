/**
@example curl/default.vcl

The VCL the Docker image in this folder runs. It sets each value on a response
header, so it can be seen with cURL. See the README in this folder.
*/
vcl 4.0;

import fiftyonedegrees;
import fiftyonedegrees_ipi;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	return (synth(200));
}

sub vcl_synth {
	set resp.http.x-device-type = fiftyonedegrees.match_all("DeviceType");
	set resp.http.x-platform-name = fiftyonedegrees.match_all("PlatformName");
	set resp.http.x-platform-version = fiftyonedegrees.match_all("PlatformVersion");
	set resp.http.x-browser-name = fiftyonedegrees.match_all("BrowserName");
	set resp.http.x-browser-version = fiftyonedegrees.match_all("BrowserVersion");
	set resp.http.x-is-mobile = fiftyonedegrees.match_all("IsMobile");
	set resp.http.x-asn = fiftyonedegrees_ipi.match("AsnName", req.http.X-Real-IP);
	return (deliver);
}

sub vcl_init {
	fiftyonedegrees.start("/app/cxx/device-detection-data/51Degrees-LiteV4.1.hash");
	fiftyonedegrees_ipi.start("/app/cxx/ip-intelligence-data/51Degrees-IPIV4AsnIpiV41.ipi");
}

/**
Find out more

The engine this example uses:
https://github.com/51Degrees/51degrees-cxx
The Varnish modules:
https://github.com/51Degrees/51degrees-varnish

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-default-vcl&utm_term=device-detection
IP Intelligence: https://51degrees.com/products/ip-intelligence?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-default-vcl&utm_term=ip-intelligence
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-default-vcl&utm_term=pricing
*/
