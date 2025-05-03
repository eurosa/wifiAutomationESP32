#include "system_manager.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_idf_version.h"

static const char* TAG = "SystemManager";

void system_manager_init() {
    esp_err_t ret;
    
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        // ESP-IDF v5.0+ uses a configuration structure
        esp_task_wdt_config_t twdt_config = {
            .timeout_ms = 30000,       // 30 second timeout
            .idle_core_mask = 0,       // Which cores to monitor (0 = all)
            .trigger_panic = true      // Trigger panic when timeout occurs
        };
        ret = esp_task_wdt_init(&twdt_config);  // Note the address-of operator &
    #elif ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
        // ESP-IDF v4.0-v4.4 uses simple timeout in seconds
        ret = esp_task_wdt_init(30);  // 30 seconds
    #else
        // Legacy versions (v3.x)
        ret = esp_task_wdt_init(30, true);  // 30 seconds, panic enabled
    #endif

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Task WDT: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Task Watchdog Timer initialized (30s timeout)");
    }
}

void system_manager_deinit() {
    esp_task_wdt_deinit();
    ESP_LOGI(TAG, "Task Watchdog Timer deinitialized");
}