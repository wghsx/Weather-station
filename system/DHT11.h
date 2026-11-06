#ifndef _DHT11_H
#define _DHT11_H

#define DHT11_DATA_H (GPIO_SetBits(GPIOC,GPIO_Pin_1))
#define DHT11_DATA_L (GPIO_ResetBits(GPIOC,GPIO_Pin_1))

#define DHT11_DATA_IN (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1))

void DHT11_Init(void);
void DHT11_Start(void);
u8 DHT11_Ask(void);
u8 DHT11_RecData(void);
u8 DHT11_GetValue(void);

extern float te;
extern float hu;
#endif
