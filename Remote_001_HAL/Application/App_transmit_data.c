#include "App_transmit_data.h"

uint8_t transmit_buff[TX_PLOAD_WIDTH] = {0};

void App_transmit_data(void) {
    uint32_t sum = 0;
    //1. 切换为发送模式
    Int_SI24R1_TX_Mode();

    //帧头校验设置为3字节
    transmit_buff[0] = FRAME_HEAD_CHECK_1;
    transmit_buff[1] = FRAME_HEAD_CHECK_2;
    transmit_buff[2] = FRAME_HEAD_CHECK_3;

    //数据包 remote_data 占 2+2+2+2+1+1 = 10 Bytes, int16_t类型高八位在前
    transmit_buff[3] = (uint8_t)((remote_data.thr >> 8) & 0xFF);
    transmit_buff[4] = (uint8_t)(remote_data.thr & 0xFF);

    transmit_buff[5] = (uint8_t)((remote_data.yaw >> 8) & 0xFF);
    transmit_buff[6] = (uint8_t)(remote_data.yaw & 0xFF);

    transmit_buff[7] = (uint8_t)((remote_data.pitch >> 8) & 0xFF);
    transmit_buff[8] = (uint8_t)(remote_data.pitch & 0xFF);

    transmit_buff[9] = (uint8_t)((remote_data.roll >> 8) & 0xFF);
    transmit_buff[10] = (uint8_t)(remote_data.roll & 0xFF);

    //设置临界区,防止在切换状态任务到来时正好在清0
    taskENTER_CRITICAL();
    transmit_buff[11] = remote_data.shutdown;
    remote_data.shutdown = 0;
    transmit_buff[12] = remote_data.fix_height;
    remote_data.fix_height = 0;
    taskEXIT_CRITICAL();

    //帧尾校验设置4字节,使用校验和形式
    for (uint8_t i = 0; i < 13; i++) {
        sum += transmit_buff[i];
    }
    transmit_buff[13] = (uint8_t)((sum >> 24) & 0xFFFF);
    transmit_buff[14] = (uint8_t)((sum >> 16) & 0xFFFF);
    transmit_buff[15] = (uint8_t)((sum >> 8) & 0xFFFF);
    transmit_buff[16] = (uint8_t)(sum & 0xFF);

    //2. 发送数据
    Int_SI24R1_TxPacket(transmit_buff);

    //3. 切换回接收模式
    Int_SI24R1_RX_Mode();
}