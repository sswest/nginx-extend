# Nginx Extras

## Nginx TLS Random Module

This repository contains the Nginx TLS Random Module which is an extension for Nginx to extract and display the `client_random` and `server_random` used in the TLS protocol. These random values are key components in the encryption process and can be useful for debugging and analysis purposes.

### Module Functionality

The `nginx-tls-random-module` hooks into the SSL handshake process and captures the `client_random` and `server_random` values. It provides several variables that can be used in the Nginx configuration to obtain the random keys:

* `tls_server_random`
* `tls_client_random`

### Usage

After the module has been compiled and installed, you can enable it in your Nginx configuration file:

```nginx
load_module /usr/local/nginx/modules/ngx_tls_random_module.so;

http {
    
    server {
        listen 443 ssl http2;
        server_name my.org;
    
        ssl_certificate /etc/nginx/ssl/my.pem;
        ssl_certificate_key /etc/nginx/ssl/my.key;
    
        location / {
            proxy_pass http://backend:8000;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-ServerRandom $tls_server_random;
            proxy_set_header X-ClientRandom $tls_client_random;
        }
    }
}
```

Each time a TLS connection is established, the `client_random` and `server_random` values are passed to the backend program through the request headers.

### Building

This repository includes a Dockerfile which uses a multi-stage build process to compile and install Nginx along with the `nginx-tls-random-module`.

To build the Docker image, navigate to the root directory of the repository and run:

```bash
docker build -t ngx:debian -f build/nginx-tls-random-module/debian/Dockerfile .
```

If you prefer Alpine, you can build an Alpine image with the following command:

```bash
docker build -t ngx:alpine -f build/nginx-tls-random-module/alpine/Dockerfile .
```

This will create a Docker image named `ngx:debian` which you can run using:

```bash
docker run -p 80:80 ngx:debian
```

**Please note that the Nginx image built by the Dockerfile is intended for development environments only. If you want to use it in a production environment, it is recommended to extract `/usr/local/nginx/modules/ngx_tls_random_module.so` from the image, and dynamically load it in the production environment.**

## Contributing

Contributions to this project are welcome. Please open a pull request or issue on the GitHub repository.
