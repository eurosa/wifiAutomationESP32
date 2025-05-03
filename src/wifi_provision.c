#include "wifi_provision.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "string.h"

static const char *TAG = "WiFi Provision";
static esp_netif_t *sta_netif = NULL;

// Define the event handler functions
static void wifi_event_handler(void *arg, esp_event_base_t event_base, 
                             int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WiFi STA started");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WiFi connected to AP");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WiFi disconnected from AP");
                // Attempt to reconnect
                esp_wifi_connect();
                break;
            default:
                break;
        }
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data) {
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta() {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // Create default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Initialize TCP/IP stack
    esp_netif_init();
    
    // Create default WiFi STA
    sta_netif = esp_netif_create_default_wifi_sta();
    
    // WiFi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, 
        ESP_EVENT_ANY_ID, 
        &wifi_event_handler, 
        NULL, 
        NULL));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, 
        IP_EVENT_STA_GOT_IP, 
        &ip_event_handler, 
        NULL, 
        NULL));
    
    // Set WiFi mode and start
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WiFi station initialized");
}

bool is_wifi_connected() {
    return esp_netif_is_netif_up(sta_netif);
}

char* get_device_ip() {
    if (!sta_netif) return NULL;
    
    esp_netif_ip_info_t ip_info;
    if (esp_netif_get_ip_info(sta_netif, &ip_info) == ESP_OK) {
        char *ip_str = malloc(16); // Enough for "xxx.xxx.xxx.xxx"
        if (ip_str) {
            sprintf(ip_str, IPSTR, IP2STR(&ip_info.ip));
            return ip_str;
        }
    }
    return NULL;
}

void wifi_deinit_sta() {
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_destroy_default_wifi(sta_netif);
    esp_event_loop_delete_default();
}

void wifi_configure_sta(const char *ssid, const char *password) {
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
}