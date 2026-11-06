#include "led.h"
#include "Delay.h"

/*
	函数功能：LED初始化
	函数名称：LED_Init()
	函数参数：无
	函数返回值：无
	PC4 - LED2  
	PC5 - LED1  
*/
void LED_Init(void)
{
	//1.开时钟--GPIOC组时钟开启
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;  //PC4/PC5
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;         //普通输出推挽模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //快速
	//2.引脚配置
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//3.输出数据--输出数据寄存器输出
	LED2_OFF;
	LED1_OFF;
}

void LED_Reverse(void)
{
	if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_4) == 0){
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
	}
	else{
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);
	}
}
