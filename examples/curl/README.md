# cURL Example

This example shows how to send evidence to the Varnish modules with cURL, and see the values which come back.

## Build

There is a Docker image with Varnish and both modules, which is convenient for testing on a machine without Varnish. Run `./run.sh` from this folder, or:

```sh
docker build -t 51d-varnish-curl .
docker run -d -p 8080:8080 51d-varnish-curl
```

## Run

Use the `curl.sh` script, or a plain cURL command with any evidence you want to try:

```sh
curl --head -H 'X-Real-IP: 212.58.224.22' \
     -H 'User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 Safari/9537.53' \
     http://127.0.0.1:8080/
```

The values are in the response headers, such as `x-is-mobile: True` and `x-asn: \"BBC\":1`.

## Find out more

The engine this example uses: https://github.com/51Degrees/51degrees-cxx

The Varnish modules: https://github.com/51Degrees/51degrees-varnish

[Device detection](https://51degrees.com/products/device-detection?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-readme-md&utm_term=device-detection), [IP Intelligence](https://51degrees.com/products/ip-intelligence?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-readme-md&utm_term=ip-intelligence) and [data files and pricing](https://51degrees.com/pricing?utm_source=github&utm_medium=example&utm_campaign=51degrees-varnish&utm_content=examples-curl-readme-md&utm_term=pricing).
