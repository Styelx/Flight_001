#ifndef FLIGHT_001_HAL_APP_FLIGHT_H
#define FLIGHT_001_HAL_APP_FLIGHT_H

#include "FreeRTOS.h"
#include "task.h"
#include "com_debug.h"
#include "com_filter.h"
#include "Int_mpu6050.h"

/*****************************
 *@function:根据陀螺仪数据计算欧拉角
 *
 *
 *****************************/
void App_flight_get_euler_angle(void);

#endif //FLIGHT_001_HAL_APP_FLIGHT_H
