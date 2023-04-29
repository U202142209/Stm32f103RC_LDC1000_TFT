#include "delay.h"

static __IO u32 TimingDelay;

/*
 *¿ªÆôÏµÍ³µÎ´ð
 */
void SysTick_Init(void)
{
    /* SystemFrequency / 1000    1ms
     * SystemFrequency / 100000  10us
     * SystemFrequency / 1000000 1us
     */
    if (SysTick_Config(SystemCoreClock / 1000000))
    { 
        /* Capture error */ 
        while (1);
    }
}

void TimingDelay_Decrement(void)
{
    if(TimingDelay  != 0x00)
    {
        TimingDelay--;
    }
}

void delay1us(__IO uint t)
{
    TimingDelay  = t;
    while(TimingDelay != 0);
}

void delay1ms(uint x)
{
    while(x--)
        delay1us(1000);
}

void delay10ms(void)
{
    delay1ms(10);
}

void delay200ms(void)
{
    delay1ms(200);
}


void delay1s(void)
{
    delay1ms(1000);
}
