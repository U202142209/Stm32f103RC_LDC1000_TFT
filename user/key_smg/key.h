#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
// 2020/11/27
// V1
//////////////////////////////////////////////////////////////////////////////////



//����ķ�ʽ��ͨ��ֱ�Ӳ���HAL�⺯����ʽ��ȡIO
#define KEY0        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)  //KEY0����PA4
#define KEY1        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)  //KEY1����PA5
#define KEY2        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)  //KEY2����PA6
#define KEY3        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)  //KEY3����PA7

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define KEY3_PRES   4

void KEY_Init(void);
void KEY_Init_Exit(void);
u8 KEY_Scan(u8 mode);
#endif
