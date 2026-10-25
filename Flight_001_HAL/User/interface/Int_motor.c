#include "Int_motor.h"

void Int_motor_set_speed(Motor_Struct *motor) {
    if (motor->speed > 1000) {
        debug_printf("error:speed is too big\r\n");
        return;
    }
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, motor->speed);
}

void Int_motor_start(Motor_Struct *motor) {
    //防止初始化的时候转一下
    __HAL_TIM_SET_COMPARE(motor->tim, motor->channel, 0);
    HAL_TIM_PWM_Start(motor->tim, motor->channel);
}

int16_t Int_motor_limit_speed(int16_t speed,int16_t min,int16_t max) {
    if (speed > max) {
        return max;
    }
    else if (speed < min) {
        return min;
    }
    else {
        return speed;
    }
}