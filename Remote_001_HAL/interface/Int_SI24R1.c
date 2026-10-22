#include "Int_SI24R1.h"

#include <sys/types.h>

//定义一个静态发送地址
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};

//SPI读写一个字节,返回值是读取的字节
static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t rx_data;
	HAL_SPI_TransmitReceive(&hspi1,&byte,&rx_data,1,1000);
	return rx_data;
}



/********************************************************
函数功能：写寄存器的值（单字节）
入口参数：reg：寄存器映射地址(SI24R1_WRITE_REG | reg)
		value：寄存器的值
返回 值：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	CS_LOW;
	status = SPI_RW(reg);				
	SPI_RW(value);
	CS_HIGH;
	return status;
}


/********************************************************
函数功能：写寄存器的值（多字节）
入口参数：reg：寄存器映射地址(SI24R1_WRITE_REG | reg)
		pBuf：写数据首地址
		size：写数据字节数
返回 值：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t size)
{
	uint8_t status,byte_ctr;

	CS_LOW;
	status = SPI_RW(reg);
	for(byte_ctr = 0; byte_ctr < size; byte_ctr++) {
		SPI_RW(*pBuf++);
	}
	CS_HIGH;

	return status;
}							  					   


/********************************************************
函数功能：读寄存器的值（单字节）
入口参数：reg：寄存器映射地址(SI24R1_READ_REG | reg)
返回 值：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Read_Reg(uint8_t reg)
{
 	uint8_t value;

	CS_LOW;
	SPI_RW(reg);			
	value = SPI_RW(0);
	CS_HIGH;

	return value;
}


/********************************************************
函数功能：读寄存器的值（多字节）
入口参数：reg：寄存器映射地址(SI24R1_READ_REG | reg)
		pBuf：接收缓冲区首地址
		bytes：读数据字节数
返回 值：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t size)
{
	uint8_t status,byte_ctr;

	CS_LOW;
	status = SPI_RW(reg);
	for(byte_ctr = 0;byte_ctr < size;byte_ctr++) {
		pBuf[byte_ctr] = SPI_RW(0);
	}
	CS_HIGH;

	return status;
}


/********************************************************
函数功能：SI24R1接收模式初始化
入口参数：无
返回 值：无
*********************************************************/
void Int_SI24R1_RX_Mode(void)
{
	CE_LOW;
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);		//接收设备接收通道0使用和发送设备相同的发送地址
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);									//使能接收通道0自动应答
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);								//使能接收通道0
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, CHANNEL);									//选择射频通道0x40
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);						//相同有效数据宽度
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);								//数据传输率2Mbps,发射功率7dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0f);									//CRC使能,16位CRC校验,上电,接收模式
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, 0xff);									//清除中断标志位
	CE_HIGH;
}						


/********************************************************
函数功能：SI24R1发送模式初始化
入口参数：无
返回 值：无
*********************************************************/
void Int_SI24R1_TX_Mode(void)
{
	CE_LOW;
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 接收通道0地址和发送地址相同

	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA, 0x01);       						// 接收通道0自动应答
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR, 0x01);   						// 使能接收通道0
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x0a);  						// 自动重发延时等待250us+86us,自动重发10次
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH, CHANNEL);         					// 选择射频通道40
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP, 0x0f);    						// 数据传输率2Mbps,发射功率7dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG, 0x0e);      						// CRC使能,16位CRC校验,上电
	CE_HIGH;
}


/********************************************************
函数功能：读取接收数据
入口参数：rxbuf：接收数据存放首地址
返回 值：0：接收到数据
		1：没接收到
*********************************************************/
uint8_t Int_SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	state = Int_SI24R1_Read_Reg(STATUS);  								//读取状态寄存器
	//原封不动写入,写1清除标志位
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG+STATUS,state);			//清除RX_DS

	if(state & RX_DR)													//接收到数据
	{
		Int_SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);				//读取数据
		Int_SI24R1_Write_Reg(FLUSH_RX,0xff);								//清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;															//没收到任何数据
}


/********************************************************
函数功能：发送一个数据包
入口参数：txbuf：发送数据包首地址
返回 值：0成功，1失败
*********************************************************/
uint8_t Int_SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
	CE_LOW;																	//CE拉低,使能SI24R1配置
	Int_SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);		//写数据到TX FIFO,32字节
 	CE_HIGH;																//CE置高，使能发送

	// while(IRQ == 1);														//等待发送完成
	state = Int_SI24R1_Read_Reg(STATUS);  									//读取状态寄存器的值
	while(((state & TX_DS) == 0) && ((state & MAX_RT) == 0)) {
		state = Int_SI24R1_Read_Reg(STATUS);
		//释放CPU,防止任务卡死
		vTaskDelay(1);
	}
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state); 			//清除TX_DS或MAX_RT中断标志
	if(state & MAX_RT)														//达到最大重发次数
	{
		Int_SI24R1_Write_Reg(FLUSH_TX,0xff);							//发送成功才会自动,此处失败手动清除TX FIFO寄存器
		return 1;
	}
	if(state & TX_DS)														//发送完成
	{
		return 0;
	}
	return 1;																//发送失败
}

uint8_t SI24R1_RX_BUFFER[TX_ADR_WIDTH] = {0};								//暂存数据

/********************************************************
函数功能：硬件接口层测试
入口参数：无
返回 值：0:初始化成功,1:初始化失败
*********************************************************/
u_int8_t Int_SI24R1_Check(void) {
	//测试SI24R1能否正常读写寄存器
	Int_SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR, SI24R1_RX_BUFFER, TX_ADR_WIDTH);	//需要先读取一次才能发送
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);			//写入发送地址
	Int_SI24R1_Read_Buf(SI24R1_READ_REG + TX_ADDR, SI24R1_RX_BUFFER, TX_ADR_WIDTH);	//读取统一寄存器数据
	for (uint8_t i = 0; i < TX_ADR_WIDTH; i++) {
		if (TX_ADDRESS[i] != SI24R1_RX_BUFFER[i]) {
			return 1;
		}
	}
	return 0;
}

/********************************************************
函数功能：硬件接口层初始化
入口参数：无
返回 值：无
*********************************************************/
void Int_SI24R1_Init(void) {
	//上电之后芯片需要启动时间
	HAL_Delay(200);

	//重复检测,直到返回值为0,启动成功
	while (Int_SI24R1_Check() == 1) {
		//每两次检测之间延时10ms,防止检测过快
		HAL_Delay(10);
	}

	//初始化后默认为接受状态,需要发送时再切换
	Int_SI24R1_RX_Mode();
	debug_printf("SI24R1 START SUCCESS!\r\n");
}

