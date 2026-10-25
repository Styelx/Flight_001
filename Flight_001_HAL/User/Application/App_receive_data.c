#include "App_receive_data.h"

uint8_t rx_buff[TX_PLOAD_WIDTH] = {0};

extern Remote_Data remote_data;
extern Remote_State remote_state;
extern Flight_State flight_state;

extern uint16_t flight_height;

THR_STATE thr_state = FREE;
TickType_t enter_time = 0;
TickType_t leave_time = 0;

uint8_t retry_times = 0;

/***************************
 *@function:接收无线通讯数据
 *
 *@return:0 校验通过   1 校验失败
 ***************************/
uint8_t App_Receive_Data() {
    memset(rx_buff,0,TX_PLOAD_WIDTH);
    Int_SI24R1_RxPacket(rx_buff);

    //说明发送方没发送数据过来
    if (strlen((char *)rx_buff) == 0) {
        return 1;
    }

    //1. 帧头校验
    if (rx_buff[0] != FRAME_HEAD_CHECK_1 || rx_buff[1] != FRAME_HEAD_CHECK_2 || rx_buff[2] != FRAME_HEAD_CHECK_3) {
        return 1;
    }
    //2. 帧尾校验
    uint32_t sum = 0;
    uint32_t sum_receive = 0;
    sum_receive = rx_buff[13] << 24 | rx_buff[14] << 16 | rx_buff[15] << 8 | rx_buff[16];
    for (uint8_t i = 0; i < 13; i++) {
        sum += rx_buff[i];
    }
    if (sum_receive != sum) {
        return 1;
    }
    //3. 保存数据
    remote_data.thr = rx_buff[3] << 8 | rx_buff[4];
    remote_data.yaw = rx_buff[5] << 8 | rx_buff[6];
    remote_data.pitch = rx_buff[7] << 8 | rx_buff[8];
    remote_data.roll = rx_buff[9] << 8 | rx_buff[10];
    remote_data.shutdown = rx_buff[11];
    remote_data.fix_height = rx_buff[12];

    // debug_printf("thr:%d, yaw:%d, pitch:%d, roll:%d, shutdown:%d, fix_height:%d\n",remote_data.thr,remote_data.yaw,remote_data.pitch,remote_data.roll,remote_data.shutdown,remote_data.fix_height);
    return 0;
}

/***************************
 *@function:处理连接状态
 *
 *@return:App_Receive_Data的返回值
 ***************************/
void App_process_connect_state(uint8_t res) {
    if (res == 0) {
        remote_state = REMOTE_CONNECTED;
        retry_times = 0;
    }
    else if (res == 1) {
        retry_times++;
        if (retry_times == MAX_RETRY_TIMES) {
            remote_state = REMOTE_DISCONNECTED;
            retry_times = 0;
        }
    }
}

/***************************
 *@function:处理解锁
 *
 *@return:0 解锁  1未解锁
 ***************************/

static uint8_t App_process_unlock(void) {
    //安全考虑需要解锁油门才能启动,防止误启动
    switch (thr_state) {
        case FREE:
            if (remote_data.thr >= 900) {
                thr_state = MAX;
                enter_time = xTaskGetTickCount();
            }
            break;
        case MAX:
            if (remote_data.thr < 900) {
                //判断油门拉满时间是否满1s
                if (xTaskGetTickCount() - enter_time >= 1000) {
                    thr_state = LEAVE_MAX;
                }
                else {
                    thr_state = FREE;
                }
            }
            break;
        case LEAVE_MAX:
            if (remote_data.thr <= 100) {
                thr_state = MIN;
                leave_time = xTaskGetTickCount();
            }
            break;
        case MIN:
            if (xTaskGetTickCount() - leave_time <= 1000) {
                if (remote_data.thr > 100) {
                    thr_state = FREE;
                }
            }
            else {
                thr_state = UNLOCK;
            }
            break;
        case UNLOCK:
            return 0;
    }
    return 1;
}

/***************************
 *@function:处理飞行状态
 *
 *@return:App_Receive_Data的返回值
 ***************************/
void App_process_flight_state(void) {
    //状态机实现
    //轮询查询无人机飞行状态并更新
    switch (flight_state) {
        case IDLE:
            //IDLE->NORMAL
            if (App_process_unlock() == 0) {
                flight_state = NORMAL;
                //解锁后需要重置解锁状态
                thr_state = FREE;
            }
            break;
        case NORMAL:
            //NORMAL->FIX_HEIGHT
            if (remote_data.fix_height == 1) {
                flight_state = FIX_HEIGHT;
                remote_data.fix_height = 0;
            }
            //NORMAL->FAULT
            if (remote_state == REMOTE_DISCONNECTED) {
                flight_state = FAULT;
            }
            break;
        case FIX_HEIGHT:
            //FIX_HEIGHT->NORMAL
            if (remote_data.fix_height == 1) {
                flight_state = NORMAL;
                remote_data.fix_height = 0;

                //记录下当前高度
                flight_height = Int_VL53L1X_GetDistance();
            }
            //FIX_HEIGHT->FAULT
            if (remote_state == REMOTE_DISCONNECTED) {
                flight_state = FAULT;
            }
            break;
        case FAULT:
            //处理失联故障,缓慢停止电机降落
            //一直等待通知, 由飞行任务通知
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            flight_state = IDLE;
            break;
    }
}