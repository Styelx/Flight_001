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

//无线通讯接收数据
typedef struct {
    int16_t thr;            //油门
    int16_t yaw;            //偏航角
    int16_t pitch;          //俯仰角
    int16_t roll;           //横滚角
    uint8_t shutdown;       //开关机指令   1:关机    0:不关机
    uint8_t fix_height;     //定高        1:定高
}Remote_Data;

//油门解锁状态
typedef enum {
    FREE = 0,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK
}THR_STATE;
#endif //FLIGHT_001_HAL_COM_CONFIG_H
