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
    FAIL
}Flight_State;

#endif //FLIGHT_001_HAL_COM_CONFIG_H
