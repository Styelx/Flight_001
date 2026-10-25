#ifndef FLIGHT_001_HAL_COM_DEBUG_H
#define FLIGHT_001_HAL_COM_DEBUG_H

#include "usart.h"
#include "unistd.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
//宏定义方式打印日志，包括文件路径和行号
#define DEBUG 1

#ifdef DEBUG

#define FILE_NAME   (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define debug_printf(format, ...) printf("[%s:%d]:" format, FILE_NAME, __LINE__, ##__VA_ARGS__)

#else

#define debug_printf(format, ...)

#endif

#endif //FLIGHT_001_HAL_COM_DEBUG_H

