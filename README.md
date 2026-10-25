<p align="right">
  <b>中文</b> | <a href="./README_Eng.md">English</a> 
</p>
  <h1 align="center">STM32四轴无人机</h1>

<div align=center>
        <img src="https://img.shields.io/badge/version-0.1.1-blue">
        <img src=https://img.shields.io/github/stars/Styelx/Flight_001>
        <img src=https://img.shields.io/badge/OS-FreeRTOS-green>
</div>

<br>

## :ledger: Overview

尚硅谷四轴无人机项目：https://www.bilibili.com/video/BV1f8rbBSEq3/

开发环境使用STM32CubeMX+Clion, 需要有CMake环境配置经验, 同时根据实际对代码进行了部分优化. 

该项目仅为记录个人使用STM32CubeMX+Clion进行STM32开发的练习.

<br>

## :link:个人主页

<p align="center">
<a href="https://space.bilibili.com/335085137">bilibili主页 </a> |
<a href="https://github.com/Styelx">GitHub主页 </a> 
<br>

## :black_nib:使用Clion进行工程构建，烧录

建议在STM32Cube官网下载STM32CubeCLT，STM32CubeCLT包含一套完整的编译工具和ST-LINK驱动.

Clion在2025.3后的版本原生支持STM32CubeMX工程的创建，在CubeMX生成项目代码后，在指定文件路径下创建工程即可.

CMake配置保持默认或自行添加STM32CubeCLT中提供的工具链即可

最后在Clion设置->构建、执行、部署->调试器->调试服务器中添加ST-LINK.

**Ps:在调试器选项中取消勾选持久会话就可下载后自动复位**



可能出现问题：

1. 构建失败：高版本CubeMX不默认包含与编译器相关CMake，而是由`CMakePresets.json`提供，尝试在CMake配置选项中添加`--preset "Debug"`解决.
2. ST-LINK下载后提示`上传完成，但存在问题`：可能是ST-LINK驱动版本过低导致，更新驱动即可.

<br>

## :rocket: 增加的功能说明
### V0.0
- V0.0.1  添加原理图和PCB
- V0.0.2  移植FreeRTOS操作系统  添加README.md文件
- V0.0.3  添加电源管理系统
- V0.0.4  基础电机控制  LED灯状态指示
- V0.0.5  手柄按键和摇杆  SI24R1双端无线通讯
- V0.0.6  飞行状态机
- V0.0.7  MPU6050数据采集及处理
### V0.1
- V0.1.1  PID控制  电源bug修复  优化项目架构
<br>

## :bookmark_tabs:硬件功能

#### 1、电源管理芯片
使用IP5305T芯片，实现功能如下:

- 短按一次开机，1s内连续短按两次关机.

- 4个LED电量指示灯，1个灯代表25%电量.

- 管理锂电池充放电，使用USB烧录口即可给锂电池充电.

- 输出5V电压，使用AMS1117-3.3V芯片实现降压输出3.3V.
**PS:本机所采用的型号为IP5305T-8S芯片,与常规芯片不同的是自动关机时间更短，待机8s直接关机，因此需要调整电源管理任务时长(实测在5s-7s最好)**
<br>

#### 2、LED闪烁指示灯

- 前两个灯亮表示连接上遥控器

- 后两个灯亮表示定高功能

- 后两个灯灭表示无人机故障

- 后两个灯慢闪表示空闲状态

- 后两个灯快闪表示飞行状态
<br>

#### 3、按键和摇杆功能
- 左上按键关机，右上按键短按定高，长按校准摇杆
- 左摇杆上下为油门，默认最下面值为0，左右为偏航角，默认回中值为500
- 右摇杆上下为俯仰角，默认回中值为500，左右为翻滚角，默认回中值为500
<br>

## :file_folder:软件架构

<img src="images\软件架构.png" alt="软件架构" width="400" />
<br>

## :computer:软件部分设计细节

#### 1、FreeRTOS操作系统

四轴无人机上同时运行多个功能，必须有一个RTOS（实时操作系统）来处理各个任务的调度，通过移植FreeRTOS操作系统来解决不同任务的调度问题.

FreeRTOS直接在官网下载即可：https://wap.freertos.org/

本项目选择版本为202406.05，移植系统只需内核文件夹`FreeRTOS-Kernel`中的内容，将必需文件复制粘贴的项目路径即可，CMake的配置可以直接交给ai，只需要注意`FreeRTOSconfig.h`文件的位置

```cmake
#./Project/CMakeLists.txt
add_subdirectory(FreeRTOS)
target_link_libraries(${CMAKE_PROJECT_NAME}
        freertos_kernel
)
```
```cmake
#./Project/FreeRTOS/CMakeLists.txt
add_library(freertos_kernel STATIC
	...
)
add_subdirectory(include)
```
```cmake
#./Project/FreeRTOS/include/CMakeLists.txt
add_library(freertos_kernel_include INTERFACE)
target_include_directories(freertos_kernel_include
    INTERFACE
        .
        $<$<NOT:$<TARGET_EXISTS:freertos_config>>:${FREERTOS_CONFIG_FILE_DIRECTORY}>
)

target_link_libraries(freertos_kernel_include
    INTERFACE
        $<$<TARGET_EXISTS:freertos_config>:freertos_config>
)
```
<br>

#### 2、8520空心杯电机

使用TIM定时器输出PWM波形, 预分频系数4-1, 自动重装值1000-1, 默认比较值200, 默认占空比200/1000 = 20%, PWM频率为72MHz/4 = 18MHz, 周期1000/18MHz =55.56us.
<br>

#### 3、SI24R1 2.4G无线通讯

发送17字节数据包，前三字节为帧头校验位，设置为字符'C'、'J'、'X'，后四字节设置为一个字宽度的前13字节总和作为校验和，高位先行，综上数据包格式为'C'、'J'、'X'、...、sum[31:24]、sum[23:16]、sum[15:8]、sum[7:0].
<br>

#### 4、飞行状态机
<p float="left">
	<img src="images\飞行状态机.png" alt="飞行状态机" width="200" />
	<img src="images\油门状态机.png" alt="油门状态机" width="200" />
</p>
<br>

#### 5、MPU6050数据低通滤波
1. 一阶低通滤波

   角速度波动不大, 采用简单的一阶低通滤波即可
   
   ```c
   int16_t Common_Filter_LowPass(int16_t newValue, int16_t preFilteredValue){
       return ALPHA * newValue + (1 - ALPHA) * preFilteredValue;
   }
   ```
<p float="left">
	<img src="images\原始数据.png" alt="角速度原始数据" width="300" />
	===>
	<img src="images\一阶低通滤波.png" alt="一阶低通滤波" width="300" />
</p>
   

2. **滑动窗口中值滤波**
  窗口宽度K=5, 即在宽度为5的窗口里取中位数作为当前输出, 例如有输出[2,3,4,5,12,3,4], 对于输出12而言, 选用[2,3,4,5,12]的窗口,使中位数4代替12, 输出就变成[2,3,4,5,4,3,4]该方法可以有效滤去采样时的毛刺, 实测在MPU6050的数据采样中效果显著.

  ```c
  int16_t median_update(MedianFilter *mf, int16_t new_val) {
      mf->buf[mf->idx] = new_val;
      mf->idx = (mf->idx + 1) % MEDIAN_WIN;
      if (mf->count < MEDIAN_WIN) mf->count++;
      int16_t tmp[MEDIAN_WIN];
      for (int i = 0; i < mf->count; i++)
          tmp[i] = mf->buf[i];
      for (int i = 1; i < mf->count; i++) {
          int16_t key = tmp[i];
          int j = i - 1;
          while (j >= 0 && tmp[j] > key) {
              tmp[j + 1] = tmp[j];
              j--;
          }
          tmp[j + 1] = key;
      }
      return tmp[mf->count / 2];
  }
  ```

<p float="left">
	<img src="images\原始数据.png" alt="角速度原始数据" width="300" />
	===>
	<img src="images\中值滤波.png" alt="中值滤波" width="300" />
</p>

但存在几个问题：
- 要想得到好的滤波效果, 噪声不能太多，噪声强度要够大.
- 采样需要循环执行, 每次采样间隔时间过长, 一个任务周期6ms, 若设置100次采样则一次采样要0.6s,数据太过分散, 这对于高速运行的无人机显然是不可以接受的, 因此该种滤波方式不采用.

3. **卡尔曼滤波**

  加速度量程小, 波动较大, 需要采取更稳定的滤波算法.

  根据方差计算每次输出的加权系数, 将各个输出的权重加起来作为当前输出.

  ```c
  typedef struct {
      float LastP; // 上一时刻的状态方差（或协方差）
      float Now_P; // 当前时刻的状态方差（或协方差）
      float out;   // 滤波器的输出值，即估计的状态
      float Kg;    // 卡尔曼增益，用于调节预测值和测量值之间的权重
      float Q;     // 过程噪声的方差，反映系统模型的不确定性
      float R;     // 测量噪声的方差，反映测量过程的不确定性
  } KalmanFilter_Struct;
  KalmanFilter_Struct kfs[3] = {
      {0.02, 0, 0, 0, 0.001, 0.543},
      {0.02, 0, 0, 0, 0.001, 0.543},
      {0.02, 0, 0, 0, 0.001, 0.543}
  };
  double Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input) {
      kf->Now_P = kf->LastP + kf->Q;
      kf->Kg = kf->Now_P / (kf->Now_P + kf->R);
      kf->out = kf->out + kf->Kg * (input - kf->out);
      kf->LastP = (1 - kf->Kg) * kf->Now_P;
      return kf->out;
  }
  ```
<p float="left">
	<img src="images\加速度原始数据.png" alt="加速度原始数据" width="300" />
	===>
	<img src="images\卡尔曼滤波.png" alt="卡尔曼滤波" width="300" />
</p>
<br>

#### 6、姿态解算
1. **角速度积分(偏航角)**

$$
\theta = \int_a^b \omega(t)\,dt
$$

2. **加速度解算(俯仰角/横滚角)**

$$
\tan\theta = \frac{a_x}{a_z}\ or\ \frac{a_y}{a_z}
$$

3. **四元数姿态解算**


$$
p = a+bi+cj+dk
$$


$$
   v^*=pvp^{-1}
$$

   虽说理论精度高, 但感觉精度和稳定性还不如互补......

   而且四元数大量使用浮点型计算, FreeRTOS任务栈溢出的风险极高, 我在调试的时候必须把任务栈大小从128字拓展到256字才能正常运行, 因此我没有使用四元数姿态解算

   具体计算方法放在`common\com_imu.c`里了
   <br>

#### 7、串级PID控制
外环角度——>内环角速度

外环角度测量值由姿态解算得出, 内环角速度由MPU6050测量得出.

内环采用比例, 微分控制, 外环只取比例, 不使用积分项是因为电机转速有上限, 要是一阵风吹来, 可能需要的修正量远大于电机承载能力, 使得误差始终无法消除, 积分项就会不断累积.

具体参数见代码(PID参数随所处环境和无人机实际状况而定,代码中所使用参数仅为个人理解)

调参过程有以下几点注意事项:

1. 通过无线接收的处理后的摇杆数据范围(0,1000), 需要根据对应油门和欧拉角进行转换
2. 调参时先调内环角速度, 再调外环角度. 
3. 选择参数时要注意极性, 即PID系数的正负, 实际上在试飞的时候就能看出来参数的正负, 如果参数极性相反的话试飞结果会与预期结果正好相反
<br>