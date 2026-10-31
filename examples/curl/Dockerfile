FROM ubuntu:24.04
WORKDIR /app

ARG VARNISH_VERSION=8.0.1

RUN apt-get update && apt-get install --no-install-recommends -y git git-lfs curl ca-certificates \
	build-essential automake autoconf libtool pkg-config cpio libedit-dev libncurses-dev \
	python3 python3-docutils libpcre2-dev libpcre3-dev

RUN curl -fLsS -o /tmp/varnish.tgz https://varnish-cache.org/downloads/varnish-${VARNISH_VERSION}.tgz \
	&& mkdir /tmp/varnish && tar xzf /tmp/varnish.tgz --strip-components=1 -C /tmp/varnish \
	&& cd /tmp/varnish && ./configure --without-jemalloc --with-sphinx-build=true && make -j4 && make install && ldconfig \
	&& rm -rf /tmp/varnish /tmp/varnish.tgz

RUN git clone --recurse-submodules https://github.com/51Degrees/51degrees-varnish .

RUN ./autogen.sh && ./configure && make -j4 && make install

COPY default.vcl /app/default.vcl

EXPOSE 8080

CMD ["varnishd", "-F", "-a", "0.0.0.0:8080", "-f", "/app/default.vcl"]
