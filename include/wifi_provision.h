#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_sta();
bool is_wifi_connected();
char* get_device_ip();

#ifdef __cplusplus
}
#endif