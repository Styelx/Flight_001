#ifndef REMOTE_001_HAL_APP_OLED_H
#define REMOTE_001_HAL_APP_OLED_H

#include "Inf_font.h"
#include "Inf_OLED.h"
#include "Int_SI24R1.h"
#include "App_process_data.h"

#define LINE1_BEGIN 28

#define LINE2_BEGIN 5

#define LINE3_BEGIN1 5
#define LINE3_BEGIN2 65

#define LINE4_BEGIN1 5
#define LINE4_BEGIN2 65

#define BAR1_BEGIN1 35
#define BAR1_BEGIN2 95

#define BAR2_BEGIN1 47
#define BAR2_BEGIN2 107


#define Y0 0
#define Y1 14
#define Y2 26
#define Y3 38



void App_oled_Init(void);

void App_oled_show(void);

#endif //REMOTE_001_HAL_APP_OLED_H
