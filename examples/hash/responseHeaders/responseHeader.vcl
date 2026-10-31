/**
@example hash/responseHeaders/responseHeader.vcl

Shows how to ask a browser for User-Agent client hints with the Accept-CH response
header, so that more evidence arrives with its next request and the detection is
more accurate. A browser which supports them shows more detail when the page is
refreshed.

This example is available in full on [GitHub](https://github.com/51Degrees/51degrees-varnish/blob/main/examples/hash/responseHeaders/responseHeader.vcl).

The example needs a data file with the SetHeader properties, which the Lite data
file does not have. The path to the data file needs to be updated before running
the example.

Open http://localhost:8080 in a browser, and refresh the page.
*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	return (synth(200));
}

sub vcl_synth {
	fiftyonedegrees.set_resp_headers();
	set resp.http.Content-Type = "text/html";
	synthetic("<html><body><ul>" +
		"<li>Hardware vendor: " + fiftyonedegrees.match_all("HardwareVendor") + "</li>" +
		"<li>Hardware name: " + fiftyonedegrees.match_all("HardwareName") + "</li>" +
		"<li>Device type: " + fiftyonedegrees.match_all("DeviceType") + "</li>" +
		"<li>Platform: " + fiftyonedegrees.match_all("PlatformVendor,PlatformName,PlatformVersion") + "</li>" +
		"<li>Browser: " + fiftyonedegrees.match_all("BrowserVendor,BrowserName,BrowserVersion") + "</li>" +
		"</ul></body></html>");
	return (deliver);
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-Enterprise-HashV41.hash");
}

/**
Find out more

The engine this example uses:
https://github.com/51Degrees/51degrees-cxx
The Varnish modules:
https://github.com/51Degrees/51degrees-varnish

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-responseheaders-responseheader-vcl&utm_term=device-detection
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-responseheaders-responseheader-vcl&utm_term=pricing
*/
