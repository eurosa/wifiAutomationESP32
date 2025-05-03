#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "relay_control.h"
#include "wifi_provision.h"
#include "http_server.h"
#include "cloud_client.h"
#include "ota_updater.h"

static const char *TAG = "Main";

void app_main() {
    // Initialize components
    relay_init();
    wifi_init_sta();
    
    // Wait for WiFi connection
    while (!is_wifi_connected()) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    char *ip = get_device_ip();
    ESP_LOGI(TAG, "Connected with IP: %s", ip);
    free(ip);
    
    start_http_server();
    cloud_client_init();
    ota_update_init();
    
    // Main loop
    while (1) {
        cloud_client_loop();
        ota_update_check();
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}