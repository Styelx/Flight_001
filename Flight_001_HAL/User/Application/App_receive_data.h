#ifndef FLIGHT_001_HAL_APP_RECEIVE_DATA_H
#define FLIGHT_001_HAL_APP_RECEIVE_DATA_H

#include "FreeRTOS.h"
#include "task.h"
#include "Int_SI24R1.h"
#include "com_config.h"
#include "com_debug.h"
#include "Int_VL53L1X.h"

//帧头校验位
#define FRAME_HEAD_CHECK_1 'C'
#define FRAME_HEAD_CHECK_2 'J'
#define FRAME_HEAD_CHECK_3 'X'

//最大重试次数
#define MAX_RETRY_TIMES 10

/***************************
 *@function:接收无线通讯数据
 *
 *@return:0 校验通过   1 校验失败
 ***************************/
uint8_t App_Receive_Data(void);

/***************************
 *@function:处理连接状态
 *
 *@return:App_Receive_Data的返回值
 ***************************/
void App_process_connect_state(uint8_t res);


/***************************
 *@function:处理飞行状态
 *
 *@return:App_Receive_Data的返回值
 ***************************/
void App_process_flight_state(void);

#endif //FLIGHT_001_HAL_APP_RECEIVE_DATA_H
