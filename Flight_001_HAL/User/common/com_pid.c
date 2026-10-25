#include "com_pid.h"

//单次PID
void Com_PID_Calculate(Pid_Struct *pid) {
    //1. 计算误差
    pid->error = pid->measure - pid->desire;

    //2. 计算积分误差
    pid->integral += (pid->error + pid->last_error) * DELTA_T / 2;

    //3. 计算微分误差
    //把除以DELTA_T放到最后计算,先乘后除减小误差
    float diff = pid->error - pid->last_error;

    //4. 计算输出
    pid->output = pid->kp * pid->error + pid->ki * pid->integral + pid->kd * diff / DELTA_T;

    //5. 更新误差
    pid->last_error = pid->error;
}

//串级PID
void Com_PID_Calculate_Chain(Pid_Struct *outer_pid, Pid_Struct *inner_pid) {
    //1. 先算外环
    Com_PID_Calculate(outer_pid);
    //2. 外环输出作为内环的目标值
    inner_pid->desire = outer_pid->output;
    Com_PID_Calculate(inner_pid);
}