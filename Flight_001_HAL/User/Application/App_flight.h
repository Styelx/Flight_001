#ifndef FLIGHT_001_HAL_APP_FLIGHT_H
#define FLIGHT_001_HAL_APP_FLIGHT_H

#include "math.h"

#include "FreeRTOS.h"
#include "task.h"
#include "com_debug.h"
#include "com_filter.h"
#include "com_imu.h"
#include "com_pid.h"
#include "Int_mpu6050.h"
#include "Int_motor.h"
#include "Int_VL53L1X.h"


/*****************************
 *@function:初始化MPU6050和电机
 *****************************/
void App_flight_Init(void);

/*****************************
 *@function:根据陀螺仪数据计算欧拉角
 *****************************/
void App_flight_gyro_accel_euler(void);

/*****************************
 *@function:根据欧拉角, 计算PID
 *****************************/
void App_pid_process(void);

/*****************************
 *@function:根据PID输出控制电机
 *****************************/
void App_flight_control_motor(void);

/*****************************
 *@function:定高功能PID
 *****************************/
void App_flight_fix_height(void);

#endif //FLIGHT_001_HAL_APP_FLIGHT_H
