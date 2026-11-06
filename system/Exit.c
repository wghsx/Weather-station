#include "stm32f10x.h"                  // Device header

extern u8 flag;
void Exit_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//配置AFIO
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	EXTI_InitTypeDef EXTI_InitStructure;//初始化配置EXIT
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	//配置NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel= EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
}


void EXTI0_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line0) ==  SET)//先检查一下是不是PA0端口中断
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
		{
			flag = 1;
			EXTI_ClearITPendingBit(EXTI_Line0);
		}
	}
}
