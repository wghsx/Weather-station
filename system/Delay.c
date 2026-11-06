#include "stm32f10x.h"

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 

void SysTick_Init(void)
{
    // 配置 SysTick 计数器为每 1ms 触发一次中断
    // SystemCoreClock 是系统时钟频率，对于 STM32F103 通常是 72MHz
    // SysTick_Config 函数会自动设置 LOAD 寄存器并使能 SysTick 中断
    if (SysTick_Config(SystemCoreClock / 1000)) // SystemCoreClock / 1000 = 72MHz / 1000 = 72000
    {
        // SysTick 配置错误处理
        while (1);
    }
    // 启用 SysTick 中断（SysTick_Config 已经默认启用了）
    // SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}