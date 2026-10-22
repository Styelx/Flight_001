#ifndef REMOTE_001_HAL_JOYSTICK_H
#define REMOTE_001_HAL_JOYSTICK_H

#include "adc.h"

typedef struct {
    int16_t thr;
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
}Joystick_Struct;

void Int_joystick_init(void);
void Int_joystick_get(Joystick_Struct *joystick);

#endif //REMOTE_001_HAL_JOYSTICK_H
