#ifndef FLIGHT_001_HAL_INT_MOTOR_H
#define FLIGHT_001_HAL_INT_MOTOR_H

#include "tim.h"
#include "com_debug.h"

typedef struct {
    TIM_HandleTypeDef *tim;
    uint16_t channel;
    uint16_t speed;
}Motor_Struct;

//传入参数为比较值,实际占空比为speed/1000,默认speed=200
void Int_motor_set_speed(Motor_Struct *motor);
void Int_motor_start(Motor_Struct *motor);

#endif //FLIGHT_001_HAL_INT_MOTOR_H
