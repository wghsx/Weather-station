#ifndef _USART_H
#define _USART_H

typedef struct{
	u8 rec_str[255];
	u8 len;
	u8 rec_value;
}REC_BUF;

extern REC_BUF rec_buf;

void USART1_Init(void);
void USART1_SendChar(uint8_t data);
uint8_t USART1_ReceiveChar(void);
void USART1_SendString(char* str);
	
#endif
