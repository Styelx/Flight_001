#ifndef REMOTE_001_HAL_INT_KEY_H
#define REMOTE_001_HAL_INT_KEY_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_RIGHT_X_LONG
}KEY_TYPE;

KEY_TYPE Int_Key_Get(void);         //获取当前按键值

#endif //REMOTE_001_HAL_INT_KEY_H
