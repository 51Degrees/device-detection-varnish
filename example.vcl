/**
@example example.vcl
Getting started example of using 51Degrees device detection.

The example shows how to:

1. Instantiate the 51Degrees dataset and the resource manager with the
specified data file.
```
sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}
```

2. Get the properties values of the match results using the available http
headers of the received requests and assign them to the appropriate request
headers during Varnish cache Receive stage.
```
sub vcl_recv {
	set req.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set req.http.X-IsMobileX = fiftyonedegrees.match_all("IsMobile,Iterations,Drift,Difference,Method,UserAgents,MatchedNodes,DeviceId");
	set req.http.X-BrowserName = fiftyonedegrees.match_all("BrowserName");
	set req.http.X-PlatformName = fiftyonedegrees.match_all("PlatformName");
	set req.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set req.http.X-Method = fiftyonedegrees.match_all("Method");
	set req.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set req.http.X-DeviceId = fiftyonedegrees.match_all("DeviceId");
}
```

3. Get the properties values of the match results using the available http
headers of the received requests and assign them to the appropriate response
headers during Varnish cache Deliver stage.
```
sub vcl_deliver {
	set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set resp.http.X-IsMobileX = fiftyonedegrees.match_all("IsMobile,Iterations,Drift,Difference,Method,UserAgents,MatchedNodes,DeviceId");
	set resp.http.X-BrowserName = fiftyonedegrees.match_single(req.http.user-agent, "BrowserName");
	set resp.http.X-PlatformName = fiftyonedegrees.match_single(req.http.user-agent, "PlatformName");
	set resp.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set resp.http.X-Method = fiftyonedegrees.match_all("Method");
	set resp.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set resp.http.X-ID = fiftyonedegrees.match_all("DeviceId");
}
```
*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	set req.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set req.http.X-IsMobileX = fiftyonedegrees.match_all("IsMobile,Iterations,Drift,Difference,Method,UserAgents,MatchedNodes,DeviceId");
	set req.http.X-BrowserName = fiftyonedegrees.match_all("BrowserName");
	set req.http.X-PlatformName = fiftyonedegrees.match_all("PlatformName");
	set req.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set req.http.X-Method = fiftyonedegrees.match_all("Method");
	set req.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set req.http.X-DeviceId = fiftyonedegrees.match_all("DeviceId");
}

sub vcl_deliver {
	set resp.http.X-IsMobile = fiftyonedegrees.match_all("IsMobile");
	set resp.http.X-IsMobileX = fiftyonedegrees.match_all("IsMobile,Iterations,Drift,Difference,Method,UserAgents,MatchedNodes,DeviceId");
	set resp.http.X-BrowserName = fiftyonedegrees.match_single(req.http.user-agent, "BrowserName");
	set resp.http.X-PlatformName = fiftyonedegrees.match_single(req.http.user-agent, "PlatformName");
	set resp.http.X-Difference = fiftyonedegrees.match_all("Difference");
	set resp.http.X-Method = fiftyonedegrees.match_all("Method");
	set resp.http.X-Rank = fiftyonedegrees.match_all("Rank");
	set resp.http.X-ID = fiftyonedegrees.match_all("DeviceId");
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}
