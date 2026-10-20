#include "Int_motor.h"

void Int_motor_set_speed(Motor_Struct *motor) {
    if (motor->speed > 1000) {
        debug_printf("error:speed is too big\r\n");
        return;
    }
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, motor->speed);
}

void Int_motor_start(Motor_Struct *motor) {
    HAL_TIM_PWM_Start(motor->tim, motor->channel);
}