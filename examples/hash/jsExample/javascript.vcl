/**
@example hash/jsExample/javascript.vcl

Shows how to serve the JavaScript in a property, so that a browser can send
evidence which only it knows. Here the page loads the script for the
ScreenPixelsWidth property from /51D.js, the script sets the 51D_ScreenPixelsWidth
cookie, and /overrides then returns the width from the evidence.

This example is available in full on [GitHub](https://github.com/51Degrees/51degrees-varnish/blob/main/examples/hash/jsExample/javascript.vcl).

The path to the data file needs to be updated before running the example.

Start Varnish listening on port 8888 with this file, then open index.html from this
folder in a browser. It shows the width of the screen.
*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	if (req.url ~ "^/(51D\.js|overrides)") {
		return (synth(200));
	}
}

sub vcl_synth {
	if (req.url ~ "^/51D\.js") {
		set resp.http.Content-Type = "application/x-javascript";
		synthetic(fiftyonedegrees.get_javascript_single(req.http.user-agent, "ScreenPixelsWidthJavascript"));
		return (deliver);
	}
	if (req.url ~ "^/overrides") {
		set resp.http.Access-Control-Allow-Origin = "*";
		set resp.http.Access-Control-Allow-Headers = "*";
		set resp.http.Access-Control-Expose-Headers = "*";
		set resp.http.x-screenpixelswidth = fiftyonedegrees.match_all("ScreenPixelsWidth");
		return (deliver);
	}
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}

/**
Find out more

The engine this example uses:
https://github.com/51Degrees/51degrees-cxx
The Varnish modules:
https://github.com/51Degrees/51degrees-varnish

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-jsexample-javascript-vcl&utm_term=device-detection
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-jsexample-javascript-vcl&utm_term=pricing
*/
