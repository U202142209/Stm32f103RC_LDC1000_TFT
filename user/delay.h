/************************************************************************ 
*    文 件 名：delay.h
*    文件描述：通用延时子程序 
*    创 建 人：管苡锋
*    版 本 号：1.0 
************************************************************************/ 
#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x_it.h"

#define uchar unsigned char
#define uint unsigned int

void SysTick_Init(void);
void TimeingDelay(void);
void delay1us(__IO uint t);
extern void delay1ms(uint x);
extern void delay10ms(void);
//extern void delay30ms(void);
extern void delay200ms(void);
//extern void delay500ms(void);
extern void delay1s(void);

#endif
