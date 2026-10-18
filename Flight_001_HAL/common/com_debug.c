#include "com_debug.h"

int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 1000);
    return len;
}