#ifndef FLIGHT_001_HAL_COM_FILTER_H
#define FLIGHT_001_HAL_COM_FILTER_H

#include <stdint.h>
#include "com_debug.h"

#define MEDIAN_WIN 5   // 必须是奇数

// 新值的系数  值越小  低通滤波的效果越强
// 一阶低通滤波 指数加权系数
#define ALPHA 0.15

//中值滤波结构体
typedef struct {
    int16_t buf[MEDIAN_WIN];
    int     idx;       // 当前写入位置
    int     count;     // 已填充数量（未满时用）
} MedianFilter;

// 卡尔曼滤波器结构体
typedef struct {
    float LastP; // 上一时刻的状态方差（或协方差）
    float Now_P; // 当前时刻的状态方差（或协方差）
    float out;   // 滤波器的输出值，即估计的状态
    float Kg;    // 卡尔曼增益，用于调节预测值和测量值之间的权重
    float Q;     // 过程噪声的方差，反映系统模型的不确定性
    float R;     // 测量噪声的方差，反映测量过程的不确定性
} KalmanFilter_Struct;

extern KalmanFilter_Struct kfs[3];

/*****************************
 *@function:中值滤波结构体初始化
 *****************************/
void median_init(MedianFilter *mf);

/*****************************
 *@function:中值滤波窗口插值
 *@return:滑动窗口中位数
 *****************************/
int16_t median_update(MedianFilter *mf, int16_t new_val);

/*****************************
* @function:一阶低通滤波
* @param:newValue 需要滤波的值 preFilteredValue 上一次滤波过的值
* @return:滤波结果
*****************************/
int16_t Common_Filter_LowPass(int16_t newValue, int16_t preFilteredValue);

/*****************************
* @function:卡尔曼滤波
* @param:kf input
* @return:滤波结果
*****************************/
double Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input);


#endif //FLIGHT_001_HAL_COM_FILTER_H
