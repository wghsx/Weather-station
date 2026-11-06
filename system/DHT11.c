#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "stdio.h"
#include "DHT11.h"

void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	Delay_ms(1000);//提供延时越过不稳定时间
	
	DHT11_DATA_H; //拉高DATA，高电平由上拉电阻提供
}

void DHT11_Start(void)
{
	DHT11_DATA_L;
	Delay_ms(20);
	DHT11_DATA_H;	
}

u8 DHT11_Ask(void)
{
	u8 cnt = 0;
	while(DHT11_DATA_IN)
	{
		cnt++;
		Delay_us(20);
		if(cnt == 5)
		{
			return 1;
		}
	}
	while(!DHT11_DATA_IN);
	while(DHT11_DATA_IN);
	
	return 0; //成功
}
//获取40bit数据
u8 DHT11_RecData(void)
{
	u8 data = 0;
	for(u8 i = 0;i < 8;i++){
		while(!DHT11_DATA_IN);
		Delay_us(40);
		data <<= 1;
		if(DHT11_DATA_IN){
			data |= 1;
		}
		while(DHT11_DATA_IN);
	}
	return data;	
}


float te;
float hu;
//获取湿温度数据一一校验
u8 DHT11_GetValue(void)
{
	u8 buff[5] = {0};  //存储数据
	DHT11_Start();  //发送起始信号
	
	if(DHT11_Ask() == 1) //判断DHT11是否响应
	{
		return 1;   //响应失败
	}
	buff[0] = DHT11_RecData();
	buff[1] = DHT11_RecData();
	buff[2] = DHT11_RecData();
	buff[3] = DHT11_RecData();
	buff[4] = DHT11_RecData();
	//校验数据
	if(buff[4] == ((buff[0] + buff[1] + buff[2] + buff[3]) & 0xff))
	{
		te = (float)buff[2] + (float)buff[3] /10;
		hu = (float)buff[0] + (float)buff[1] /10;
		return 0;  //接收湿温度数据成功
	}
	return 2; //校验失败
}

