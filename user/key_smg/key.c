#include "key.h"
#include "delay.h"

/*
extern u8 LEDFlag; //��־λ
extern u16 delayTime; //��ʱ
*/

//������ʼ������(��ͨ����)
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;   //PA4 5 6 7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //����
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

}

//����������(��ͨ����)
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;   //�����ɿ���־
    if(mode == 1)key_up = 1; //֧������
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
    return 0;   //�ް�������
}

// ������ʼ���������жϷ�����
void KEY_Init_Exit(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��

    GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;   //PA4 5 6 7
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;    //�½��ش���
    GPIO_Initure.Pull = GPIO_PULLUP;         //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;    //����
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0); //��ռ���ȼ�Ϊ 2�������ȼ�Ϊ 0
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);        //ʹ���ж���4
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0); //��ռ���ȼ�Ϊ 2�������ȼ�Ϊ 0
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);        //ʹ���ж���9~5


}

// KEY0���жϷ�����
void EXTI4_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

// KEY1~3���жϷ�����
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

//�жϷ����������Ҫ��������
//�� HAL �������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(50); //����

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

