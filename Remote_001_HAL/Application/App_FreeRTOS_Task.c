#include "App_FreeRTOS_Task.h"

//STM32F103C8T6 SRAM 20k => FreeRTOS 17k

//电源管理任务
void power_task( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define POWER_TASK_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define POWER_TASK_PRIO 4
TaskHandle_t power_task_handler;

/*
 *@启动FreeRTOS
 *
 */

void App_FreeRTOS_start(void) {
    //1. 创建任务
    xTaskCreate(power_task, "power_task", POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIO, &power_task_handler);
    //2. 启动调度器
    vTaskStartScheduler();
}

void power_task( void * arg ) {
    //获取基准时间
    TickType_t xLastExecutionTime = xTaskGetTickCount();
    while (1) {
        //每10s执行一次，启动电源防止自动关机
        vTaskDelayUntil(&xLastExecutionTime,10000);
        //先延时再启动，防止第一次启动时按键抖动导致关机
        //启动电源
        Int_IP5305T_start();
    }
}
