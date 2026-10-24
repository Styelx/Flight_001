#include "App_FreeRTOS_Task.h"

//STM32F103C8T6 SRAM 20k => FreeRTOS 12k

//电机结构体定义
Motor_Struct left_top_motor = {&htim3, TIM_CHANNEL_1, 200};
Motor_Struct left_bottom_motor = {&htim4, TIM_CHANNEL_4, 200};
Motor_Struct right_top_motor = {&htim2, TIM_CHANNEL_2, 200};
Motor_Struct right_bottom_motor = {&htim1, TIM_CHANNEL_3, 200};

//LED结构体定义
LED_Struct left_top_led = {LED1_GPIO_Port,LED1_Pin};
LED_Struct left_bottom_led = {LED4_GPIO_Port,LED4_Pin};
LED_Struct right_top_led = {LED2_GPIO_Port,LED2_Pin};
LED_Struct right_bottom_led = {LED3_GPIO_Port,LED3_Pin};

//表示当前连接状态
Remote_State remote_state = REMOTE_DISCONNECTED;

//表示当前飞行状态
Flight_State flight_state = IDLE;

//接收到的遥控数据
Remote_Data remote_data = {0};

//电源管理任务
void power_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define POWER_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define POWER_TASK_PRIO 4
TaskHandle_t power_task_handler;
//任务周期
#define POWER_TASK_PERIOD 10000


//电机飞行控制任务
void flight_task( void * arg );
#define FLIGHT_TASK_STACK_SIZE 128
#define FLIGHT_TASK_PRIO 3
TaskHandle_t flight_task_handler;
#define FLIGHT_TASK_PERIOD 6


//LED灯控闪烁任务
void led_task( void * arg );
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIO 1
TaskHandle_t led_task_handler;
#define LED_TASK_PERIOD 100

//SI24R1通讯任务
void si24r1_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define SI24R1_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define SI24R1_TASK_PRIO 2
TaskHandle_t si24r1_task_handler;
#define SI24R1_TASK_PERIOD 6

/*
 *@启动FreeRTOS
 *
 */

void App_FreeRTOS_start(void) {

    //1. 创建电源管理任务
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIO, &power_task_handler);

    //2. 创建飞行管理任务
    xTaskCreate(flight_task, "flight_task", FLIGHT_TASK_STACK_SIZE, NULL,FLIGHT_TASK_PRIO, &flight_task_handler);

    //3. 创建LED任务
    xTaskCreate(led_task, "led_task", LED_TASK_STACK_SIZE, NULL,LED_TASK_PRIO,&led_task_handler);

    //4. 创建SI24R1通讯任务
    xTaskCreate(si24r1_task,"si24r1_task",SI24R1_TASK_STACK_SIZE,NULL,SI24R1_TASK_PRIO,&si24r1_task_handler);

    //启动调度器
    vTaskStartScheduler();
}

void power_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        //等待任务通知,直到 收到通知res=1 或 超时res=0
        uint32_t res = ulTaskNotifyTake(pdTRUE, POWER_TASK_PERIOD);
        if (res != 0) {
            Int_IP5305T_stop();
        }
        else {
            Int_IP5305T_start();
        }
    }
}

void flight_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    Int_MPU6050_Init();
    while (1) {
        //1. 获取三周角速度和加速度
        App_flight_get_euler_angle();

        vTaskDelayUntil(&xLastExecutionTime,FLIGHT_TASK_PERIOD);
    }
}

void led_task( void * arg ) {
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    uint8_t count = 0;
    while (1) {
        count++;
        //1. 判断当前连接状态
        if (remote_state == REMOTE_CONNECTED) {
            //点亮前两个灯
            Int_led_turn_on(&left_top_led);
            Int_led_turn_on(&right_top_led);
        }
        else if (remote_state == REMOTE_DISCONNECTED) {
            //关掉前两个灯
            Int_led_turn_off(&left_top_led);
            Int_led_turn_off(&right_top_led);
        }

        //2. 判断飞行状态
        if (flight_state == IDLE) {
            //500ms亮500ms灭
            if (count % 5 == 0) {
                Int_led_toggle(&left_bottom_led);
                Int_led_toggle(&right_bottom_led);
            }
        }
        else if (flight_state == NORMAL) {
            //200ms亮200ms灭
            if (count % 2 == 0) {
                Int_led_toggle(&left_bottom_led);
                Int_led_toggle(&right_bottom_led);
            }
        }
        else if (flight_state == FIX_HEIGHT) {
            Int_led_turn_on(&left_bottom_led);
            Int_led_turn_on(&right_bottom_led);
        }
        else if (flight_state == FAULT) {
            Int_led_turn_off(&left_bottom_led);
            Int_led_turn_off(&right_bottom_led);
        }

        if (count == 10) {
            count = 0;
        }

        vTaskDelayUntil(&xLastExecutionTime,LED_TASK_PERIOD);
    }
}


void si24r1_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        //1. 接收数据 返回0说明连接成功，有收到数据
        uint8_t res = App_Receive_Data();

        //2. 更新连接状态
        App_process_connect_state(res);

        //3. 处理关机命令
        if (remote_data.shutdown ==1) {
            //使用FreeRTOS直接任务通知,在电源任务中执行关机
            xTaskNotifyGive(power_task_handler);
        }

        //4. 处理飞行状态 flight_state
        App_process_flight_state();

        //6ms执行一次
        vTaskDelayUntil(&xLastExecutionTime, SI24R1_TASK_PERIOD);
    }
}