/************************************************************************
函数功能：LDC1000模块程序
设 计 人：XL
版    本：1.0
完成时间：2020年7月9日12:14:54
库版本  ：v3.5.0
修改时间：
************************************************************************/
																				
/******************************************************************************
*
相关说明：该程序实现了STM32单片机控制串口的一系列操作
                          本程序给出了用户串口中断接收的服务程序
                          若要使用改程序则要注意在全局变量定义一个
                          READ_Buf[]数组并将下面注释的串口服务程序加入到
                          Stm32f10x_it.c中
接口定义：
******************************************************************************/
#include<usart.h>

void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitSturcture;
    USART_InitTypeDef USART_InitSturcture;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitSturcture.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitSturcture.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitSturcture.GPIO_Speed=GPIO_Speed_50MHz;

    GPIO_Init(GPIOA,&GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin=GPIO_Pin_10;
    GPIO_InitSturcture.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&GPIO_InitSturcture);

    USART_InitSturcture.USART_BaudRate=115200;
    USART_InitSturcture.USART_WordLength=USART_WordLength_8b;
    USART_InitSturcture.USART_StopBits=USART_StopBits_1;
    USART_InitSturcture.USART_Parity=USART_Parity_No;
    USART_InitSturcture.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    USART_InitSturcture.USART_HardwareFlowControl=USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitSturcture);
    USART_Cmd(USART1, ENABLE);
}

void USART2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitSturcture;
    USART_InitTypeDef USART_InitSturcture;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitSturcture.GPIO_Pin=GPIO_Pin_2;
    GPIO_InitSturcture.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitSturcture.GPIO_Speed=GPIO_Speed_50MHz;

    GPIO_Init(GPIOA,&GPIO_InitSturcture);

    GPIO_InitSturcture.GPIO_Pin=GPIO_Pin_3;
    GPIO_InitSturcture.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&GPIO_InitSturcture);

    USART_InitSturcture.USART_BaudRate=9600;
    USART_InitSturcture.USART_WordLength=USART_WordLength_8b;
    USART_InitSturcture.USART_StopBits=USART_StopBits_1;
    USART_InitSturcture.USART_Parity=USART_Parity_No;
    USART_InitSturcture.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
    USART_InitSturcture.USART_HardwareFlowControl=USART_HardwareFlowControl_None;

    USART_Init(USART2, &USART_InitSturcture);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

void NVIC_USART2_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStr;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//配置中断为响应式优先级
    NVIC_InitStr.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStr.NVIC_IRQChannelSubPriority = 0; //中断优先级设置为最高
    NVIC_InitStr.NVIC_IRQChannelCmd = ENABLE;   

    NVIC_Init(&NVIC_InitStr);
}

/*为重定义printf函数所建立的函数*/
int fputc(int ch,FILE *f)
{
USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_SendData(USART1, (unsigned char)ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
    return (ch);
}

char * itoa(int value,char * string,int radix)
{
    int d,i;
    char flag=0;
    char *p=string;
    if(radix!=10)
    {
        *p=0;
        return(string);
    }
    if(!value)
    {
        *p++=0x30;
        *p=0;
        return(string);
     }
    if(value<0)
    {
        *p++='-';
        value*=-1;
    }
    for(i=10000;i>0;i/=10)
	{
		d=value/i;
		if(d||flag)
		{
			*p++=(char)(d+0x30);
			value-=(d*i);
			flag=1;
		}
	}
*p=0;
return (string);
}

void USART_printf(USART_TypeDef * USARTx,uint8_t *Data,...)
{
    const char *s;
    char buf[16];
    int d;
    //char c;
    va_list ap;
    va_start(ap,Data);
    while(*Data!=0)//判断是字符串是否结束
    {
    USART_ClearFlag(USARTx,USART_FLAG_TC);
        if(*Data==0x5c)//判断是否是"\"
        {
            switch(*++Data)
            {
                case 'r':             //回车符其ascll为13也就是0x0d
                        USART_SendData(USARTx,0x0d );
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                case 'n':         //换行符其ascll为10也就是0x0a
                        USART_SendData(USARTx, 0X0a);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                default :
                        USART_SendData(USARTx, *Data);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;                              
            }
        }
        else
        {
            if(*Data==0x25)
            {
                switch (*++Data)
                {
                    case 's':
                        s=va_arg(ap,const char* );
                        for(;*s;s++)
                        {
                            USART_SendData(USARTx,*s );
                            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        }
                        Data++;
                        break;
                    case 'd':
                        d=va_arg(ap,int);
                        itoa(d,buf,10);
                        for(s=buf;*s;s++)
                        {
                            USART_SendData(USARTx,*s);
                            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        }
                        Data++;
                        break;
                    case 'c':
                        d=va_arg(ap,int);
                        USART_SendData(USARTx,d);
                        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
                        Data++;
                        break;
                   default:
                        Data++;
                        break;                                                  
                }
            }
            else
            {
                USART_SendData(USARTx, *Data++);
                while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);
            }
        }
        //delay10ms();
    }
    va_end(ap);
    
}

//串口中断服务程序
/*
void USART2_IRQHandler()
{
   if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)//中断产生
   {
       if(READ_Point<READ_READ_Buf_Max)
       {
            USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除中断标志
            Uart2_Buffer[READ_Point]=USART_ReceiveData(USART2);
            READ_Point++;
        }
    }
}
*/
    
