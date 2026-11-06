#ifndef _LED_H
#define _LED_H



#include "stm32f10x.h"


	
#define LED2_OFF	GPIO_SetBits(GPIOC, GPIO_Pin_4);    //÷√1
#define LED1_OFF	GPIO_SetBits(GPIOC, GPIO_Pin_5);    //÷√1
#define LED2_ON	  GPIO_ResetBits(GPIOC, GPIO_Pin_4);  //«Â0
#define LED1_ON	  GPIO_ResetBits(GPIOC, GPIO_Pin_5);  //«Â0



void LED_Init(void);
void LED_Reverse(void);
#endif
