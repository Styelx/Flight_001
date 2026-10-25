/***********************
 *处理摇杆按键数据
 ***********************/
#include "App_process_data.h"

//摇杆数据结构体
Joystick_Struct joystick = {0,0,0,0};
//遥控数据结构体
Remote_Data remote_data = {0,0,0,0,0,0};

//摇杆偏移量
int16_t joystick_thr_offset = 0;
int16_t joystick_yaw_offset = 0;
int16_t joystick_pitch_offset = 0;
int16_t joystick_roll_offset = 0;

//按键偏移值
int16_t key_pitch_offset = 0;
int16_t key_roll_offset = 0;

//校准时必须保证油门在最低档位,其他方向档均回中
void App_calibrate_joystick(void) {

    int16_t thr_sum = 0;
    int16_t yaw_sum = 0;
    int16_t pitch_sum = 0;
    int16_t roll_sum = 0;

    key_pitch_offset = 0;
    key_roll_offset = 0;
    //多次读取求平均值
    for (uint8_t i = 0; i < 10; i++) {
        App_process_joystick_data();
        thr_sum += joystick.thr - 0;
        yaw_sum += joystick.yaw - 500;
        pitch_sum += joystick.pitch - 500;
        roll_sum += joystick.roll - 500;
        vTaskDelay(10);
    }
    joystick_thr_offset += thr_sum / 10;
    joystick_yaw_offset += yaw_sum / 10;
    joystick_pitch_offset += pitch_sum / 10;
    joystick_roll_offset += roll_sum / 10;
}


void App_process_key_data(void) {
    KEY_TYPE key = Int_Key_Get();
    if (key == KEY_UP) {
        //向前飞微调 => 机头下压,俯仰角减小
        key_pitch_offset += 10;
    }
    else if (key == KEY_DOWN) {
        //向后飞微调 => 机头抬起,俯仰角增大
        key_pitch_offset -= 10;
    }
    else if (key == KEY_LEFT) {
        key_roll_offset -= 10;
    }
    else if (key == KEY_RIGHT) {
        key_roll_offset += 10;
    }
    else if (key == KEY_LEFT_X) {
        //
        remote_data.shutdown = 1;
    }
    else if (key == KEY_RIGHT_X) {
        remote_data.fix_height = 1;
    }
    else if (key == KEY_RIGHT_X_LONG) {
        //校准摇杆
        //校准时必须保证油门在最低档位,其他方向档均回中
        App_calibrate_joystick();
    }
}

void App_process_joystick_data(void) {
    //摇杆和按键任务优先级相等,设置临界区防止任务跳转
    taskENTER_CRITICAL();

    //1. 获取摇杆ADC转换值
    Int_joystick_get(&joystick);

    //2. 处理极性和范围 => 使用范围0-1000
    joystick.thr = 1000 - (joystick.thr * 1000 / 4095);
    joystick.yaw = 1000 - (joystick.yaw * 1000 / 4095);
    joystick.pitch = 1000 - (joystick.pitch * 1000 / 4095);
    joystick.roll = 1000 - (joystick.roll * 1000 / 4095);

    //3. 处理ADC误差  必须保证在初始状态时启动校准
    joystick.thr -= joystick_thr_offset;
    joystick.yaw -= joystick_yaw_offset;
    joystick.pitch -= joystick_pitch_offset;
    joystick.roll -= joystick_roll_offset;

    //4. 按键微调
    joystick.pitch += key_pitch_offset;
    joystick.roll += key_roll_offset;

    //5. 防止超出最大最小值
    joystick.thr = com_limit(joystick.thr,0,1000);
    joystick.yaw = com_limit(joystick.yaw,0,1000);
    joystick.pitch = com_limit(joystick.pitch,0,1000);
    joystick.roll = com_limit(joystick.roll,0,1000);

    taskEXIT_CRITICAL();

    //6. ADC数据调整完成
    remote_data.thr = joystick.thr;
    remote_data.yaw = joystick.yaw;
    remote_data.pitch = joystick.pitch;
    remote_data.roll = joystick.roll;
}