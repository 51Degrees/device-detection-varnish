#!/bin/sh
# Example requests against the VCL in this folder. Each one sends its evidence as
# headers, and the values come back as response headers.

HOST=127.0.0.1:8080

echo "=== Device detection: desktop browser with client hints ==="
curl --head \
	-H 'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36 Edg/114.0.1823.41' \
	-H 'Sec-CH-UA: "Not.A/Brand";v="8","Chromium";v="114","Microsoft Edge";v="114"' \
	-H 'Sec-CH-UA-Arch: x86' \
	-H 'Sec-CH-UA-Bitness: 64' \
	-H 'Sec-CH-UA-Full-Version: 114.0.5735.110' \
	-H 'Sec-CH-UA-Full-Version-List: "Not.A/Brand";v="8.0.0.0","Chromium";v="114.0.5735.110","Microsoft Edge";v="114.0.1823.41"' \
	-H 'Sec-CH-UA-Mobile: ?0' \
	-H 'Sec-CH-UA-Model: ' \
	-H 'Sec-CH-UA-Platform: Windows' \
	-H 'Sec-CH-UA-Platform-Version: 15.0.0' \
	"http://$HOST/"

echo "=== IP Intelligence: an address in a header ==="
curl --head -H 'X-Real-IP: 212.58.224.22' "http://$HOST/"

echo "=== Both: a mobile browser and an address together ==="
curl --head \
	-H 'User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 Safari/9537.53' \
	-H 'X-Real-IP: 212.58.224.22' \
	"http://$HOST/"
