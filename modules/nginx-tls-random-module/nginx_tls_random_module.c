#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

extern ngx_module_t ngx_tls_random_module;
static ngx_int_t ngx_tls_random_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_tls_server_random_variable(
        ngx_http_request_t *r,
        ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_tls_client_random_variable(
        ngx_http_request_t *r,
        ngx_http_variable_value_t *v, uintptr_t data);


static ngx_http_module_t ngx_tls_random_module_ctx = {
        ngx_tls_random_add_variables,       /* preconfiguration */
        NULL,                               /* postconfiguration */

        NULL,                               /* create main configuration */
        NULL,                               /* init main configuration */

        NULL,                               /* create server configuration */
        NULL,                               /* merge server configuration */

        NULL,                               /* create location configuration */
        NULL                                /* merge location configuration */
};


static ngx_command_t ngx_tls_random_commands[] = {
        ngx_null_command
};


ngx_module_t ngx_tls_random_module = {
        NGX_MODULE_V1,
        &ngx_tls_random_module_ctx,            /* module context */
        ngx_tls_random_commands,               /* module directives */
        NGX_HTTP_MODULE,                       /* module type */
        NULL,                                  /* init master */
        NULL,                                  /* init module */
        NULL,                                  /* init process */
        NULL,                                  /* init thread */
        NULL,                                  /* exit thread */
        NULL,                                  /* exit process */
        NULL,                                  /* exit master */
        NGX_MODULE_V1_PADDING
};


static ngx_http_variable_t  ngx_tls_random_vars[] = {
        { ngx_string("tls_server_random"), NULL, ngx_tls_server_random_variable, 0,
          NGX_HTTP_VAR_CHANGEABLE|NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

        { ngx_string("tls_client_random"), NULL, ngx_tls_client_random_variable, 0,
          NGX_HTTP_VAR_CHANGEABLE|NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

        ngx_http_null_variable
};


static ngx_int_t
ngx_tls_random_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t  *var, *v;

    for (v = ngx_tls_random_vars; v->name.len; v++) {
        var = ngx_http_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}


static ngx_int_t
ngx_tls_server_random_variable(ngx_http_request_t *r,
   ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->connection->ssl != NULL) {
        unsigned char out[32];  // ServerRandom的长度是32字节
        size_t random_len = SSL_get_server_random(r->connection->ssl->connection, out, sizeof(out));

        if (random_len != sizeof(out)) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to get ServerRandom");
        } else {
            // 将ServerRandom转换为十六进制格式
            char hex_output[65];  // 加1个字符用于存储null终止符
            for (size_t i = 0; i < random_len; i++) {
                sprintf(hex_output + i * 2, "%02x", out[i]);
            }
            v->len = sizeof(hex_output) - 1;
            v->valid = 1;
            v->no_cacheable = 0;
            v->not_found = 0;
            v->data = (u_char *) ngx_pnalloc(r->pool, sizeof(hex_output));
            ngx_memcpy(v->data, hex_output, sizeof(hex_output));
        }
    } else {
        v->len = 0;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 1;
    }
    return NGX_OK;
}



static ngx_int_t
ngx_tls_client_random_variable(ngx_http_request_t *r,
   ngx_http_variable_value_t *v, uintptr_t data)
{
    if (r->connection->ssl != NULL) {
        unsigned char out[32];
        size_t random_len = SSL_get_client_random(r->connection->ssl->connection, out, sizeof(out));

        if (random_len != sizeof(out)) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to get ClientRandom");
        } else {
            char hex_output[65];
            for (size_t i = 0; i < random_len; i++) {
                sprintf(hex_output + i * 2, "%02x", out[i]);
            }
            v->len = sizeof(hex_output) - 1;
            v->valid = 1;
            v->no_cacheable = 0;
            v->not_found = 0;
            v->data = (u_char *) ngx_pnalloc(r->pool, sizeof(hex_output));
            ngx_memcpy(v->data, hex_output, sizeof(hex_output));
        }
    } else {
        v->len = 0;
        v->valid = 1;
        v->no_cacheable = 0;
        v->not_found = 1;
    }
    return NGX_OK;
}
