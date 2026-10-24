#ifndef FLIGHT_001_HAL_COM_CONFIG_H
#define FLIGHT_001_HAL_COM_CONFIG_H

#include "main.h"

//连接状态
typedef enum {
    REMOTE_CONNECTED = 0,
    REMOTE_DISCONNECTED
}Remote_State;

//飞行状态
//空闲,飞行,定高,故障
typedef enum {
    IDLE = 0,
    NORMAL,
    FIX_HEIGHT,
    FAULT
}Flight_State;

//油门解锁状态
typedef enum {
    FREE = 0,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK
}THR_STATE;

//无线通讯接收数据
typedef struct {
    int16_t thr;            //油门
    int16_t yaw;            //偏航角
    int16_t pitch;          //俯仰角
    int16_t roll;           //横滚角
    uint8_t shutdown;       //开关机指令   1:关机    0:不关机
    uint8_t fix_height;     //定高        1:定高
}Remote_Data;

//MPU6050角速度
typedef struct {
    int16_t gyro_x;     //向右飞为正     横滚角
    int16_t gyro_y;     //向前飞为正     俯仰角
    int16_t gyro_z;     //逆时针为正     偏航角
}Gyroscope_Data;

//MPU6050角加速度
typedef struct {
    int16_t accel_x;    //向前为正
    int16_t accel_y;    //向左为正
    int16_t accel_z;    //向上为正
}Accelerometer_Data;

typedef struct {
    Gyroscope_Data gyro_data;
    Accelerometer_Data accel_data;
}Gyro_Accel_Data;

//欧拉角
typedef struct {
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
}Euler_Data;

#endif //FLIGHT_001_HAL_COM_CONFIG_H
