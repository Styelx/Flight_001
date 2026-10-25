#ifndef __INT_VL53L1X__
#define __INT_VL53L1X__

#include "vl53l1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"
#include "FreeRTOS.h"
#include "task.h"

void Int_VL53L1X_Init(void);

uint16_t Int_VL53L1X_GetDistance(void);

#endif // __INT_VL53L1X__
