#include "Lcd.h"





/*
	函数功能：LCD初始化
	函数参数：无
	函数返回值：无
	备注：
	PC6--LCD_CS      
	PC7--LCD_D/C               
	PC8--LCD_RST     
	PB12--LCD_BL
  PB13--LCD_SCL
  PB15--LCD_SDA
	推挽输出
*/
void LCD_IO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure = {0}; 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;					//推挽输出
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         //快速
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//设置IO口默认电平--器件初始状态
	LCD_CS_H;    //片选拉高，不工作
	LCD_RST_H;   //复位管教拉高
}


/*
	函数功能：spi数据交换函数
	函数参数：u8 data
	函数返回值：无
	备注：IO模拟SPI（串行外设接口），一次发送8bit数据，高位先出
*/
void SPI_TransferData(u8 data)
{
	for(u8 i = 0; i < 8; i++)
	{
		LCD_SCL_L;            //拉低时钟线，单片机发送数据到ST7789
		if(data & 0x80 >> i)
			LCD_SDA_H;          //拉高数据线，发送数据“1”
		else
			LCD_SDA_L;					//拉低数据线，发送数据“0”
		LCD_SCL_H; //拉高时钟线，ST7789接收单片机刚刚发来的数据
	}
}


/*
	函数功能：发送命令函数
	函数参数：u8 cmd
	函数返回值：无
*/
void ST7789_Write_Command(u8 cmd)
{
	//1.选择发送命令
	LCD_CMD;
	//2.拉低片选，开始通信
	LCD_CS_L;
	//3.发送命令
	SPI_TransferData(cmd);
	//4.拉高片选，结束通信
	LCD_CS_H;
}


/*
	函数功能：发送8bit函数
	函数参数：u8 data
	函数返回值：无
*/
void ST7789_Write_Parameter(u8 data)
{
	//1.选择发送数据
	LCD_DATA;
	//2.拉低片选，开始通信
	LCD_CS_L;
	//3.发送命令
	SPI_TransferData(data);
	//4.拉高片选，结束通信
	LCD_CS_H;
}



/*
	函数功能：发送16bit函数
	函数参数：u16 data
	函数返回值：无
*/
void ST7789_Write_data(u16 data)
{
	//1.选择发送数据
	LCD_DATA;
	//2.拉低片选，开始通信
	LCD_CS_L;
	//3.发送命令
	SPI_TransferData(data >> 8);  //先发送高8bit数据
	SPI_TransferData(data);       //再发低8bit数据
	//4.拉高片选，结束通信
	LCD_CS_H;
}


/*
	函数功能：设置行地址和列地址（显示内容位置）
	函数参数：
						u16 xs   -- x轴起始坐标 
						u16 ys   -- y轴起始坐标 
						u16 xe   -- x轴结束坐标 
						u16 ye   -- y轴结束坐标
	函数返回值：无
*/
void LCD_SetPosition(u16 xs, u16 ys, u16 xe, u16 ye)
{
	//设置列地址
	ST7789_Write_Command(0x2A);
	ST7789_Write_data(xs);
	ST7789_Write_data(xe - 1);   //数据地址下标从0开始
	//设置行地址
	ST7789_Write_Command(0x2B);
	ST7789_Write_data(ys);
	ST7789_Write_data(ye - 1);   //数据地址下标从0开始
	//设置存储寄存器命令
	ST7789_Write_Command(0x2C);
}


/*
	函数功能：LCD功能初始化
	函数参数：无
	函数返回值：无
*/
void LCD_Init(void)
{
	//1、LCD管脚初始化
	LCD_IO_Init();
	
	//2、ST7789芯片复位
	LCD_RST_L;//拉低复位管脚，ST7789产生复位
	Delay_ms(100);
	LCD_RST_H;//拉高复位管脚，ST7789结束复位
	Delay_ms(100);
	
	//3、ST7789命令设置
		//设置ST778VW芯片
	ST7789_Write_Command(0x11);
	Delay_ms(100); //Delay 120ms
	ST7789_Write_Command(0X36);// Memory Access Control
	ST7789_Write_Parameter(0x70);
	ST7789_Write_Command(0X3A);
	ST7789_Write_Parameter(0X05);
	//--------------------------------ST7789S Frame rate setting-------------------------
	ST7789_Write_Command(0xb2);
	ST7789_Write_Parameter(0x0c);
	ST7789_Write_Parameter(0x0c);
	ST7789_Write_Parameter(0x00);
	ST7789_Write_Parameter(0x33);
	ST7789_Write_Parameter(0x33);
	ST7789_Write_Command(0xb7);
	ST7789_Write_Parameter(0x35);
	//---------------------------------ST7789S Power setting-----------------------------
	ST7789_Write_Command(0xbb);
	ST7789_Write_Parameter(0x35);
	ST7789_Write_Command(0xc0);
	ST7789_Write_Parameter(0x2c);
	ST7789_Write_Command(0xc2);
	ST7789_Write_Parameter(0x01);
	ST7789_Write_Command(0xc3);
	ST7789_Write_Parameter(0x12);
	ST7789_Write_Command(0xc4);
	ST7789_Write_Parameter(0x20);
	ST7789_Write_Command(0xc6);
	ST7789_Write_Parameter(0x0f);
	ST7789_Write_Command(0xca);
	ST7789_Write_Parameter(0x0f);
	ST7789_Write_Command(0xc8);
	ST7789_Write_Parameter(0x08);
	ST7789_Write_Command(0x55);
	ST7789_Write_Parameter(0x90);
	ST7789_Write_Command(0xd0);
	ST7789_Write_Parameter(0xa4);
	ST7789_Write_Parameter(0xa1);
	//--------------------------------ST7789S gamma setting------------------------------
	ST7789_Write_Command(0xe0);
	ST7789_Write_Parameter(0xd0);
	ST7789_Write_Parameter(0x00);
	ST7789_Write_Parameter(0x06);
	ST7789_Write_Parameter(0x09);
	ST7789_Write_Parameter(0x0b);
	ST7789_Write_Parameter(0x2a);
	ST7789_Write_Parameter(0x3c);
	ST7789_Write_Parameter(0x55);
	ST7789_Write_Parameter(0x4b);
	ST7789_Write_Parameter(0x08);
	ST7789_Write_Parameter(0x16);
	ST7789_Write_Parameter(0x14);
	ST7789_Write_Parameter(0x19);
	ST7789_Write_Parameter(0x20);
	ST7789_Write_Command(0xe1);
	ST7789_Write_Parameter(0xd0);
	ST7789_Write_Parameter(0x00);
	ST7789_Write_Parameter(0x06);
	ST7789_Write_Parameter(0x09);
	ST7789_Write_Parameter(0x0b);
	ST7789_Write_Parameter(0x29);
	ST7789_Write_Parameter(0x36);
	ST7789_Write_Parameter(0x54);
	ST7789_Write_Parameter(0x4b);
	ST7789_Write_Parameter(0x0d);
	ST7789_Write_Parameter(0x16);
	ST7789_Write_Parameter(0x14);
	ST7789_Write_Parameter(0x21);
	ST7789_Write_Parameter(0x20);
	ST7789_Write_Command(0x29);
	
	LCD_Clear(0,0,320,240,WHITE);//全屏清白色
	
	LCD_BLK_H;//打开背光板
}



/*
	函数功能：LCD清屏函数
	函数参数：u16 xs, u16 ys, u16 xe, u16 ye, u16 color
	函数返回值：无
*/
void LCD_Clear(u16 xs, u16 ys, u16 xe, u16 ye, u16 color)
{
	//1.设置清屏范围
	LCD_SetPosition(xs, ys, xe, ye);
	//2.向每一个像素点发送16bit数据
	for(u32 i = 0; i < (xe - xs) * (ye - ys); i++)
	{
		ST7789_Write_data(color);   //发送颜色数据
	}
}

//编写一个打点函数
void LCD_Point(u16 x, u16 y, u16 color)
{
	//1.设置清屏范围
	LCD_SetPosition(x, y, x, y);
	ST7789_Write_data(color);   //发送颜色数据
}


//写一个实心圆函数
//x^2+y^3=r^2
void LCD_Circle_Shi(u16 x, u16 y, u16 r, u16 color)
{
	u16 i, j;
	for(i = x-r; i <= x+r; i++)      //遍历横坐标
	{
		for(j = y-r; j <= y+r; j++)    //遍历纵坐标
		{
			if((i-x)*(i-x) + (j-y)*(j-y) <= r*r)   //判断遍历的点的总范围是否满足
			{
				LCD_Point(i, j, color);
			}
		}
	}
}


/*
	函数功能：LCD显示单个字符函数
	函数参数：u16 x, u16 y, u16 pin_color, u16 back_color, u16 ch, u8 size
	函数返回值：无
  参数：x坐标   y坐标   字符颜色   背景颜色   显示字符    字符大小
	size不同，范围不同
*/
void LCD_ShowChar(u16 x, u16 y, u16 pin_color, u16 back_color, u16 ch, u8 size)
{
	u8 i, j, n;
	u16 temp;
	
	n = ch - ' ';   //得到偏移值
	//1.确定范围  
	for(i = 0; i < size; i++)       //横遍历
	{
		if(size==16)	temp = ASC16[n*16+i];  //"- =2
		//2*i和2*i+1获取当前字符第i行的两个字节，因为32位需要两个字节表示
		//<<8 | 将第一个字节左移8位，然后与第二个字节拼接，组成16位数据
		if(size==32)  temp = ASC32[n*64+2*i]<<8 | ASC32[n*64+2*i+1];
		
		for(j = 0; j < size/2; j++)   //列遍历
		{
			//size/2 表示字符的宽度(这里是8)
			//1<<(size/2-1-j) 创建一个位掩码
			//size/2-1-j 计算要从右数第几位(因为j从0开始)
			//例如当j=0时，是第7位；j=1时是第6位，依此类推
			//temp & (1<<...) 检查temp的指定位是否为1
			if(temp & (1<<(size/2-1-j)))  //得到点亮的点--字符显示  0000 0000 
			{
				LCD_Point(x+j, y+i, pin_color);
			}
			else
			{
				LCD_Point(x+j, y+i, back_color);
			}
		}
	}
}



/*
	函数功能：LCD显示单个汉字函数
	函数参数：u16 x, u16 y, u16 pin_color, u16 back_color, u8 *ch, u8 size
	函数返回值：无
  参数：x坐标   y坐标   字符颜色   背景颜色   显示汉字    汉字大小
	size不同，范围不同
*/
void LCD_ShowHz(u16 x, u16 y, u16 pin_color, u16 back_color, u8 *ch, u8 size)  //"信"
{
	u8 i, j;
	u8 n = 0;
	u32 data;
	
	if(size==16)
	{
		//计算偏移值
		while(hz_16[2*n] != '\0')    //判断范围
		{
			if(*ch==hz_16[2*n] && *(ch+1)==hz_16[2*n+1])  //0 1==一个字
			{
				break;
			}
			n++;
		}
		if(hz_16[2*n] == '\0')
		{
			return;   //没有找到结束程序
		}
		//根据size大小遍历行数
		for(i=0; i < size; i++)
		{
			if(size==16) data = hz16_16[n*32+2*i] << 8 | hz16_16[n*32+2*i+1];
			
			for(j = 0; j < size; j++)
			{
				if(data & (1<<(size-1-j)))
				{
					LCD_Point(x+j, y+i, pin_color);
				}
				else
				{
					LCD_Point(x+j, y+i, back_color);
				}
			}	
		}
	}
	else if(size ==32)
	{
		//得到偏移值
		while(hz_32[2*n] != '\0')  //循环遍历数组
		{
			if(*ch == hz_32[2*n] && *(ch+1)==hz_32[2*n+1])
			{
				break;
			}
			n++;
		}
		if(hz_32[2*n] == '\0')
		{
			return;  //没有找到结束程序
		}
	}
	//根据字符大小遍历行数
	for(i = 0; i < size; i++)
	{
		if(size == 16)
			data = hz16_16[n*32+2*i] << 8 | hz16_16[n*32+2*i+1];
		else if(size ==32)
			data = hz32_32[n*128+4*i] << 24 | hz32_32[n*128+4*i+1]<<16 | hz32_32[n*128+4*i+2]<<8 | hz32_32[n*128+4*i+3];
		
		//根据字符大小遍历列数
		for(j = 0; j < size; j++)
		{
			if(data & (1<<(size-1-j)))
			{
				LCD_Point(x+j,y+i,pin_color);
			}
			else
			{
				LCD_Point(x+j,y+i,back_color);
			}
		}
	}
}


/*
	函数功能：LCD显示字符汉字混合函数
	函数参数：u16 x, u16 y, u16 pin_color, u16 back_color, u8 *str, u8 size
	函数返回值：无
  参数：x坐标   y坐标   字符颜色   背景颜色   显示字符串    字符串大小
	size不同，范围不同
*/
void LCD_ShowString(u16 x, u16 y, u16 pin_color, u16 back_color, u8 *str, u8 size) //"asd你好"
{
	
	while(*str != '\0')
	{
		if(*str >= 32 && *str <= 127) //1.判断字符--基于ASCLL码值
		{
			LCD_ShowChar(x, y, pin_color,back_color, *str, size);
			str++;  //数组中指针偏移，字符递增
			x += size/2;  //屏幕显示多个数据，8/16地址地址
			if(x >= 320-1 - size)
			{
				y += size;
				x = 0;
			}
		}
		else    //2.判断汉字
		{
			LCD_ShowHz(x, y, pin_color, back_color, str, size);
			str += 2;
			x += size;   //16/32
			if(x >= 320-1 - size)
			{
				y += size;
				x = 0;
			}
		}
	}
}



/*
	函数功能：LCD显示图片函数
	函数参数：u16 xs, u16 ys, u16 xe, u16 ye, u8 *pic
	函数返回值：无
  参数：x坐标   y坐标  图片数据
*/
void LCD_ShowPicture(u16 xs, u16 ys, u16 xe, u16 ye, u8 *pic)
{
	//确定位置
	LCD_SetPosition(xs, ys, xe,ye);
	
	for(u32 i = 0; i < (xe-xs)*(ye-ys); i++)
	{
		//发送16bit数据函数
		ST7789_Write_data(pic[i*2+1]<<8 | pic[i*2]);
	}
}































