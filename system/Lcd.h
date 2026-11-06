#ifndef _LCD_H
#define _LCD_H


#include "stm32f10x.h"
#include "Delay.h"
#include "font.h"



#define LCD_CS_L   (GPIO_ResetBits(GPIOC,GPIO_Pin_6))
#define LCD_CS_H   (GPIO_SetBits(GPIOC,GPIO_Pin_6))

#define LCD_CMD    (GPIO_ResetBits(GPIOC,GPIO_Pin_7))
#define LCD_DATA   (GPIO_SetBits(GPIOC,GPIO_Pin_7))

#define LCD_RST_L   (GPIO_ResetBits(GPIOC,GPIO_Pin_8))
#define LCD_RST_H   (GPIO_SetBits(GPIOC,GPIO_Pin_8))

#define LCD_BLK_L   (GPIO_ResetBits(GPIOB,GPIO_Pin_12))
#define LCD_BLK_H   (GPIO_SetBits(GPIOB,GPIO_Pin_12))

#define LCD_SCL_L   (GPIO_ResetBits(GPIOB,GPIO_Pin_13))
#define LCD_SCL_H   (GPIO_SetBits(GPIOB,GPIO_Pin_13))

#define LCD_SDA_L   (GPIO_ResetBits(GPIOB,GPIO_Pin_15))
#define LCD_SDA_H   (GPIO_SetBits(GPIOB,GPIO_Pin_15))



/* 颜色定义开始 */
#define WHITE         0xFFFF
#define BLACK         0x0000	  
#define BLUE          0x001F  
#define BRED          0XF81F
#define GRED 					0XFFE0
#define GBLUE					0X07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define GREEN         0x07E0
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN 			  0XBC40 //棕色
#define BRRED 			  0XFC07 //棕红色
#define GRAY  			  0X8430 //灰色
#define DARKBLUE      0X01CF	//深蓝色
#define LIGHTBLUE     0X7D7C	//浅蓝色  
#define GRAYBLUE      0X5458 //灰蓝色
#define LIGHTGREEN    0X841F //浅绿色
#define LGRAY 			  0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE     0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE        0X2B12 //浅棕蓝色(选择条目的反色)
/* 颜色定义结束 */




/******************初始化函数***************/
void LCD_IO_Init(void);
void SPI_TransferData(u8 data);
void ST7789_Write_Command(u8 cmd);
void ST7789_Write_Parameter(u8 data);
void ST7789_Write_data(u16 data);
void LCD_SetPosition(u16 xs, u16 ys, u16 xe, u16 ye);
void LCD_Init(void);
void LCD_Clear(u16 xs, u16 ys, u16 xe, u16 ye, u16 color);

/*********************功能函数****************************/
void LCD_Point(u16 x, u16 y, u16 color);
void LCD_Circle_Shi(u16 x, u16 y, u16 r, u16 color);
void LCD_ShowChar(u16 x, u16 y, u16 pin_color, u16 back_color, u16 ch, u8 size);
void LCD_ShowHz(u16 x, u16 y, u16 pin_color, u16 back_color, u8 *ch, u8 size);
void LCD_ShowString(u16 x, u16 y, u16 pin_color, u16 back_color, u8 *str, u8 size);
void LCD_ShowPicture(u16 xs, u16 ys, u16 xe, u16 ye, u8 *pic);






#endif
