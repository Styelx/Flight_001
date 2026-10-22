#ifndef REMOTE_001_HAL_APP_PROCESS_DATA_H
#define REMOTE_001_HAL_APP_PROCESS_DATA_H

#include "FreeRTOS.h"
#include "task.h"
#include "Int_KEY.h"
#include "Int_joystick.h"
#include "com_tool.h"
#include "com_debug.h"

typedef struct {
    int16_t thr;            //油门
    int16_t yaw;            //偏航角
    int16_t pitch;          //俯仰角
    int16_t roll;           //横滚角
    uint8_t shutdown;       //开关机指令   1:关机    0:不关机
    uint8_t fix_height;     //定高        1:定高

}Remote_Data;

extern Remote_Data remote_data;

//处理按键数据 => 根据键值记录对应功能
void App_process_key_data(void);

//处理摇杆数据 => 根据ADC数值记录对应航行数据
void App_process_joystick_data(void);

#endif //REMOTE_001_HAL_APP_PROCESS_DATA_H
