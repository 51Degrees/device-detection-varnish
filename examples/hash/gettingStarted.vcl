/**
@example hash/gettingStarted.vcl

@include{doc} example-getting-started-hash.txt

This example is available in full on [GitHub](https://github.com/51Degrees/device-detection-varnish/blob/master/examples/hash/gettingStarted.vcl).

@include{doc} example-require-datafile.txt

The path to the data need to be updated before running the example.

In a Linux environment, the following commands:
```
$ curl localhost:8080 -I -A "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36"
$ curl localhost:8080 -I -A "Mozilla/5.0 (iPhone; CPU iPhone OS 11_2 like Mac OS X) AppleWebKit/604.4.7 (KHTML, like Gecko) Mobile/15C114"
```
Expected output:
```
HTTP/1.1 200 OK
...
X-IsMobile: False
...

...
HTTP/1.1 200 OK
...
X-IsMobile: True
```

*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	set req.http.X-IsMobile = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");
}

sub vcl_deliver {
	set resp.http.X-IsMobile = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}
