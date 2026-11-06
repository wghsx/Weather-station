#include "stm32f10x.h"                  // Device header
#include "Voice.h"
#include "stdio.h"
#include "My1600.h"
#include "DHT11.h"
#include "led.h" // 假设你的LED控制函数在led.h中声明

// Global U4 struct instance
U4 u4; // u4已经在Voice.h中声明为extern，这里也需要声明extern

extern u8 water_flag;   // 定义在 main.c，用于通知主循环发起天气查询

void Voice_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽(TX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_Initstructure);

	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//(RX)
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC,&GPIO_Initstructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART4,&USART_InitStructure);

	NVIC_InitTypeDef NVICInitStructure;
	NVICInitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVICInitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVICInitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVICInitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVICInitStructure);


	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//使能接收中断
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//使能空闲中断

	//使能USART
	USART_Cmd(UART4,ENABLE);

	// Initialize U4 struct
	u4.len = 0;
	u4.v_flag = 0;
}

//语音识别执行
void Voice_Recognition(void)
{
	//判断数据接受完成
	if(u4.v_flag == 1){ //
		u4.v_flag = 0; //重置标签

        // For debugging: Print the received data
        printf("Received Voice Data (len %d): ", u4.len);
        for(u16 i = 0; i < u4.len; i++){
            printf("%02X ", u4.buff[i]);
        }
        printf("\r\n");

        // 假设语音模块的返回格式为 AA BB XX CC，总长度为 4 字节
        // 需要检查接收到的数据长度是否至少为 4 字节，并且起始和结束字节是否正确
        if (u4.len >= 4 && u4.buff[0] == 0xAA && u4.buff[1] == 0xBB && u4.buff[3] == 0xCC) {
            // 根据图片中的命令ID来判断
            switch(u4.buff[2]){ // u4.buff[2]是命令ID (XX)
                case 0x01: // 唤醒词 "wakeup uni"
                    printf("Voice Command: Wakeup Uni\r\n");
                    Voice_Num(02, 002); 
                    break;
                case 0x02: //"Who" (你是谁)
                    printf("Voice Command: Who\r\n");
                    Voice_Num(02, 006); 
                    break;
                case 0x03: // "tem" (室内温度是多少)
                    printf("Voice Command: Temperature Query\r\n");
					// 【修改部分】
                    // 1. 先播报“当前室内温度是”
                    Voice_Num(00, 014); // 假设 02 文件夹下的 003 语音文件是“当前室内温度是”
                    // 2. 将温度值拆分成数字，并依次播报
                    // 假设 te 是浮点数，比如 25.6
                    int temp_integer = (int)te; // 整数部分，例如 25
                    int temp_decimal = (int)((te - temp_integer) * 10); // 小数部分，例如 6 (如果需要播报小数)
                    // 播报整数部分的十位和个位
                    if (temp_integer >= 10) { // 播报十位
                        Voice_Num(00, temp_integer / 10); // 假设 0 文件夹下是数字语音文件
                    }
                    Voice_Num(00, temp_integer % 10); // 播报个位
                    // 播报“点”和小数部分 (如果需要播报小数)
                    if (temp_decimal > 0) {
                        Voice_Num(00, 020); // 假设 0 文件夹下的 10 是“点”的语音文件
                        Voice_Num(00, temp_decimal); // 播报小数位
                    }
                    Voice_Num(00, 013); // 假设 02 文件夹下的 004 语音文件是“摄氏度”
                    break;
                case 0x04: // "hu" (室内湿度是多少)
                    printf("Voice Command: Humidity/Temperature Query\r\n");
                    Voice_Num(00, 015);
                    break;
                case 0x05: // "singasong" (播放音乐/放歌)
                    printf("Voice Command: Play Music\r\n");
					Voice_Num(02, 007);
					Voice_Num(01, 004); // 调用播放音乐函数
                    break;
                case 0x06: // "stopsing" (停止播放/闭嘴)
                    printf("Voice Command: Stop Music\r\n");
					Voice_Num(02, 007);
                    Voice_Stop(); // 调用停止播放函数
                    break;
                case 0x07: // "openled" (开灯)
                    printf("Voice Command: Open LED\r\n");
                    // 假设你的LED控制函数是 LED_ON()
                    LED1_ON;
					LED2_ON;		// 请根据你的led.c中的实际函数名进行调用
                    break;
                case 0x08: // "colseled" (关灯)
                    printf("Voice Command: Close LED\r\n");
                    // 假设你的LED控制函数是 LED_OFF()
                    LED1_OFF;
					LED2_OFF	// 请根据你的led.c中的实际函数名进行调用
                    break;
                case 0x09: // "curtq" (今天天气怎么样)
                    printf("Voice Command: Weather Query \r\n");
					Voice_Num(03, 000);
                    water_flag = 1;   // 置位后由主循环调用 Check_Weather
                    break;
                default:
                    printf("Unknown Voice Command ID: 0x%02X\r\n", u4.buff[2]);
                    break;
            }
        } else {
            printf("Invalid Voice Data Format or Length. Length: %d, Data: %02X %02X %02X %02X...\r\n", u4.len, u4.buff[0], u4.buff[1], u4.buff[2], u4.buff[3]);
        }
        u4.len = 0; // Reset length after processing
	}
}

void UART4_IRQHandler(void)
{
	//接收中断发生
	if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET)  //RXNE收到数据，产生中断
	{
		//1.清除中断标记位
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
		// Store received data
		if(u4.len < sizeof(u4.buff)){ // Prevent buffer overflow
			u4.buff[u4.len++] = USART_ReceiveData(UART4);
		}
	}
	//空闲中断发生
	if(USART_GetITStatus(UART4, USART_IT_IDLE) == SET)  //IDLE检测到总线空闲
	{
		//1.清除中断标记位 (Reading SR then DR clears the IDLE flag)
		// Important: Use UART4, not USART1
		UART4->SR;
		UART4->DR;
		if(u4.len > 0){
			u4.v_flag = 1; 
		} else {
            u4.v_flag = 0;
        }
	}
}
