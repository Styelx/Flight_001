#include "App_FreeRTOS_Task.h"

#include "Int_SI24R1.h"

//STM32F103C8T6 SRAM 20k => FreeRTOS 17k

//电源管理任务
void power_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define POWER_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define POWER_TASK_PRIO 4
TaskHandle_t power_task_handler;
#define POWER_TASK_PERIOD 10000

//SI24R1通讯任务
void si24r1_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define SI24R1_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define SI24R1_TASK_PRIO 3
TaskHandle_t si24r1_task_handler;
#define SI24R1_TASK_PERIOD 6

//按键任务
void key_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define KEY_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define KEY_TASK_PRIO 2
TaskHandle_t key_task_handler;
#define KEY_TASK_PERIOD 20

//摇杆任务
void joystick_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define JOYSTICK_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define JOYSTICK_TASK_PRIO 2
TaskHandle_t joystick_task_handler;
#define JOYSTICK_TASK_PERIOD 20

/*
 *@启动FreeRTOS
 *
 */

void App_FreeRTOS_start(void) {

    //1. 创建电源管理任务
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIO, &power_task_handler);

    //2. 创建SI24R1通讯任务
    xTaskCreate(si24r1_task,"si24r1_task",SI24R1_TASK_STACK_SIZE,NULL,SI24R1_TASK_PRIO,&si24r1_task_handler);

    //3. 创建按键任务
    xTaskCreate(key_task, "key_task", KEY_TASK_STACK_SIZE, NULL,KEY_TASK_PRIO,&key_task_handler);

    //4. 创建摇杆任务
    xTaskCreate(joystick_task,"joystick_task",JOYSTICK_TASK_STACK_SIZE,NULL,JOYSTICK_TASK_PRIO,&joystick_task_handler);

    //启动调度器
    vTaskStartScheduler();
}

void power_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        //每10s执行一次，启动电源防止自动关机
        vTaskDelayUntil(&xLastExecutionTime, POWER_TASK_PERIOD);
        //先延时再启动，防止第一次启动时按键抖动导致关机
        //启动电源
        Int_IP5305T_start();
    }
}

uint8_t si24r1_buff[TX_PLOAD_WIDTH] = {0};

void si24r1_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        //将处理打包好的摇杆按键数据发送出去
        App_transmit_data();

        //6ms执行一次
        vTaskDelayUntil(&xLastExecutionTime, SI24R1_TASK_PERIOD);
    }
}

void key_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        App_process_key_data();
        vTaskDelayUntil(&xLastExecutionTime, KEY_TASK_PERIOD);
    }
}

void joystick_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    Int_joystick_init();
    while (1) {
        App_process_joystick_data();
        vTaskDelayUntil(&xLastExecutionTime, JOYSTICK_TASK_PERIOD);
    }
}