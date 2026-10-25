#include "Int_led.h"

void Int_led_turn_on(LED_Struct* led) {
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
}

void Int_led_turn_off(LED_Struct* led) {
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
}

void Int_led_toggle(LED_Struct* led) {
    HAL_GPIO_TogglePin(led->port, led->pin);
}