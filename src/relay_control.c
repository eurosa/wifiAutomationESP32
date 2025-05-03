#include "relay_control.h"
#include "driver/gpio.h"

void relay_init() {
    gpio_set_direction(RELAY_GPIO_PIN, GPIO_MODE_OUTPUT);
    relay_set_state(false);
}

void relay_set_state(bool state) {
    gpio_set_level(RELAY_GPIO_PIN, state ? 1 : 0);
}

bool relay_get_state() {
    return gpio_get_level(RELAY_GPIO_PIN) == 1;
}

void relay_toggle() {
    relay_set_state(!relay_get_state());
}