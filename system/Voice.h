#ifndef _VOICE_H
#define _VOICE_H

#include "stdio.h"


typedef struct{
	u8 buff[250];     //存储数据
	u16 len;          //计数接收数据大小
	u8 v_flag;        //记录数据是否接收完成 0：未完成，1：完成
}U4;

void Voice_Init(void);
void Voice_Recognition(void);
#endif
