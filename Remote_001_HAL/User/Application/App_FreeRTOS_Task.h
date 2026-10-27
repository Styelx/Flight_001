#ifndef FLIGHT_001_HAL_APP_FREERTOS_TASK_H
#define FLIGHT_001_HAL_APP_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "com_debug.h"
#include "Int_IP5305T.h"
#include "Int_SI24R1.h"
#include "App_process_data.h"
#include "App_transmit_data.h"
#include "App_oled.h"

/*********************
 *@启动FreeRTOS
 *********************/

void App_FreeRTOS_start(void);

#endif //FLIGHT_001_HAL_APP_FREERTOS_TASK_H
