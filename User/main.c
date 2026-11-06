#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "USART.h"
#include "led.h"
#include "stdio.h"
#include "Key.h"
#include "Exit.h"
#include "My1600.h"
#include "Voice.h"
#include "string.h"
#include "DHT11.h"
#include "Lcd.h"
#include "font.h"
#include "wifi.h"


u8 ui_flag = 0;
u8 water_flag=0;
//u8 my_string[] = "Hello, STM32!";
u8 flag;
int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	DHT11_Init();
	Exit_Init();
	Key_Init();
	MY1600_Init();
	Delay_ms(50);
	USART1_Init();
	LED_Init();
	Voice_Init();
	LCD_Init();
	WIFI_Init();
	printf("Init succeed...");
	//LCD_ShowString(20, 20, RED, WHITE,"Hello, STM32!" ,16);
	//LCD_ShowChar(10, 10, RED, WHITE, 'A', 32);
	LCD_ShowPicture(0,0,100,56,(u8*)gImage_cc);
	
	//SysTick_Init();
	SysTick_Config(7200);
	while(1){
		Voice_Recognition();
		if(ui_flag == 0)
		{
			ui_flag = 1;
			LCD_ShowString(100, 0, RED, WHITE, (u8*)"ÓïÒôÆøÏóÕ¾", 16);
			if(water_flag == 1)
				Check_Weather();
		}
		
	}
}

u16 time_cnt = 0;
void SysTick_Handler(void)
{
	if(SysTick->CTRL & (1<<16))
	{
		time_cnt++;
		if(time_cnt == 2000)
		{
			time_cnt = 0;
			u8 buff1[20] = {0};
			u8 buff2[20] = {0};
			DHT11_GetValue();
			sprintf((char*)buff1,"TEM:%.2f",te);
			sprintf((char*)buff2,"HUM:%.2f",hu);
			LCD_ShowString(0,200,RED,WHITE,buff1,16);
			LCD_ShowString(150,200,RED,WHITE,buff2,16);
		}
	}
}
