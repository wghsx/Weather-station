#ifndef _MY1600_H
#define _MY1600_H

void MY1600_Init(void);
void UART5_Send_Data(u8* data,u8 len);
void Voice_Play(void);
void Voice_Stop(void);
void Voice_Back(void);
void Voice_Next(void);
void Voice_Sound(void);
void Voice_Num(u8 dir,u8 num);

#endif
