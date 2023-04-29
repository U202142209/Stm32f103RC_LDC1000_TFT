/************************************************************************
函数功能：LDC1000模块程序
设 计 人：XL
版    本：1.0
完成时间：2020年7月9日12:14:54
库版本  ：v3.5.0
修改时间：
************************************************************************/


#include <stm32f10x.h>
#include "delay.h"
#include "QDTFT_demo.h"

#include "usart.h"
#include "ldc1000.h"


extern unsigned long ProximityData;                                           //LDC上Proximity Data
extern unsigned long FrequencyData;


int main(void)
{
    USART1_Config();			// 配置串口
    SPI_LDC_Init();				// 初始化LDC1000
    ````
    selftest();
    /*
    while(1)
    {
    // LDCRead();
    // printf("proximitydata: %d, frequencydata: %d\r\n", ProximityData, FrequencyData);
    // 自定义程序入口


    		//--加入检测处理程序--
    //------------------
    int i, j;
    for(i = 0; i < 10000; i++)
    for(j = 0; j < 1000; j++)
    ;
    }
    */

}
}
