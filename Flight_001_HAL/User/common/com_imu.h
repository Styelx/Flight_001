#ifndef FLIGHT_001_HAL_COM_IMU_H
#define FLIGHT_001_HAL_COM_IMU_H

#include "com_debug.h"
#include "com_config.h"
#include "math.h"

/***************************本项目没用到四元数***************************/
//表示四元数的结构体
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_Struct;

extern float RtA;
extern float Gyro_G;
extern float Gyro_Gr;

void Common_IMU_GetEulerAngle(Gyro_Accel_Data *gyroAccel, Euler_Data *eulerAngle, float dt);
float Common_IMU_GetNormAccZ(void);


#endif //FLIGHT_001_HAL_COM_IMU_H
/***************************本项目没用到四元数***************************/