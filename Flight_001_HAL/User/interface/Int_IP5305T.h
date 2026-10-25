#ifndef FLIGHT_001_HAL_INT_IP5305T_H
#define FLIGHT_001_HAL_INT_IP5305T_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

//启动IP5305T,防止自动关机
void Int_IP5305T_start(void);
void Int_IP5305T_stop(void);

#endif //FLIGHT_001_HAL_INT_IP5305T_H
