#ifndef _LED_SMG_H
#define _LED_SMG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// STM32开发板
//数码管驱动代码	   
//创建日期:2019/4/26
//版本：V1.8								  
////////////////////////////////////////////////////////////////////////////////// 

////74HC138操作线
#define LED_A0 PCout(10) //A0地址线
#define LED_A1 PCout(11) //A1地址线
#define LED_A2 PCout(12) //A2地址线

////74HC595操作线
#define LED_DS		PBout(3) //数据线
#define LED_LCLK	PBout(4) //锁存时钟线
#define LED_SCK		PBout(5) //时钟线

void LED_SMG_Init(void);
void LED_Refresh(void);
void LED_Write_Data(u8 duan,u8 wei);

#endif



