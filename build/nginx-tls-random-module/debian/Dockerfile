FROM debian:buster-slim AS build

RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/security.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
    build-essential \
    libpcre3-dev \
    zlib1g-dev \
    libssl-dev \
    openssl

COPY ./nginx /opt/nginx
COPY ./modules /opt/modules

WORKDIR /opt/nginx

RUN ./auto/configure --prefix=/usr/local/nginx  \
    --add-dynamic-module=../modules/nginx-tls-random-module  \
    --with-http_ssl_module  \
    --with-http_stub_status_module  \
    --with-http_v2_module  \
    --conf-path=/etc/nginx/nginx.conf && \
    make && make install

FROM debian:buster-slim

RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/security.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
    libpcre3 \
    zlib1g \
    libssl1.1 && \
    rm -rf /var/lib/apt/lists/*

COPY --from=build /usr/local/nginx /usr/local/nginx
COPY --from=build /etc/nginx /etc/nginx

RUN mkdir -p /etc/nginx/conf.d && \
    mkdir -p /var/log/nginx

EXPOSE 80
CMD ["/usr/local/nginx/sbin/nginx","-g","daemon off;"]
