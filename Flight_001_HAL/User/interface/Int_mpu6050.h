#ifndef FLIGHT_001_HAL_INT_MPU6050_H
#define FLIGHT_001_HAL_INT_MPU6050_H

#include "FreeRTOS.h"
#include "task.h"
#include "stdlib.h"
#include "i2c.h"
#include "com_config.h"
#include "com_debug.h"

//从设备地址
#define MPU6050_ADDRESS 0x68
//读写地址
#define MPU6050_ADDR_WRITE 0xD0
#define MPU6050_ADDR_READ 0xD1

//MPU6050寄存器地址
#define SMPRT_DIV 0x19
#define CONFIG_6050 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define INT_ENABLE 0x38

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define USER_CTRL 0x6A
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

/****************************
 *@function:零偏校准
 *
 *
 ****************************/
void Int_MPU6050_calculate_offset(void);

/****************************
 *@function:初始化MPU6050
 *
 *
 ****************************/
void Int_MPU6050_Init(void);

/****************************
 *@function:读取角速度
 *
 *
 ****************************/
void Int_MPU6050_Get_Gyro(Gyroscope_Data *gyroscope_data);

/****************************
 *@function:读取角加速度
 *
 *
 ****************************/
void Int_MPU6050_Get_Accel(Accelerometer_Data *accelerometer_data);

/****************************
 *@function:归并角速度和角加速度
 *
 *
 ****************************/
void Int_MPU6050_Get_ALL(Gyro_Accel_Data *gyro_accel_data);

#endif //FLIGHT_001_HAL_INT_MPU6050_H
