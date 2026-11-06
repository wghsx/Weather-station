#include "wifi.h"

/*
	函数功能：USART3初始化
	函数参数：无
	函数返回值：无
	WIFI_ENABLE  ---  PC9  ---  通用推挽输出
			输出高电平:使能WIFI模块
			输出低电平:禁止WIFI模块
	USART3_TX   ---  PB10   ---   复用推挽输出
	USART3_RX   ---  PB11   ---   浮空输入
*/
void WIFI_Init(void)
{
	
	//1、打开时钟  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//WIFI模块使能管脚配置
	GPIO_InitTypeDef  GPIO_InitStruct={0};//定义结构体变量
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//将WIFI模块使能   将PC9拉高
	GPIOC->ODR |= (1 << 9);
	//WIFI模块使能需要上电时间
	Delay_ms(2000);
	
	//2、IO口初始化  ---  标准库函数进行编程（函数调用）
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//3、完成USART3初始化   usart
	USART_InitTypeDef USART_InitStruct={0};
	USART_InitStruct.USART_BaudRate            = 115200;                      //设置波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;//接收器使能和发送器使能
	USART_InitStruct.USART_Parity              = USART_Parity_No;              //不使用奇偶校验
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;             //停止位为1bit
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;          //数据位大小为8bit
	USART_Init(USART3,&USART_InitStruct);
	
	//使能接收中断和空闲中断   XXX_ITConfig();
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	//配置NVIC
	NVIC_InitTypeDef NVIC_InitStruct={0};
	//选择中断源  查找中断源名称：CTRL + F --> Find in files --> 搜索：IRQn
	NVIC_InitStruct.NVIC_IRQChannel                   = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;     //ENABLE:使能   DISABLE:禁止
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 7;          //设置抢占优先级  0 - 15 注意：标号越小，优先级越高
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;          //设置次级优先级  无
	NVIC_Init(&NVIC_InitStruct);
	//4、使能USART1
	USART_Cmd(USART3,ENABLE);
}

/*
	函数功能：USART3中断服务函数
	函数参数：无
	函数返回值：无
	中断服务函数格式和名字固定
	查找中断服务函数名字方法：
	CTRL + F    -->  Find in Files  -->  搜索：IRQH
	注意：不需要声明和调用，此函数达到条件时，自动触发
*/
U3 u3;
void USART3_IRQHandler(void)
{
	
	//判断进入接收中断 -- 判断接收中断标志位是否1
	if(USART_GetITStatus(USART3,USART_IT_RXNE))
	{
		//接收PC端过来数据
		u3.buff[u3.len++] = USART_ReceiveData(USART3);
	}
	//判断进入空闲中断 -- 判断接收空闲标志位是否1
	if(USART_GetITStatus(USART3,USART_IT_IDLE))
	{
		//清零空闲中断标准方法：先读取一次状态寄存器，再去读取一次数据寄存器
		USART3->SR;
		USART3->DR;
		//代表数据接收完成
		u3.buff[u3.len] = '\0';//字符串结尾补充结束符
		u3.len = 0;            //清空数据长度，准备接收下一字节数据
		u3.flag = 1;           //代表数据接收完成
		
		//接收数据完成，将数据回显到XCOM
		printf("%s",u3.buff);	
	}
}



/*
	USART3发送1字节数据至PC
	u8 data : 发送的1byte数据
	利用这个函数，将1byte发送到PC端
*/
void usart3_send_byte(u8 data)
{
	//1、等待发送标志位置1
	while(!USART_GetFlagStatus(USART3,USART_FLAG_TXE));
	//2、发送数据
	USART_SendData(USART3,data);
	
}

//USART3发送字符串
void usart3_send_str(u8 *data)
{
	
	while(*data != '\0')
	{
		usart3_send_byte(*data);
		data++;
	}
}



//WIFI_TEST：练习2
void WIFI_TEST(void)
{
	//注意：每一个AT指令结尾都要有\r\n
	usart3_send_str((u8 *)"AT\r\n");
}

//WIFI模块发送AT指令函数
u8 WIFI_SendData(u8 *data,u16 timeout)
{
	
	usart3_send_str(data);
	Delay_ms(300);
	while(1)
	{
		if(u3.flag == 1)
			u3.flag = 0;
		if(strstr((const char *)u3.buff,"OK") != NULL)
		{
			return 1;//设置成功
		}
		timeout--;
		Delay_ms(1);
		if(timeout == 0)
			return 0;//设置失败
	}
}


//WIFI模块联网
u8 WIFI_Connect(void)
{
	/*
		LCD屏幕显示正在连接网络
		语音播报：正在连接网络
	*/
	
	u8 ret = 0;
	//1、设置客户端模式 //服务端模式
	ret = WIFI_SendData((u8 *)"AT+CWMODE=1\r\n",300);
	if(ret == 0)
		{return 0;}
		printf("设置客户端模式成功\r\n");
	//2、设置透传模式
	ret = WIFI_SendData((u8 *)"AT+CIPMODE=1\r\n",300);
	if(ret == 0)
		{return 0;}
		printf("设置透传模式成功\r\n");
	//3、连接网络
	ret = WIFI_SendData((u8 *)"AT+CWJAP=\"YOUR_WIFI_SSID\",\"YOUR_WIFI_PASSWORD\"\r\n",10000);
	if(ret == 0)
		{return 0;}
		printf("连接网络成功\r\n");
	
	//4、连接服务器
	ret = WIFI_SendData((u8 *)"AT+CIPSTART=\"TCP\",\"www.yytianqi.com\",80\r\n",10000);
	if(ret == 0)
		{return 0;}
		printf("连接服务器成功");
	
	ret = WIFI_SendData((u8 *)"AT+CIPSEND\r\n",300);
	if(ret == 0)
		{return 0;}
		printf("进入透传成功\r\n");
	
	return 1;  //失败则返回1
	
}


/*
	获取东莞天气
	注意：此函数只有连接网络成功后才能使用
*/
u8 WIFI_GetWeather(void)
{
	u16 timeout = 10000;

	//http://api.yytianqi.com/接口名称?city=城市ID&key=用户key
	//获取湛江天气实况   湛江城市ID：CH281001
	//东莞  CH281601  key=YOUR_API_KEY
	while(1)
	{
		//usart3_send_str((u8 *)"GET http://api.yytianqi.com/observe?city=CH281001&key=YOUR_API_KEY\r\n");
		usart3_send_str((u8 *)"GET http://api.yytianqi.com/observe?city=CH281601&key=YOUR_API_KEY\r\n");
		Delay_ms(50);
		
		
		if(strstr((const char *)u3.buff,"\"numtq\"")!=NULL && u3.flag == 1)
		{
			return 0;
		}
		Delay_ms(1);
		timeout--;
		if(timeout == 0)
		{
			Voice_Num(2,9);
			return 1;  
			
		}
	}
}


//解析天气函数
void Check_Weather(void)
{
	
	WIFI_GetWeather();  //获取到天气状态后，根据天气不同播报天气
	
	//运行这个函数之前先要获取天气
	u3.flag = 0;
	printf("获取天气数据成功\r\n");
	//JSON格式的键值对
	//"\"numtq\":\"00\"" 是要查找的子字符串，实际内容是："numtq":"00"（双引号被转义）
	if(strstr((const char *)u3.buff,"\"numtq\":\"00\"") != NULL)
	{
		//获取到天气为晴天
		Voice_Num(3,00);
		//晴天图片
		//LCD_ShowPicture(0,0,305,155,(u8*)gImage_tt);
		printf("此时天气晴天\r\n");
	}
	if(strstr((const char *)u3.buff,"\"numtq\":\"01\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,01);
		
		printf("此时天气多云\r\n");
	}
		if(strstr((const char *)u3.buff,"\"numtq\":\"09\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,9);
		
		printf("此时天气大雨\r\n");
	}
	if(strstr((const char *)u3.buff,"\"numtq\":\"02\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,2);
	
		printf("此时天气阴天\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"03\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,3);
		
		printf("此时天气阵雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"04\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,4);
		
		printf("此时天气雷阵雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"05\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,5);
		printf("此时天气雷阵雨伴有冰雹\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"06\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,6);
		printf("此时天气雨夹雪\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"07\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,7);
		
		printf("此时天气小雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"08\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,8);
		
		printf("此时天气中雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"10\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,10);
	
		printf("此时天气暴雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"11\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,10);
		
		printf("此时天气大暴雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"12\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,10);
	
		printf("此时天气特大暴雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"18\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,18);

		printf("此时天气雾天\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"21\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,11);

		printf("此时天气小到中雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"22\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,11);

		printf("此时天气小到中到大雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"23\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,11);
	
		printf("此时天气大到暴雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"24\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,10);

		printf("此时天气暴雨到大暴雨\r\n");
	}	
	if(strstr((const char *)u3.buff,"\"numtq\":\"25\"") != NULL)
	{
		//获取到天气为多云
		Voice_Num(3,10);

		printf("此时天气大暴雨到特大暴雨\r\n");
	}	
}
