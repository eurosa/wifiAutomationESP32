#pragma once
#include <stdbool.h>

#define RELAY_GPIO_PIN 4

void relay_init();
void relay_set_state(bool state);
bool relay_get_state();
void relay_toggle();