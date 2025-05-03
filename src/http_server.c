#include "http_server.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "relay_control.h"
#include "wifi_provision.h"
#include "cJSON.h"  // Add this line

static const char *TAG = "HTTP Server";
static httpd_handle_t server = NULL;

static esp_err_t root_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    FILE *file = fopen("/data/config.html", "r");
    if (file) {
        char buffer[256];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            httpd_resp_send_chunk(req, buffer, bytes_read);
        }
        fclose(file);
        httpd_resp_send_chunk(req, NULL, 0);
    } else {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
    }
    return ESP_OK;
}

static esp_err_t api_relay_handler(httpd_req_t *req) {
    char buf[10];
    int ret = httpd_req_recv(req, buf, sizeof(buf)-1);
    if (ret <= 0) return ESP_FAIL;
    
    buf[ret] = '\0';
    if (strcmp(buf, "on") == 0) {
        relay_set_state(true);
    } else if (strcmp(buf, "off") == 0) {
        relay_set_state(false);
    } else if (strcmp(buf, "toggle") == 0) {
        relay_toggle();
    }
    
    httpd_resp_set_type(req, "application/json");
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddBoolToObject(response, "state", relay_get_state());
    const char *json_str = cJSON_Print(response);
    httpd_resp_sendstr(req, json_str);
    free((void*)json_str);
    cJSON_Delete(response);
    return ESP_OK;
}

void start_http_server() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root_uri);
        
        httpd_uri_t relay_uri = {
            .uri = "/api/relay",
            .method = HTTP_POST,
            .handler = api_relay_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &relay_uri);
        
        ESP_LOGI(TAG, "HTTP server started");
    }
}

void stop_http_server() {
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "HTTP server stopped");
    }
}