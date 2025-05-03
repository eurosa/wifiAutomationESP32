#include "cloud_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "wifi_provision.h"

static const char *TAG = "Cloud Client";
static const char *CLOUD_URL = "https://api.example.com/device";

void cloud_client_init() {
    // Initialize any required resources
}

void cloud_client_send_data(const char *data) {
    if (!is_wifi_connected()) return;
    
    esp_http_client_config_t config = {
        .url = CLOUD_URL,
        .cert_pem = (const char *)"/data/ca_cert.pem",
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, data, strlen(data));
    
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Status = %d", esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Error sending data: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}

void cloud_client_loop() {
    // Periodic tasks for cloud communication
}