<p align="right">
  <b>中文</b> | <a href="./README_Eng.md">English</a> 
</p>
  <h1 align="center">STM32四轴无人机</h1>

<div align=center>
        <img src="https://img.shields.io/badge/version-0.0.6-blue">
        <img src=https://img.shields.io/github/stars/Styelx/Flight_001>
        <img src=https://img.shields.io/badge/OS-FreeRTOS-green>
</div>

<br>

## :ledger: Overview

尚硅谷四轴无人机项目：https://www.bilibili.com/video/BV1f8rbBSEq3/

不同的是开发环境使用STM32CubeMX+Clion，默认编译工具为CMake，涉及部分CMake环境配置.

该项目仅用来记录个人使用STM32CubeMX+Clion进行STM32开发的练习.

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

#### V0.0.1
- 添加原理图和PCB
#### V0.0.2
- 移植FreeRTOS操作系统
- 添加README.md文件
#### V0.0.3
- 添加电源管理系统
#### V0.0.4
- 基础电机控制
- LED灯状态指示
#### V0.0.5
- 手柄按键和摇杆
- SI24R1双端无线通讯
#### V0.0.6
- 飞行状态机
<br>

## :bookmark_tabs:硬件功能

#### 1、电源管理芯片
使用IP5305T芯片，实现功能如下:

- 短按一次开机，1s内连续短按两次关机.

- 4个LED电量指示灯，1个灯代表25%电量.

- 管理锂电池充放电，使用USB烧录口即可给锂电池充电.

- 输出5V电压，使用AMS1117-3.3V芯片实现降压输出3.3V.
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

![软件架构](images\软件架构.png)
<br>

## :star2:实物图

占位符
<br>

## :computer:软件部分设计细节

#### 1、FreeRTOS操作系统

四轴无人机上同时运行多个功能，必须有一个RTOS（实时操作系统）来处理各个任务的调度，通过移植FreeRTOS操作系统来解决不同任务的调度问题.

FreeRTOS直接在官网下载即可：https://wap.freertos.org/

本项目选择版本为202406.05，移植系统只需内核文件夹`FreeRTOS-Kernel`中的内容，将必需文件复制粘贴的项目路径即可，CMake的配置则稍微麻烦，具体操作可以参照提供的examples，这里只给出示例代码

**Ps:也可以直接交给agent进行配置**

```cmake
#./Project/CMakeLists.txt
#添加FreeRTOS子目录
add_subdirectory(FreeRTOS)
#链接子目录下的freertos_kernel库，该库名称默认即为freertos_kernel
target_link_libraries(${CMAKE_PROJECT_NAME}
        freertos_kernel
)
```
```cmake
#./Project/FreeRTOS/CMakeLists.txt
#添加freertos_kernel静态库
add_library(freertos_kernel STATIC
	...
	#静态库包含.c文件
)
#添加FreeRTOS/include子目录
add_subdirectory(include)
```
```cmake
#./Project/FreeRTOS/include/CMakeLists.txt
#添加依赖库freertos_kernel_include
add_library(freertos_kernel_include INTERFACE)
#后面两句用来处理FreeRTOSConfig.h的配置，默认即可
target_include_directories(freertos_kernel_include
    INTERFACE
        .
        #如果不存在freertos_config，则该值为FreeRTOSConfig.h的文件路径
        $<$<NOT:$<TARGET_EXISTS:freertos_config>>:${FREERTOS_CONFIG_FILE_DIRECTORY}>
)

target_link_libraries(freertos_kernel_include
    INTERFACE
    	#如果存在freertos_config，则该值为freertos_config，将包含Config.h的库作为依赖
        $<$<TARGET_EXISTS:freertos_config>:freertos_config>
)
```
<br>

#### 2、8520空心杯电机

使用TIM定时器输出PWM波形, 预分频系数4-1, 自动重装值1000-1, 默认比较值200, 默认占空比200/1000 = 20%, PWM频率为72MHz/4 = 18MHz, 周期1000/18MHz =55.56us.
<br>

#### 3、SI24R1 2.4G无线通讯

发送17字节数据包，前三字节为帧头校验位，设置为字符'C'、'J'、'X'，后四字节设置为一个字宽度的前13字节总和作为校验和，高位先行，综上数据包格式为'C'、'J'、'X'、...、sum[31:24]、sum[23:16]、sum[15:8]、sum[7:0]
<br>

#### 4、飞行状态机
<div style="display:flex;gap:16px">
	<img src="images\飞行状态机.png" alt="飞行状态机" style="zoom:25%" />
	<img src="images\油门状态机.png" alt="油门状态机" style="zoom:25%" />
</div>
<br>