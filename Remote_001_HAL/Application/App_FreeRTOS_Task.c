#include "App_FreeRTOS_Task.h"

//STM32F103C8T6 SRAM 20k => FreeRTOS 17k

void task1( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define TASK1_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define TASK1_PRIO 2
TaskHandle_t Task1_Handler;

void task2( void * arg );
//任务所占空间128 * 4 = 512 Byte
#define TASK2_STACK_SIZE 128
//任务优先级,范围0-4，数值越大优先级越大
#define TASK2_PRIO 1
TaskHandle_t Task2_Handler;

/*
 *@启动FreeRTOS
 *
 */

void App_FreeRTOS_start(void) {
    //1. 创建任务
    xTaskCreate(task1, "task1", TASK1_STACK_SIZE, NULL, TASK1_PRIO, &Task1_Handler);

    xTaskCreate(task2, "task2", TASK2_STACK_SIZE, NULL, TASK2_PRIO, &Task1_Handler);

    //2. 启动调度器
    vTaskStartScheduler();
}

void task1( void * arg ) {
    while(1) {
        debug_printf("task1\r\n");
        vTaskDelay(1000);
    }
}

void task2( void * arg ) {
    while(1) {
        debug_printf("task2\r\n");
        vTaskDelay(1000);
    }
}
