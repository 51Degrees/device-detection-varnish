/**
@example hash/matchMetrics.vcl

Shows how to get the metrics of a match as well as the properties of the device.
The metrics are Drift, Difference, Method, MatchedNodes, UserAgents, DeviceId and
Iterations.

This example is available in full on [GitHub](https://github.com/51Degrees/51degrees-varnish/blob/main/examples/hash/matchMetrics.vcl).

The path to the data file needs to be updated before running the example.

In a Linux environment, the following command:
```
$ curl localhost:8080 -I -A "Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 Safari/9537.53"
```
Expected output:
```
HTTP/1.1 200 OK
...
x-metrics: 0,0,PREDICTIVE,18
x-user-agents: _________.0 (iPhone_______Phone ___7_1 like ____OS X_______WebK__________2 (KHTML,______Gecko_ Version_______bile/1__________ari/95_____
x-device-id: 12280-24384-24305-0
...
```
*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_deliver {
	set resp.http.x-metrics = fiftyonedegrees.match_all("Drift,Difference,Method,MatchedNodes");
	set resp.http.x-user-agents = fiftyonedegrees.match_all("UserAgents");
	set resp.http.x-device-id = fiftyonedegrees.match_all("DeviceId");
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

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-matchmetrics-vcl&utm_term=device-detection
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-matchmetrics-vcl&utm_term=pricing
*/
