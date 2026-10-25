#include "Int_joystick.h"

int16_t adc_buff[4] = {0};

void Int_joystick_init(void) {
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff,4);
}

void Int_joystick_get(Joystick_Struct *joystick) {
    joystick->thr = adc_buff[0];
    joystick->yaw = adc_buff[1];
    joystick->pitch = adc_buff[2];
    joystick->roll = adc_buff[3];
}