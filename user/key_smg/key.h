#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
// 2020/11/27
// V1
//////////////////////////////////////////////////////////////////////////////////



//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)  //KEY0按键PA4
#define KEY1        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)  //KEY1按键PA5
#define KEY2        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)  //KEY2按键PA6
#define KEY3        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)  //KEY3按键PA7

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define KEY3_PRES   4

void KEY_Init(void);
void KEY_Init_Exit(void);
u8 KEY_Scan(u8 mode);
#endif
