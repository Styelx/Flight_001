#include "com_filter.h"

//卡尔曼滤波参数
//分别为x,y,z的初始参数
KalmanFilter_Struct kfs[3] = {
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543}
};

/*****************************
 *@function:中值滤波结构体初始化
 *****************************/
void median_init(MedianFilter *mf) {
    mf->idx = 0;
    mf->count = 0;
    for (int i = 0; i < MEDIAN_WIN; i++)
        mf->buf[i] = 0;
}

/*****************************
 *@function:中值滤波窗口插值
 *@return:滑动窗口中位数
 *****************************/
int16_t median_update(MedianFilter *mf, int16_t new_val) {
    // 1. 写入环形缓冲
    mf->buf[mf->idx] = new_val;
    mf->idx = (mf->idx + 1) % MEDIAN_WIN;
    if (mf->count < MEDIAN_WIN) mf->count++;

    // 2. 拷贝一份用于排序（不破坏原始环形缓冲）
    int16_t tmp[MEDIAN_WIN];
    for (int i = 0; i < mf->count; i++)
        tmp[i] = mf->buf[i];

    // 3. 插入排序
    for (int i = 1; i < mf->count; i++) {
        int16_t key = tmp[i];
        int j = i - 1;
        while (j >= 0 && tmp[j] > key) {
            tmp[j + 1] = tmp[j];
            j--;
        }
        tmp[j + 1] = key;
    }

    // 4. 取中位数
    return tmp[mf->count / 2];
}

/*****************************
* @function:一阶低通滤波
* @param:newValue 需要滤波的值 preFilteredValue 上一次滤波过的值
* @return:滤波结果
*****************************/
int16_t Common_Filter_LowPass(int16_t newValue, int16_t preFilteredValue) {
    return ALPHA * newValue + (1 - ALPHA) * preFilteredValue;
}

/*****************************
* @function:卡尔曼滤波
* @param:kf input
* @return:滤波结果
*****************************/
double Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input) {
    kf->Now_P = kf->LastP + kf->Q;
    kf->Kg = kf->Now_P / (kf->Now_P + kf->R);
    kf->out = kf->out + kf->Kg * (input - kf->out);
    kf->LastP = (1 - kf->Kg) * kf->Now_P;
    return kf->out;
}