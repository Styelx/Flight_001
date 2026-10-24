#include "App_flight.h"

Gyro_Accel_Data gyro_accel_data = {0};
Euler_Data euler_data = {0};
Gyroscope_Data last_gyro_data = {0};

/*****************************
 *@function:根据陀螺仪数据计算欧拉角
 *
 *
 *****************************/
void App_flight_get_euler_angle(void) {
    //1. 得到三轴角速度和加速度
    Int_MPU6050_Get_ALL(&gyro_accel_data);

    //2. 低通滤波

    /******************************
     *一阶低通滤波
     *角速度变化不大, 可以用这种方法滤波
     ******************************/
    gyro_accel_data.gyro_data.gyro_x = Common_Filter_LowPass(gyro_accel_data.gyro_data.gyro_x,last_gyro_data.gyro_x);
    gyro_accel_data.gyro_data.gyro_y = Common_Filter_LowPass(gyro_accel_data.gyro_data.gyro_y,last_gyro_data.gyro_y);
    gyro_accel_data.gyro_data.gyro_z = Common_Filter_LowPass(gyro_accel_data.gyro_data.gyro_z,last_gyro_data.gyro_z);

    last_gyro_data.gyro_x = gyro_accel_data.gyro_data.gyro_x;
    last_gyro_data.gyro_y = gyro_accel_data.gyro_data.gyro_y;
    last_gyro_data.gyro_z = gyro_accel_data.gyro_data.gyro_z;

    /******************************
     *滑动窗口中值滤波,效果不错,但采样速度有点慢,一次采样6*100ms
     ******************************/
    // MedianFilter mf_x;
    // median_init(&mf_x);
    //
    // MedianFilter mf_y;
    // median_init(&mf_y);
    //
    // MedianFilter mf_z;
    // median_init(&mf_z);
    //
    // int16_t filtered_x = 0;
    // int16_t filtered_y = 0;
    // int16_t filtered_z = 0;
    //
    // int16_t raw = 0;
    // Gyroscope_Data gyro_data = {0};
    // for (int i = 0; i < 50; i++) {
    //     Int_MPU6050_Get_Gyro(&gyro_data);
    //
    //     raw = gyro_data.gyro_x;          // 读原始数据
    //     filtered_x = median_update(&mf_x, raw);
    //
    //     raw = gyro_data.gyro_y;
    //     filtered_y = median_update(&mf_y, raw);
    //
    //     raw = gyro_data.gyro_z;
    //     filtered_z = median_update(&mf_z, raw);
    //     vTaskDelay(6);
    //     // 使用 filtered
    // }
    // gyro_accel_data.gyro_data.gyro_x = filtered_x;
    // gyro_accel_data.gyro_data.gyro_y = filtered_y;
    // gyro_accel_data.gyro_data.gyro_z = filtered_z;

    /******************************
     *卡尔曼滤波
     *加速度变化大, 可以用这种方法滤波
     ******************************/
    gyro_accel_data.accel_data.accel_x = Common_Filter_KalmanFilter(&kfs[0],gyro_accel_data.accel_data.accel_x);
    gyro_accel_data.accel_data.accel_y = Common_Filter_KalmanFilter(&kfs[1],gyro_accel_data.accel_data.accel_y);
    gyro_accel_data.accel_data.accel_z = Common_Filter_KalmanFilter(&kfs[2],gyro_accel_data.accel_data.accel_z);

    //调试
    // debug_printf("%d, %d, %d\n",gyro_accel_data.gyro_data.gyro_x,gyro_accel_data.gyro_data.gyro_y,gyro_accel_data.gyro_data.gyro_z);
    debug_printf("%d, %d, %d\n",gyro_accel_data.accel_data.accel_x,gyro_accel_data.accel_data.accel_y,gyro_accel_data.accel_data.accel_z);
}