#ifndef FLIGHT_001_HAL_INT_LED_H
#define FLIGHT_001_HAL_INT_LED_H

#include "main.h"

typedef struct {
    GPIO_TypeDef * port;
    uint16_t pin;
}LED_Struct;

void Int_led_turn_on(LED_Struct* led);
void Int_led_turn_off(LED_Struct* led);
void Int_led_toggle(LED_Struct* led);


#endif //FLIGHT_001_HAL_INT_LED_H
