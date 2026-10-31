#!/bin/sh
docker build -t 51d-varnish-curl .
docker run -d -p 8080:8080 51d-varnish-curl
sleep 2
./curl.sh
