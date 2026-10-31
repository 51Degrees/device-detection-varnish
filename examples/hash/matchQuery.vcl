/**
@example hash/matchQuery.vcl

Shows how to perform a detection using evidence from the query string of a
request. A query argument with the name of a header takes the place of that
header, and a query argument or cookie named after a property with the prefix
51D_ overrides that property.

This example is available in full on [GitHub](https://github.com/51Degrees/51degrees-varnish/blob/main/examples/hash/matchQuery.vcl).

The path to the data file needs to be updated before running the example.

In a Linux environment, the following command sends a desktop User-Agent in the
header, and a mobile one in the query string:
```
$ curl -I -A "Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0" "localhost:8080/?User-Agent=Mozilla%2F5.0%20(iPhone%3B%20CPU%20iPhone%20OS%207_1%20like%20Mac%20OS%20X)%20AppleWebKit%2F537.51.2%20(KHTML%2C%20like%20Gecko)%20Version%2F7.0%20Mobile%2F11D167%20Safari%2F9537.53"
```
Expected output:
```
HTTP/1.1 200 OK
...
x-mobile-all: True
x-mobile-header: False
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
	set resp.http.x-mobile-all = fiftyonedegrees.match_all("IsMobile");
	set resp.http.x-mobile-header = fiftyonedegrees.match_single(req.http.user-agent, "IsMobile");
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

Device detection: https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-matchquery-vcl&utm_term=device-detection
Data files and pricing: https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-hash-matchquery-vcl&utm_term=pricing
*/
