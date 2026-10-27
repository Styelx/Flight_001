#include "App_oled.h"

extern Remote_Data remote_data;

void App_oled_show_bar(uint8_t x, uint8_t y, uint8_t count) {
    if (count < 13) {
        OLED_Show_CH(x,y,12 + count,12,1);
    }
}

void App_oled_Init(void) {
    OLED_Init();
}

void App_oled_show(void) {

    //第一行 标题
    for (uint8_t i = 0; i < 6; i++) {
        OLED_Show_CH(LINE1_BEGIN + 12 * i, Y0,i,12,1);
    }

    //第二行 展示2.4g信道
    uint8_t buff[3] = {0};
    sprintf((char *)buff,"%03d",CHANNEL);
    OLED_ShowString(LINE2_BEGIN,Y1,buff,12,1);

    //第三行 展示遥控器数据
    uint8_t count = 0;
    OLED_ShowString(LINE3_BEGIN1,Y2,"THR:",12,1);
    //把100%进度条分成两个字符来表示
    //小于500时第二个字符留空,第一个字符填对应字库进度条
    //大于500时第一个字符涂满,第二个字符填对应字库进度条
    if (remote_data.thr > 500) {
        count = (remote_data.thr - 500) / 41;
        App_oled_show_bar(BAR1_BEGIN1,Y2,12);
        App_oled_show_bar(BAR2_BEGIN1,Y2,count);
    }
    else {
        count = remote_data.thr / 41;
        App_oled_show_bar(BAR1_BEGIN1,Y2,count);
        App_oled_show_bar(BAR2_BEGIN1,Y2,0);
    }

    OLED_ShowString(LINE3_BEGIN2,Y2,"ROL:",12,1);
    if (remote_data.roll > 500) {
        count = (remote_data.roll - 500) / 41;
        App_oled_show_bar(BAR1_BEGIN2,Y2,12);
        App_oled_show_bar(BAR2_BEGIN2,Y2,count);
    }
    else {
        count = remote_data.roll / 41;
        App_oled_show_bar(BAR1_BEGIN2,Y2,count);
        App_oled_show_bar(BAR2_BEGIN2,Y2,0);
    }

    OLED_ShowString(LINE4_BEGIN1,Y3,"YAW:",12,1);
    if (remote_data.yaw > 500) {
        count = (remote_data.yaw - 500) / 41;
        App_oled_show_bar(BAR1_BEGIN1,Y3,12);
        App_oled_show_bar(BAR2_BEGIN1,Y3,count);
    }
    else {
        count = remote_data.yaw / 41;
        App_oled_show_bar(BAR1_BEGIN1,Y3,count);
        App_oled_show_bar(BAR2_BEGIN1,Y3,0);
    }

    OLED_ShowString(LINE4_BEGIN2,Y3,"PIT:",12,1);
    if (remote_data.pitch > 500) {
        count = (remote_data.pitch - 500) / 41;
        App_oled_show_bar(BAR1_BEGIN2,Y3,12);
        App_oled_show_bar(BAR2_BEGIN2,Y3,count);
    }
    else {
        count = remote_data.pitch / 41;
        App_oled_show_bar(BAR1_BEGIN2,Y3,count);
        App_oled_show_bar(BAR2_BEGIN2,Y3,0);
    }
    //刷新屏幕
    OLED_Refresh_Gram();

}