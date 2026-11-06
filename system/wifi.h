#ifndef _WIFI_H
#define _WIFI_H


#include "stm32f10x.h"


#include "stm32f10x.h"
#include "Delay.h"
#include "string.h"
#include "My1600.h"
#include "DHT11.h"
#include "Voice.h"
#include "Lcd.h"


typedef struct{

	u8 buff[512];     //用来保存USART3接收到的数据
	u16 len;          //用来记录接收数据大小
	u8 flag;          //记录数据是否接收完成

}U3;

void WIFI_Init(void);
void WIFI_TEST(void);
u8 WIFI_Connect(void);
u8 WIFI_GetWeather(void);
void Check_Weather(void);





#endif
