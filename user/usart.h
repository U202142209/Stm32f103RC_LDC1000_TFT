#ifndef _USART_H_
#define  _USART_H_

#include<stm32f10x.h>
#include<stdio.h>
#include <stdarg.h>
#include "delay.h"

void USART1_Config (void);//其TX为A9,RX为A10
void USART2_Confing(void);//其TX为A2，RX为A3
int fputc(int ch,FILE *f);
char * itoa(int value,char * string,int radix);
void USART_printf(USART_TypeDef * USARTx,uint8_t *Data,...);
void NVIC_USART2_Config(void);


#endif
