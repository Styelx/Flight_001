#ifndef FLIGHT_001_HAL_COM_PID_H
#define FLIGHT_001_HAL_COM_PID_H

#define DELTA_T 0.006

typedef struct {
    float kp;           //比例
    float ki;           //积分
    float kd;           //微分
    float error;        //当前误差
    float last_error;   //上次误差
    float desire;       //目标值
    float measure;      //测量值
    float integral;     //积分累积
    float output;       //输出
}Pid_Struct;

//单次PID
void Com_PID_Calculate(Pid_Struct *pid);

//串级PID
void Com_PID_Calculate_Chain(Pid_Struct *pid1, Pid_Struct *pid2);

#endif //FLIGHT_001_HAL_COM_PID_H
