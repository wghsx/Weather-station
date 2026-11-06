#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "USART.h"

void USART1_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽(TX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//(RX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	USART_InitTypeDef USART1InitStructure;
	USART1InitStructure.USART_BaudRate = 9600;
	USART1InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件
	USART1InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全
	USART1InitStructure.USART_Parity = USART_Parity_No;
	USART1InitStructure.USART_StopBits = USART_StopBits_1;
	USART1InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART1InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVICInitStructure;
	NVICInitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVICInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVICInitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVICInitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVICInitStructure);
	//使能接收非空中断
	//USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//使能空闲中断
	
	//使能USART
	USART_Cmd(USART1,ENABLE);
}

void USART1_SendChar(uint8_t data)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	USART_SendData(USART1,data);
}

uint8_t USART1_ReceiveChar(void)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(USART1);
}
 
void USART1_SendString(char* str)
{
	while(*str)
	{
		USART1_SendChar(*str++);
	}
}

//printf重定向
int fputc(int c,FILE* stream)
{
	USART1_SendChar(c);  
	return c;
}

REC_BUF rec_buf;

void USART1_IRQHandler(void)
{
	//接收中断发生
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)  //RXNE收到数据，产生中断
	{
		//1.清除中断标记位
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		rec_buf.rec_str[rec_buf.len] = USART_ReceiveData(USART1);   //"asdf"
		rec_buf.len++;
		
	}
	//空闲中断发生
	if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET)  //IDLE检测到总线空闲   			 
	{
		//1.清除中断标记位
		USART1->SR;
		USART1->DR; 
		rec_buf.rec_str[rec_buf.len] = '\0';   //数据接收完成，数组成员等于结束符
		rec_buf.len = 0;											 //数据长度清零
		rec_buf.rec_value = 1;									 //标记数据接收完成
		printf("Receive:%s\r\n",rec_buf.rec_str);
	}
}



