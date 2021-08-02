/**
@example hash/clientHints.vcl

@include{doc} example-client-hints-hash.txt

This example is available in full on [GitHub](https://github.com/51Degrees/device-detection-varnish/blob/master/examples/hash/clientHints.vcl).

@include{doc} example-require-datafile.txt 

The path to the data need to be updated before running the example.

Access localhost:8080 from a browser which supports client-hints. the output will show first the
device properties from the standard headers alone. By clicking the "Make second request" button,
the properties will refresh using the client-hint headers fetched from the browser.

Expected output:
```
User Agent Client Hints Example
Select the Make second request button below, to use User Agent Client Hint headers in evidence for device detections.
...
Browser Vendor: Google
Browser Name: Chrome
Browser Version: 91
...
```

*/
vcl 4.0;

import fiftyonedegrees;

backend default {
	.host = "127.0.0.1";
	.port = "80";
}

sub vcl_recv {
	if (req.url ~ "/") {
        return (synth(800, ""));
    }
}

sub vcl_synth {
	// Set the client-hint response headers that request more info
	// from the browser. Here we're using vcl_synth, rather than
	// vcl_deliver so that we don't have to set up a backend just
	// for the example.
	fiftyonedegrees.set_resp_headers();

	// Serve the example page. NOTE: this is not a good example of
	// returning a file, it's just for simplicity.
    if (resp.status == 800) {
        set resp.status = 200;
        set resp.http.Content-Type = "text/html; charset=utf-8";

		synthetic({"
<h2>User Agent Client Hints Example</h2>

<p>
    By default, the user-agent, sec-ch-ua and sec-ch-ua-mobile HTTP headers
    are sent.
    <br />
    This means that on the first request, the server can determine the
    browser from sec-ch-ua while other details must be derived from the
    user-agent.
    <br />
    If the server determines that the browser supports client hints, then
    it may request additional client hints headers by setting the
    Accept-CH header in the response.
    <br />
    Select the <strong>Make second request</strong> button below,
    to use send another request to the server. This time, any
    additional client hints headers that have been requested
    will be included.
</p>
<button type='button' onclick='redirect()'>Make second request</button>

<script>
    // This script will run when button will be clicked and device detection request will again
    // be sent to the server with all additional client hints that was requested in the previous
    // response by the server.
    // Following sequence will be followed.
    // 1. User will send the first request to the web server for detection.
    // 2. Web Server will return the properties in response based on the headers sent in the request. Along
    // with the properties, it will also send a new header field Accept-CH in response indicating the additional
    // evidence it needs. It builds the new response header using SetHeader[Component name]Accept-CH properties
    // where Component Name is the name of the component for which properties are required.
    // 3. When 'Make second request' button will be clicked, device detection request will again
    // be sent to the server with all additional client hints that was requested in the previous
    // response by the server.
    // 4. Web Server will return the properties based on the new User Agent Client Hint headers
    // being used as evidence.

    function redirect() {
        sessionStorage.reloadAfterPageLoad = true;
        window.location.reload(true);
    }

    window.onload = function () {
        if (sessionStorage.reloadAfterPageLoad) {
            document.getElementById('description').innerHTML = '<p>The information shown below is determined using <strong>User Agent Client Hints</strong> that was sent in the request to obtain additional evidence. If no additional evidence appears in the table above, then it may indicate an external problem such as <strong>User Agent Client Hints</strong> being disabled or unsupported by your browser.</p>';
            sessionStorage.reloadAfterPageLoad = false;
        }
        else {
            document.getElementById('description').innerHTML = '<p>The following values are determined by sever-side device detection on the first request.</p>';
        }
		var evidence = "} + fiftyonedegrees.evidence_used() + {";
		var table = document.getElementById('evidence');
		for (const [key, value] of Object.entries(evidence)) {
			var row = document.createElement('tr');
			row.innerHTML = '<td>' + key + '</td><td>' + value + '</td>';
			table.appendChild(row);
		}

    }
</script>

<div>
    <strong>Evidence values used:</strong>
    <table id='evidence'>
        <tr>
            <th>Key</th>
            <th>Value</th>
        </tr>
    </table>
</div>
<br />
<div id=description></div>
<div id='content'>
    <strong>Detection results:</strong>
    <p>
		Hardware Vendor: "} + fiftyonedegrees.match_all("HardwareVendor") + {"<br />
		Hardware Name: "} + fiftyonedegrees.match_all("HardwareName") + {"<br />
		Device Type: "} + fiftyonedegrees.match_all("DeviceType") + {"<br />
		Platform Vendor: "} + fiftyonedegrees.match_all("PlatformVendor") + {"<br />
		Platform Name: "} + fiftyonedegrees.match_all("PlatformName") + {"<br />
		Platform Version: "} + fiftyonedegrees.match_all("PlatformVersion") + {"<br />
		Browser Vendor: "} + fiftyonedegrees.match_all("BrowserVendor") + {"<br />
		Browser Name: "} + fiftyonedegrees.match_all("BrowserName") + {"<br />
		Browser Version: "} + fiftyonedegrees.match_all("BrowserVersion") + {"
    </p>
</div>"});
        return (deliver);
    }
}

sub vcl_init {
	fiftyonedegrees.start("/etc/varnish/51Degrees-LiteV4.1.hash");
}
