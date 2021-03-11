/**
@example hash/config.vcl

This example shows how to configure the available settings for 51Degrees'
on-premise device detection in Varnish. This example is available in full 
on [GitHub](https://github.com/51Degrees/device-detection-varnish/blob/master/examples/hash/config.vcl).

@include{doc} example-require-datafile.txt

The path to the data need to be updated before running the example.

In a Linux environment, the following commands:
```
$ curl localhost:8080 -I -A "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36"
```
Expected output:
```
HTTP/1.1 200 OK
...
X-Metadata: False:55:0:0:PREDICTIVE:__zilla/5.0 (Windows____10.0; Win6_______________________________________________Chrome/7_.0.3904__7 Safari/5_____:23:15364-38914-97847-18092
```

*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_deliver {
	set resp.http.X-Metadata = fiftyonedegrees.match_all("IsMobile,Iterations,Drift,Difference,Method,UserAgents,MatchedNodes,DeviceId");
}

sub vcl_init {
	fiftyonedegrees.set_delimiter(":");
	fiftyonedegrees.set_drift(10);
	fiftyonedegrees.set_difference(8);
	fiftyonedegrees.set_max_concurrency(5);
	fiftyonedegrees.set_allow_unmatched("YES");
	fiftyonedegrees.set_use_performance_graph("NO");
	fiftyonedegrees.set_use_predictive_graph("YES");
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}
