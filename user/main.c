/************************************************************************
�������ܣ�LDC1000ģ�����
�� �� �ˣ�XL
��    ����1.0
���ʱ�䣺2020��7��9��12:14:54
��汾  ��v3.5.0
�޸�ʱ�䣺
************************************************************************/


#include <stm32f10x.h>
#include "delay.h"
#include "QDTFT_demo.h"

#include "usart.h"
#include "ldc1000.h"


extern unsigned long ProximityData;                                           //LDC��Proximity Data
extern unsigned long FrequencyData;


int main(void)
{
    USART1_Config();			// ���ô���
    SPI_LDC_Init();				// ��ʼ��LDC1000
    ````
    selftest();
    /*
    while(1)
    {
    // LDCRead();
    // printf("proximitydata: %d, frequencydata: %d\r\n", ProximityData, FrequencyData);
    // �Զ���������


    		//--�����⴦�����--
    //------------------
    int i, j;
    for(i = 0; i < 10000; i++)
    for(j = 0; j < 1000; j++)
    ;
    }
    */

}
}
