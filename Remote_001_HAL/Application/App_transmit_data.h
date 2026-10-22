#ifndef REMOTE_001_HAL_APP_TRANSMIT_DATA_H
#define REMOTE_001_HAL_APP_TRANSMIT_DATA_H

#include "FreeRTOS.h"
#include "task.h"
#include "Int_SI24R1.h"
#include "App_process_data.h"

//帧头校验三字节
#define FRAME_HEAD_CHECK_1 'C'
#define FRAME_HEAD_CHECK_2 'J'
#define FRAME_HEAD_CHECK_3 'X'

void App_transmit_data(void);

#endif //REMOTE_001_HAL_APP_TRANSMIT_DATA_H
