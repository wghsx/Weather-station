#include "stm32f10x.h"                  // Device header
#include "stdio.h"
void MY1600_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽(TX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//(RX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//(RX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	USART_InitTypeDef USART5InitStructure;
	USART5InitStructure.USART_BaudRate = 9600;
	USART5InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件
	USART5InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全
	USART5InitStructure.USART_Parity = USART_Parity_No;
	USART5InitStructure.USART_StopBits = USART_StopBits_1;
	USART5InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART5,&USART5InitStructure);

	
	//使能USART
	USART_Cmd(UART5,ENABLE);
}

void UART5_SendChar(uint8_t data)
{
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE) == RESET);
	USART_SendData(UART5,data);
}

//发送指定长度数据
void UART5_Send_Data(u8* data,u8 len)
{
	while(len--)
	{
		UART5_SendChar(*data);
		data++;
	}
}
//语音播放函数
void Voice_Play(void)
{
	u8 buff[5] = {0x7E,0x03,0x11,0x12,0xEF};
	UART5_Send_Data(buff,5);
}
//语音停止
void Voice_Stop(void)
{
	u8 buff[5] = {0x7E,0x03,0x1E,0x1D,0xEF};
	UART5_Send_Data(buff,5);
}
//语音播放上一曲
void Voice_Back(void)
{
	u8 buff[5] = {0x7E,0x03,0x14,0x17,0xEF};
	UART5_Send_Data(buff,5);
}
//语音播放下一曲
void Voice_Next(void)
{
	u8 buff[5] = {0x7E,0x03,0x13,0x10,0xEF};
	UART5_Send_Data(buff,5);
}
//控制音量
void Voice_Sound(void)
{
	u8 buff[5] = {0x7E,0x04,0x31,0x00,0xEF};
	UART5_Send_Data(buff,5);
}
//播放指定文件夹指定曲目
void Voice_Num(u8 dir,u8 num)
{
	u8 check = 0;
	u8 buff[10] = {0x7E,0x05,0x42,dir,num,0,0xEF};
	check = 0x05^0x42^dir^num;
	buff[5] = check;
	UART5_Send_Data(buff,7);
}

