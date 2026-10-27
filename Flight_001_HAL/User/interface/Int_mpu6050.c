#include "Int_mpu6050.h"

int32_t gyro_x_offset = 0;
int32_t gyro_y_offset = 0;
int32_t gyro_z_offset = 0;

int32_t accel_x_offset = 0;
int32_t accel_y_offset = 0;
int32_t accel_z_offset = 0;

/****************************
 *@function:写入指定寄存器
 *
 *@param:reg 寄存器地址   data 写入值
 ****************************/
void Int_MPU6050_Write_Reg(uint8_t reg, uint8_t data) {
    HAL_I2C_Mem_Write(&hi2c1,MPU6050_ADDR_WRITE,reg,I2C_MEMADD_SIZE_8BIT,&data,1,1000);

}

/****************************
 *@function:读取指定寄存器
 *
 *@param:reg 寄存器地址   data 读缓冲区
 ****************************/
void Int_MPU6050_Read_Reg(uint8_t reg, uint8_t *data) {
    HAL_I2C_Mem_Read(&hi2c1,MPU6050_ADDR_READ, reg,I2C_MEMADD_SIZE_8BIT,data,1,1000);
}

/****************************
 *@function:零偏校准
 *
 *
 ****************************/
void Int_MPU6050_calculate_offset(void) {
    //1. 等待飞机停放平稳
    //前后两次加速度差值<=200mg 连续判断100次
    Accelerometer_Data data_last = {0};
    Accelerometer_Data data_current = {0};
    uint8_t count = 0;
    Int_MPU6050_Get_Accel(&data_last);
    while(count < 100) {
        Int_MPU6050_Get_Accel(&data_current);
        if (abs(data_current.accel_x - data_last.accel_x) <= 400
            && abs(abs(data_current.accel_y - data_last.accel_y) <= 400)
            && abs(data_current.accel_z - data_last.accel_z) <= 400) {
        count++;
        }
        else {
            count = 0;
        }
        data_last = data_current;
        HAL_Delay(6);
    }

    //2. 此时飞机已经平稳
    //连续采样100次算出偏移量
    Gyro_Accel_Data gyro_accel_data = {0};

    int32_t gyro_x_sum = 0;
    int32_t gyro_y_sum = 0;
    int32_t gyro_z_sum = 0;

    int32_t accel_x_sum = 0;
    int32_t accel_y_sum = 0;
    int32_t accel_z_sum = 0;

    for (uint8_t i = 0; i < 100; i++) {
        //此处每次获取值都减去偏移量,因此需要创建sum变量单独保存累加,否则会变成+offset后又-offse,相当于每次循环没累加上
        Int_MPU6050_Get_ALL(&gyro_accel_data);

        gyro_x_sum += (gyro_accel_data.gyro_data.gyro_x - 0);
        gyro_y_sum += (gyro_accel_data.gyro_data.gyro_y - 0);
        gyro_z_sum += (gyro_accel_data.gyro_data.gyro_z - 0);

        accel_x_sum += (gyro_accel_data.accel_data.accel_x - 0);
        accel_y_sum += (gyro_accel_data.accel_data.accel_y - 0);
        accel_z_sum += (gyro_accel_data.accel_data.accel_z - 16384);

        HAL_Delay(6);
    }
    gyro_x_offset = gyro_x_sum / 100;
    gyro_y_offset = gyro_y_sum / 100;
    gyro_z_offset = gyro_z_sum / 100;

    accel_x_offset = accel_x_sum / 100;
    accel_y_offset = accel_y_sum / 100;
    accel_z_offset = accel_z_sum / 100;
}


/****************************
 *@function:初始化MPU6050
 *
 *
 ****************************/
void Int_MPU6050_Init(void) {
    //1. 重启芯片 重置所有寄存器初始值 寄存器地址0x6B 最高位写1
    Int_MPU6050_Write_Reg(PWR_MGMT_1,0x80);
    uint8_t data_receive = 0;
    //2. 重置完成之后 PWR_MGMT_1寄存器值为0x40 表示低功耗状态 用来判断重置完成
    while (data_receive != 0x40) {
        Int_MPU6050_Read_Reg(PWR_MGMT_1,&data_receive);
    }
    //3. 唤醒MPU6050 SLEEP写0 进入正常工作状态
    Int_MPU6050_Write_Reg(PWR_MGMT_1,0x00);
    //4. 选择量程 角速度±2000°(000[11]000) 加速度±2g 高频滤波5Hz(000[00][001])
    Int_MPU6050_Write_Reg(GYRO_CONFIG,0x18);
    Int_MPU6050_Write_Reg(ACCEL_CONFIG,0x01);
    //5. 中断全部关闭
    Int_MPU6050_Write_Reg(INT_ENABLE,0x00);
    //6. 用户配置FIFO/SPI都用不到,写0x00
    Int_MPU6050_Write_Reg(USER_CTRL,0x00);
    //7. 设置采样频率 Fs > 2Fm  Fm = 1/(6ms)
    //角速度低通滤波184Hz,加速度低通滤波188Hz
    Int_MPU6050_Write_Reg(SMPRT_DIV,0x01);
    Int_MPU6050_Write_Reg(CONFIG_6050,0x01);
    //8. 设置X陀螺仪轴为参考时钟
    Int_MPU6050_Write_Reg(PWR_MGMT_1,0x01);

    //9. 设置三个方向的待机与否 用来调试
    // Int_MPU6050_Write_Reg(PWR_MGMT_2,0x00);

    //10.零偏校准
    Int_MPU6050_calculate_offset();

    debug_printf("MPU6050 START SUCCESS!\r\n");
}

/****************************
 *@function:读取角速度
 ****************************/
void Int_MPU6050_Get_Gyro(Gyroscope_Data *gyroscope_data) {
    uint8_t high = 0;
    uint8_t low = 0;
    //单位°/s
    Int_MPU6050_Read_Reg(GYRO_XOUT_H,&high);
    Int_MPU6050_Read_Reg(GYRO_XOUT_L,&low);
    //此处必须显式类型转换,否则 high << 8 | low 为uint16_t类型，会视为一个0-65565的数字参与计算
    gyroscope_data->gyro_x = (int16_t)(high << 8 | low) - gyro_x_offset;

    Int_MPU6050_Read_Reg(GYRO_YOUT_H,&high);
    Int_MPU6050_Read_Reg(GYRO_YOUT_L,&low);
    gyroscope_data->gyro_y = (int16_t)(high << 8 | low) - gyro_y_offset;

    Int_MPU6050_Read_Reg(GYRO_ZOUT_H,&high);
    Int_MPU6050_Read_Reg(GYRO_ZOUT_L,&low);
    gyroscope_data->gyro_z = (int16_t)(high << 8 | low) - gyro_z_offset;
}

/****************************
 *@function:读取角加速度
 ****************************/
void Int_MPU6050_Get_Accel(Accelerometer_Data *accelerometer_data) {
    uint8_t high = 0;
    uint8_t low = 0;
    Int_MPU6050_Read_Reg(ACCEL_XOUT_H,&high);
    Int_MPU6050_Read_Reg(ACCEL_XOUT_L,&low);
    //单位mg
    accelerometer_data->accel_x = (int16_t)(high << 8 | low) - accel_x_offset;

    Int_MPU6050_Read_Reg(ACCEL_YOUT_H,&high);
    Int_MPU6050_Read_Reg(ACCEL_YOUT_L,&low);
    accelerometer_data->accel_y = (int16_t)(high << 8 | low) - accel_y_offset;

    Int_MPU6050_Read_Reg(ACCEL_ZOUT_H,&high);
    Int_MPU6050_Read_Reg(ACCEL_ZOUT_L,&low);
    accelerometer_data->accel_z = (int16_t)(high << 8 | low) - accel_z_offset;
}

/****************************
 *@function:归并角速度和角加速度
 ****************************/
void Int_MPU6050_Get_ALL(Gyro_Accel_Data *gyro_accel_data) {
    Int_MPU6050_Get_Gyro(&(gyro_accel_data->gyro_data));
    Int_MPU6050_Get_Accel(&(gyro_accel_data->accel_data));
}