#ifndef FLIGHT_001_HAL_APP_FREERTOS_TASK_H
#define FLIGHT_001_HAL_APP_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "com_config.h"
#include "com_debug.h"
#include "Int_IP5305T.h"
#include "Int_motor.h"
#include "Int_led.h"
#include "Int_SI24R1.h"

/*
 *@启动FreeRTOS
 *
 */

void App_FreeRTOS_start(void);

#endif //FLIGHT_001_HAL_APP_FREERTOS_TASK_H
