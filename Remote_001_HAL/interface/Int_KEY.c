#include "Int_KEY.h"

KEY_TYPE Int_Key_Get(void) {
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET) {
        //电弧抖动,和人手按下的抖动,需要消抖
        //读取按键值是在进入FreeRTOS系统后的任务,因此用FreeRTOS的延时函数
        vTaskDelay(5);
        //1. 二次判断防止电弧抖动
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET) {
            //2. 死循环等待按键抬起，防止人手按下时间过长
            while (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET) {
                //防止一直不抬手导致其他任务堵塞
                vTaskDelay(1);
            }
            //按键确实被按下,返回按键值
            return KEY_UP;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET) {
        vTaskDelay(5);
        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET) {
            while (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET) {
                vTaskDelay(1);
            }
            return KEY_DOWN;
        }

    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET) {
        vTaskDelay(5);
        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET) {
            while (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET) {
                vTaskDelay(1);
            }
            return KEY_LEFT;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET) {
        vTaskDelay(5);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET) {
            while (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET) {
                vTaskDelay(1);
            }
            return KEY_RIGHT;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET) {
        vTaskDelay(5);
        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET) {
            while (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET) {
                vTaskDelay(1);
            }
            return KEY_LEFT_X;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET) {
        TickType_t count_first = xTaskGetTickCount();
        vTaskDelay(5);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET) {
            while (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET) {
                vTaskDelay(1);
            }
            TickType_t count_second = xTaskGetTickCount();
            if (count_second - count_first > 1000) {
                //这里必须长按后松开才能读取到长按
                //如果改成长按一秒不松开也能读取，需要在应用层的按键任务和此处各加一个死循环判断按键是否松开
                return KEY_RIGHT_X_LONG;
            }
            else {
                return KEY_RIGHT_X;
            }
        }
    }

    //无事发生
    return KEY_NONE;
}