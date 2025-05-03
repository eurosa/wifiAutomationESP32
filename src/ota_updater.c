#include "ota_updater.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "wifi_provision.h"

static const char *TAG = "OTA Updater";

void ota_update_init() {
    // Initialization if needed
}

void ota_update_check() {
    if (!is_wifi_connected()) return;
    
    // Check for updates periodically
}

void ota_update_start(const char *url) {
    esp_http_client_config_t config = {
        .url = url,
        .cert_pem = (const char *)"/data/ca_cert.pem",
    };
    
    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Update successful, restarting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "OTA update failed: %s", esp_err_to_name(ret));
    }
}