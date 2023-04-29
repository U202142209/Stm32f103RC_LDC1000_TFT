#include "key.h"
#include "delay.h"

/*
extern u8 LEDFlag; //标志位
extern u16 delayTime; //延时
*/

//按键初始化函数(普通方法)
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;   //PA4 5 6 7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //输入
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

}

//按键处理函数(普通方法)
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //按键松开标志
    if(mode == 1)key_up = 1; //支持连按
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if(KEY0 == 0)       return KEY0_PRES;
        else if(KEY1 == 0)  return KEY1_PRES;
        else if(KEY2 == 0)  return KEY2_PRES;
        else if(KEY3 == 0)  return KEY3_PRES;
    }
    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1)key_up = 1;
    return 0;   //无按键按下
}

// 按键初始化函数（中断方法）
void KEY_Init_Exit(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;   //PA4 5 6 7
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;    //下降沿触发
    GPIO_Initure.Pull = GPIO_PULLUP;         //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    //高速
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0); //抢占优先级为 2，子优先级为 0
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);        //使能中断线4
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0); //抢占优先级为 2，子优先级为 0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);        //使能中断线9~5


}

// KEY0的中断服务函数
void EXTI4_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

// KEY1~3的中断服务函数
void EXTI9_5_IRQHandler()
{
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != 0x00u)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    }
    else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != 0x00u)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    }
    else if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != 0x00u)
    {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    }
}

//中断服务程序中需要做的事情
//在 HAL 库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(50); //消抖

    /*
    switch(GPIO_Pin)
    {
    // KEY0
    case GPIO_PIN_4:
    	if(delayTime>50)
    		delayTime-=50;
    	break;
    // KEY1
    case GPIO_PIN_5:
    	LEDFlag=1;
    	break;
    // KEY2
    case GPIO_PIN_6:
    	delayTime+=100;
    	break;
    // KEY3
    case GPIO_PIN_7:
    	LEDFlag=0;
    	break;
    }
    */
}

