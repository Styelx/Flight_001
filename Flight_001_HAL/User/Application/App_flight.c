#include "App_flight.h"

#include "timers.h"

Gyro_Accel_Data gyro_accel_data = {0};
Euler_Data euler_data = {0};
Gyroscope_Data last_gyro_data = {0};
float euler_yaw_sum = 0.0f;

extern TaskHandle_t si24r1_task_handler;

extern Remote_Data remote_data;
extern Flight_State flight_state;
extern uint16_t flight_height;

//外环和内环均用pd 因为电机有速度上限 如果电机推力不足以抵消误差 误差会在积分项上一直累积 产生积分饱和
//先调节内环角速度  再调节外环角度

//偏航角 俯仰角 横滚角
//极性为负
Pid_Struct yaw_pid = {.kp = -3.0f,.ki = 0.0f,.kd = 0.0f, .last_error = 0.0f};
//极性为正
Pid_Struct pitch_pid = {.kp = 5.0f,.ki = 0.0f,.kd = 0.0f, .last_error = 0.0f};
//极性为正
Pid_Struct roll_pid = {.kp = 5.0f,.ki = 0.0f,.kd = 0.0f, .last_error = 0.0f};

//z轴角速度 y轴角速度 x轴角速度
//调参数时先将外环设置为0,现象往一个方向倾斜时能明显感觉到风扇有反方向的推力
//极性为负
Pid_Struct gyro_z_pid = {.kp = -5.0f,.ki = 0.0f,.kd = 0.0f, .last_error = 0.0f};
//极性为正
Pid_Struct gyro_y_pid = {.kp = 3.0f,.ki = 0.0f,.kd = 0.02f, .last_error = 0.0f};
//极性为负
Pid_Struct gyro_x_pid = {.kp = 3.0f,.ki = 0.0f,.kd = 0.02f, .last_error = 0.0f};


//定高PID
Pid_Struct fix_height_pid = {.kp = -0.6f,.ki = 0.0f,.kd = -0.02f, .last_error = 0.0f};


//电机结构体定义
Motor_Struct left_top_motor = {&htim3, TIM_CHANNEL_1, 0};
Motor_Struct left_bottom_motor = {&htim4, TIM_CHANNEL_4, 0};
Motor_Struct right_top_motor = {&htim2, TIM_CHANNEL_2, 0};
Motor_Struct right_bottom_motor = {&htim1, TIM_CHANNEL_3, 0};

/*****************************
 *@function:初始化MPU6050和电机
 *****************************/
void App_flight_Init(void) {
    //初始化MPU6050
    Int_MPU6050_Init();

    //启动电机
    Int_motor_start(&left_top_motor);
    Int_motor_start(&right_top_motor);
    Int_motor_start(&left_bottom_motor);
    Int_motor_start(&right_bottom_motor);

    //初始化激光测距仪
    Int_VL53L1X_Init();
}

/*****************************
 *@function:根据陀螺仪数据计算欧拉角
 *****************************/
void App_flight_gyro_accel_euler(void) {
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

    //3. 姿态解算, 通过加速度和角速度计算欧拉角
    //加速度解算 俯仰角向上为正 横滚角向右为负
    //范围 -180° —— +180°
    euler_data.pitch = atan2(gyro_accel_data.accel_data.accel_x, gyro_accel_data.accel_data.accel_z) * 180 / 3.14159;
    euler_data.roll = atan2(gyro_accel_data.accel_data.accel_y, gyro_accel_data.accel_data.accel_z) * 180 / 3.14159;

    //角速度积分 偏航角 启动后初始化完成时面朝的方向为0°,从上往下看逆时针为正
    //创建一个float变量保存累加和,若直接用(int16_t)yaw累加,强制类型转换时会截断,获取不到真实数据
    //此处如果一直往一个方向转会超过360°
    euler_yaw_sum += (gyro_accel_data.gyro_data.gyro_z * 2000 / 32769.0f) * 0.006;
    euler_data.yaw = euler_yaw_sum;

    //四元数 感觉精度还不如互补
    // Common_IMU_GetEulerAngle(&gyro_accel_data, &euler_data,0.006);

    //调试
    // debug_printf("%d, %d, %d\n",gyro_accel_data.gyro_data.gyro_x,gyro_accel_data.gyro_data.gyro_y,gyro_accel_data.gyro_data.gyro_z);
    // debug_printf("%d, %d, %d\n",gyro_accel_data.accel_data.accel_x,gyro_accel_data.accel_data.accel_y,gyro_accel_data.accel_data.accel_z);
    // debug_printf("%d, %d, %d\n", euler_data.pitch, euler_data.roll, euler_data.yaw);
}

void App_pid_process(void) {
    //俯仰角
    //1. 设置目标值和传感器测量值
    //平稳飞行等于0  遥控飞行等于遥控器的值
    //(0,1000)->(-500,500)->(10,-10)
    pitch_pid.desire =  0.0f - (remote_data.pitch - 500) / 50.0f;
    pitch_pid.measure = euler_data.pitch;
    gyro_y_pid.measure = gyro_accel_data.gyro_data.gyro_y * 2000.0f / 32768.0f;
    Com_PID_Calculate_Chain(&pitch_pid,&gyro_y_pid);

    //(0,1000)->(-500,500)->(10,-10)
    roll_pid.desire =  0.0f - (remote_data.roll - 500) / 50.0f;
    roll_pid.measure = euler_data.roll;
    gyro_x_pid.measure = gyro_accel_data.gyro_data.gyro_x * 2000.0f / 32768.0f;
    Com_PID_Calculate_Chain(&roll_pid,&gyro_x_pid);

    //(0,1000)->(-500,500)->(-10,10)
    yaw_pid.desire =  (remote_data.yaw - 500) / 50.0f;
    yaw_pid.measure = euler_data.yaw;
    gyro_z_pid.measure = gyro_accel_data.gyro_data.gyro_z * 2000.0f / 32768.0f;
    Com_PID_Calculate_Chain(&yaw_pid,&gyro_z_pid);

    // debug_printf("%d, %d, %d\n",(int16_t)pitch_pid.error,(int16_t)gyro_y_pid.error,(int16_t)gyro_y_pid.output);
}

/*****************************
 *@function:根据PID输出控制电机
 *****************************/
void App_flight_control_motor(void) {
    //1. 判断飞行状态
    switch (flight_state) {
        case IDLE:
            left_bottom_motor.speed = 0;
            left_top_motor.speed = 0;
            right_top_motor.speed = 0;
            right_bottom_motor.speed = 0;
            break;
        case NORMAL:
            left_top_motor.speed = remote_data.thr + gyro_y_pid.output - gyro_x_pid.output + gyro_z_pid.output;
            right_top_motor.speed = remote_data.thr + gyro_y_pid.output + gyro_x_pid.output - gyro_z_pid.output;
            left_bottom_motor.speed = remote_data.thr - gyro_y_pid.output - gyro_x_pid.output - gyro_z_pid.output;
            right_bottom_motor.speed = remote_data.thr - gyro_y_pid.output + gyro_x_pid.output + gyro_z_pid.output;
            break;
        case FIX_HEIGHT:
            left_top_motor.speed = remote_data.thr + gyro_y_pid.output - gyro_x_pid.output + gyro_z_pid.output + fix_height_pid.output;
            right_top_motor.speed = remote_data.thr + gyro_y_pid.output + gyro_x_pid.output - gyro_z_pid.output + fix_height_pid.output;
            left_bottom_motor.speed = remote_data.thr - gyro_y_pid.output - gyro_x_pid.output - gyro_z_pid.output + fix_height_pid.output;
            right_bottom_motor.speed = remote_data.thr - gyro_y_pid.output + gyro_x_pid.output + gyro_z_pid.output + fix_height_pid.output;
            break;
        case FAULT:
            //任务每6ms执行一次,每次执行任务降低2点速度
            left_bottom_motor.speed -= 2;
            left_top_motor.speed -= 2;
            right_top_motor.speed -= 2;
            right_bottom_motor.speed -= 2;
            if (left_bottom_motor.speed <= 0 && right_bottom_motor.speed <= 0
                && left_top_motor.speed <= 0 && right_bottom_motor.speed <= 0) {
                xTaskNotifyGive(si24r1_task_handler);
            }
            break;
    }
    //限制电机速度上限 防止过热
    left_top_motor.speed = Int_motor_limit_speed(left_top_motor.speed,0,600);
    right_top_motor.speed = Int_motor_limit_speed(right_top_motor.speed,0,600);
    left_bottom_motor.speed = Int_motor_limit_speed(left_bottom_motor.speed,0,600);
    right_bottom_motor.speed = Int_motor_limit_speed(right_bottom_motor.speed,0,600);

    //安全限制 油门小于50时强制归零
    if (remote_data.thr < 50) {
        left_top_motor.speed = 0;
        right_top_motor.speed = 0;
        left_bottom_motor.speed = 0;
        right_bottom_motor.speed = 0;
    }

    //将速度应用到电机
    Int_motor_set_speed(&left_top_motor);
    Int_motor_set_speed(&right_top_motor);
    Int_motor_set_speed(&right_bottom_motor);
    Int_motor_set_speed(&left_bottom_motor);

}

/*****************************
 *@function:定高功能PID
 *****************************/
void App_flight_fix_height(void) {
    fix_height_pid.desire =  flight_height;
    fix_height_pid.measure = Int_VL53L1X_GetDistance();
    Com_PID_Calculate(&fix_height_pid);
}