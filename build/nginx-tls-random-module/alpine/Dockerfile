FROM alpine:3.13 AS build

RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories && \
    apk update && apk add --no-cache openssl gcc g++ make linux-headers openssl-dev pcre-dev zlib-dev

COPY ./nginx /opt/nginx
COPY ./modules /opt/modules

WORKDIR /opt/nginx

RUN auto/configure --prefix=/usr/local/nginx  \
    --add-dynamic-module=../modules/nginx-tls-random-module  \
    --with-http_ssl_module  \
    --with-http_stub_status_module  \
    --with-http_v2_module  \
    --conf-path=/etc/nginx/nginx.conf && \
    make && make install

FROM alpine:3.13

RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories && \
     apk add --no-cache pcre zlib openssl
COPY --from=build /usr/local/nginx /usr/local/nginx
COPY --from=build /etc/nginx /etc/nginx

RUN mkdir -p /etc/nginx/conf.d && \
    mkdir -p /var/log/nginx

EXPOSE 80
CMD ["/usr/local/nginx/sbin/nginx","-g","daemon off;"]
