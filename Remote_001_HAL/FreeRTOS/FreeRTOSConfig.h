/*
 * FreeRTOS configuration for STM32F103xB (Cortex-M3, 72 MHz, 20 KB SRAM).
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stddef.h>
#include <stdint.h>

/* Scheduler ----------------------------------------------------------------*/
#define configUSE_PREEMPTION                       1
#define configUSE_TIME_SLICING                     1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#define configUSE_TICKLESS_IDLE                    0

#define configCPU_CLOCK_HZ                         ( 72000000UL )
#define configTICK_RATE_HZ                         ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                       7
#define configMINIMAL_STACK_SIZE                   ( ( uint16_t ) 128 )
#define configMAX_TASK_NAME_LEN                    16
#define configTICK_TYPE_WIDTH_IN_BITS              TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                    1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES      1
#define configQUEUE_REGISTRY_SIZE                  8
#define configENABLE_BACKWARD_COMPATIBILITY        1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    0
#define configSTACK_DEPTH_TYPE                     size_t
#define configMESSAGE_BUFFER_LENGTH_TYPE           size_t
#define configHEAP_CLEAR_MEMORY_ON_FREE            1

/* Software timers ----------------------------------------------------------*/
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                   10
#define configTIMER_TASK_STACK_DEPTH               ( configMINIMAL_STACK_SIZE * 2 )

/* Synchronisation primitives -----------------------------------------------*/
#define configUSE_EVENT_GROUPS                     1
#define configUSE_STREAM_BUFFERS                   1
#define configUSE_TASK_NOTIFICATIONS               1
#define configUSE_MUTEXES                          1
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_COUNTING_SEMAPHORES              1
#define configUSE_QUEUE_SETS                       0

/* Memory allocation --------------------------------------------------------*/
#define configSUPPORT_STATIC_ALLOCATION            0
#define configSUPPORT_DYNAMIC_ALLOCATION           1
#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 8 * 1024 ) )
#define configAPPLICATION_ALLOCATED_HEAP           0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP  0
#define configENABLE_HEAP_PROTECTOR                0

/* Cortex-M interrupt priorities --------------------------------------------*/
#define configPRIO_BITS                            4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY    15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY \
    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_API_CALL_INTERRUPT_PRIORITY \
    configMAX_SYSCALL_INTERRUPT_PRIORITY

/* Hooks and callbacks ------------------------------------------------------*/
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configUSE_MALLOC_FAILED_HOOK               0
#define configUSE_DAEMON_TASK_STARTUP_HOOK         0
#define configCHECK_FOR_STACK_OVERFLOW             0

/* Statistics ---------------------------------------------------------------*/
#define configGENERATE_RUN_TIME_STATS              0
#define configUSE_TRACE_FACILITY                   0
#define configUSE_STATS_FORMATTING_FUNCTIONS       0

/* Co-routines --------------------------------------------------------------*/
#define configUSE_CO_ROUTINES                      0

/* Optional API functions ---------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet                   1
#define INCLUDE_uxTaskPriorityGet                  1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskSuspend                       1
#define INCLUDE_vTaskDelayUntil                    1
#define INCLUDE_vTaskDelay                         1
#define INCLUDE_xTaskGetSchedulerState             1
#define INCLUDE_xTaskGetCurrentTaskHandle          1
#define INCLUDE_uxTaskGetStackHighWaterMark        1
#define INCLUDE_xTaskGetIdleTaskHandle             1
#define INCLUDE_eTaskGetState                      1
#define INCLUDE_xTaskGetHandle                     1
#define INCLUDE_xResumeFromISR                     1
#define INCLUDE_xTaskResumeFromISR                 1
#define INCLUDE_xEventGroupSetBitFromISR           1
#define INCLUDE_xTimerPendFunctionCall             1

/* Debugging ----------------------------------------------------------------*/
#define configASSERT( x )                          \
    do                                             \
    {                                              \
        if( ( x ) == 0 )                           \
        {                                          \
            taskDISABLE_INTERRUPTS();              \
            for( ; ; )                             \
            {                                      \
            }                                      \
        }                                          \
    } while( 0 )

/* Set to 1 after SVC_Handler/PendSV_Handler are routed to the FreeRTOS port
 * handlers in stm32f1xx_it.c. */
#define configCHECK_HANDLER_INSTALLATION           0

#define configUSE_NEWLIB_REENTRANT                 0

#define xPortPendSVHandler PendSV_Handler
#define vPortSVCHandler SVC_Handler

#define INCLUDE_xTaskGetSchedulerState 1

#endif /* FREERTOS_CONFIG_H */
