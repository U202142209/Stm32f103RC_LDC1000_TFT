#include "ldc1000.h"

#define SPI_RWBIT 		0x80

unsigned long ProximityData;
unsigned long FrequencyData;

volatile u32 speed = 999;

__IO uint16_t  DataRcv[5] ;


static void LDC_Delay_1ms(u32 time)
{
    u16 i = 0;
    while(time--)
    {
        i = 12000; //�Լ�����
        while(i--) ;
    }
}

/***********************��ʼ��spi***********************
*��    ��: ��ʼ��Ұ���spi
*��    ��:  void
*�� �� ֵ:void
*��    ע:
*****************************************************************/
extern void MY_SPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStr;
    GPIO_InitTypeDef GPIO_InitStr;

    /*ʹ��GPIOB,GPIOD,���ù���ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    /*ʹ��SPI1ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /*��Ӳ��stm32��SPI����*/
    /*���� SPI_LDC_SPI�� SCK,MISO,MOSI���ţ�GPIOA^5,GPIOA^6,GPIOA^7 */
    GPIO_InitStr.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP; //���ù���
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /*�Դӻ�LDC1000�Ŀ��ƽ�����*/
    /*����CE���ţ�GPIOA^2�� CSN ����: NSS GPIOA^1*/
    GPIO_InitStr.GPIO_Pin = GPIO_LDC_CSN_Pin;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /*��Ӳ��stm32��SPI����*/
    /*����SPI_LDC_SPI��IRQ���ţ�GPIOA^3*/
    GPIO_InitStr.GPIO_Pin = GPIO_LDC_IQ_Pin;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU ;  //��������
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /* �����Զ���ĺ꣬��������csn���ţ�LDC�������״̬ */
    LDC_CSN_HIGH();

    SPI_InitStr.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫��
    SPI_InitStr.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
    SPI_InitStr.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
    SPI_InitStr.SPI_CPOL = SPI_CPOL_High;		 				//ʱ�Ӽ��ԣ�����ʱΪ��
    SPI_InitStr.SPI_CPHA = SPI_CPHA_2Edge;						//��1��������Ч��������Ϊ����ʱ��
    SPI_InitStr.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
    SPI_InitStr.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //8��Ƶ��9MHz
    SPI_InitStr.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
    SPI_InitStr.SPI_CRCPolynomial = 7;                          //CRCУ�鸴λ
    SPI_Init(SPI1, &SPI_InitStr);

    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);
}

/***********************��LDC�ж�/дһ���ֽ�***********************
*��    ��: ��LDC�ж�/дһ���ֽ�
*��    ��:  д�������
*�� �� ֵ:  ��ȡ�õ�����
*��    ע:
*****************************************************************/
u8 SPI_LDC_RW(u8 data) //��LDC�ж�дһ���ֽ�
{
    /* �� SPI���ͻ������ǿ�ʱ�ȴ� */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* ͨ�� SPI2����һ�ֽ����� */
    SPI_I2S_SendData(SPI1, data);

    /* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/***********************��LDCָ���ļĴ���дֵ***********************
*��    ��: ��LDCָ���ļĴ���дֵ
*��    ��: reg:LDC������+�Ĵ�����ַ��
*    	   data:��Ҫ��Ĵ���д�������
*�� �� ֵ: �Ĵ�����״̬
*��    ע:
*****************************************************************/
static void SPI_LDC_WriteReg(u8 reg, u8 data)
{
    // u8 status;

//    LDC_CE_LOW();//�������ģʽ1���͹���ģʽ����ģʽ��LDC���Խ�������
    LDC_CSN_LOW();//����csnƬѡ�źţ�ʹ��LDC��spi����

    SPI_LDC_RW(reg);//��LDC��������ͼĴ����ĺ�

    SPI_LDC_RW(data);//��ղ�ָ���ļĴ���д������

    LDC_CSN_HIGH();//����csnƬѡ�źţ����ͷ�LDC��spi������ϣ�
}

/***********************��ȡLDCָ���ļĴ���ֵ***********************
*��    ��: ��ȡLDCָ���ļĴ���ֵ
*��    ��: reg:LDC�Ĵ�����ַ��
*�� �� ֵ: ״̬�Ĵ���������
*��    ע:
*****************************************************************/
static u8 SPI_LDC_ReadReg(u8 reg)
{
    u8 Reg_Value;

//    LDC_CE_LOW();//�������ģʽ1���͹���ģʽ����ģʽ��LDC���Խ�������
    LDC_CSN_LOW();//����csnƬѡ�źţ�ʹ��LDC��spi����

    SPI_LDC_RW(reg | SPI_RWBIT); //ѡ��Ĵ���
    Reg_Value = SPI_LDC_RW(SPI_NOP);

    LDC_CSN_HIGH();//����csnƬѡ�źţ����ͷ�LDC��spi������ϣ�

    return Reg_Value;
}

/********************************************************************
 * @brief��		LDC1000��ʼ������,ps:��SPI������������λ16�����ݳ��ȣ���
 * 				�ڷ�������ʱ���Խ���ַ��ֵ���л�����һ���ͳ�ȥ
 * @param:		none
 * @return:		none
 *********************************************************************/
void LDC1000_init(void)
{
//	SPI_Write(LDC1000_CMD_RPMAX<<8|0xff);
    SPI_LDC_WriteReg(LDC1000_CMD_RPMAX, 0x09);					//����Rp_MAX(0x01)�Ĵ���
    SPI_LDC_WriteReg(LDC1000_CMD_RPMIN, 0x37);					//����Rp_MIN(0x02)�Ĵ���
    SPI_LDC_WriteReg(LDC1000_CMD_SENSORFREQ, 0xE2);						//����Sensor Frequency(0x03)�Ĵ���
    SPI_LDC_WriteReg(LDC1000_CMD_LDCCONFIG, 0x17);							//����LDC Configuration(0x04)�Ĵ���
    SPI_LDC_WriteReg(LDC1000_CMD_CLKCONFIG, 0x00);							//����Clock Configuration(0x05)�Ĵ���,
    //ʹ��TBCLK��Ϊʱ��Դ																		//����INTBΪ�Ƚ������־λ(status of Comparator output)
    SPI_LDC_WriteReg(LDC1000_CMD_THRESHILSB, 0x50);						//����Comparator Threshold High(0x06)�Ĵ�����8λ
    SPI_LDC_WriteReg(LDC1000_CMD_THRESHIMSB, 0x14);						//����Comparator Threshold High(0x07)�Ĵ�����8λ
    SPI_LDC_WriteReg(LDC1000_CMD_THRESLOLSB, 0xC0);						//����Comparator Threshold Low(0x08)�Ĵ�����8λ
    SPI_LDC_WriteReg(LDC1000_CMD_THRESLOMSB, 0x12);						//����Comparator Threshold Low(0x09)�Ĵ�����8λ
    SPI_LDC_WriteReg(LDC1000_CMD_INTCONFIG, 0x02);							//����INTB Pin Configuration(0x0A),
    SPI_LDC_WriteReg(LDC1000_CMD_PWRCONFIG, 0x01);							//����Power Configuration(0x0B)�Ĵ�����																	//ΪActive Mode,ʹ��ת��
}

/********************************************************************
 * @brief��		ʹ��SPI��ȡLDC1000�е�����
 * @param:		none
 * @return:		none
 *********************************************************************/
void LDCRead(void)
{
//	LDC_CSN_LOW();
    ProximityData = 0;
    FrequencyData = 0;
    while(LDC_Read_IRQ() != 0);
    DataRcv[0] = SPI_LDC_ReadReg(LDC1000_CMD_PROXLSB);          			//д�뽫Ҫ��ȡ��Proximity Data LSB�Ĵ�����ַ(0x21)
    //printf("0 %d\r\n",DataRcv[0]);
    //SPI_Read(&DataRcv[0]);                       			//��ȡ�����Ĵ����е�ֵ��������DataRcv[0]
    ProximityData |= DataRcv[0] ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[1] = SPI_LDC_ReadReg(LDC1000_CMD_PROXMSB);					//д�뽫Ҫ��ȡ��Proximity Data MSB�Ĵ�����ַ(0x22)
    //printf("1 %d\r\n",DataRcv[1]);
    //SPI_Read(&DataRcv[1]);									//��ȡ�����Ĵ����е�ֵ��������DataRcv[1]
    ProximityData |= (DataRcv[1] << 8) ;									//��ϳ�ProximityData
    while(LDC_Read_IRQ() != 0);
    DataRcv[2] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRLSB);					//д�뽫Ҫ��ȡ��Frequency Counter Data LSB�Ĵ�����ַ(0x23)
    //printf("2 %d\r\n",DataRcv[2]);
    //SPI_Read(&DataRcv[2]);									//��ȡ�����Ĵ����е�ֵ��������DataRcv[2]
    FrequencyData |= DataRcv[2] ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[3] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRMID);					//д�뽫Ҫ��ȡ��Frequency Counter Data Mid-Byte�Ĵ�����ַ(0x24)
    //printf("3 %d\r\n",DataRcv[3]);
    //SPI_Read(&DataRcv[3]);									//��ȡ�����Ĵ����е�ֵ��������DataRcv[3]
    FrequencyData |= (DataRcv[3] << 8) ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[4] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRMSB);					//д�뽫Ҫ��ȡ��Frequency Counter Data MSB�Ĵ�����ַ(0x25)
    //printf("4 %d\r\n\n\n",DataRcv[4]);
    //SPI_Read(&DataRcv[4]);									//��ȡ�����Ĵ����е�ֵ��������DataRcv[4]
    FrequencyData |= (DataRcv[4] << 16) ;									//��ϳ�FrequencyData
//	LDC_CSN_HIGH();
}

extern void SPI_LDC_Init(void)
{
    TIM3_Init();
    MY_SPI_Init();
    LDC1000_init();
}

/*
 * ��������TIM3_NVIC_Configuration
 * ����  ��TIM3�ж����ȼ�����
 * ����  ����
 * ���  ����
 */
void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



static void TIM3_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void TIM3_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM�źŵ�ƽ����ֵ */
    u16 CCR3_Val = 2;

    /* -----------------------------------------------------------------------
    TIME3 ������� 4 · PWM ���Σ�
    TIME3_CH1	------	PA6
    TIME3_CH2	------	PA7
    TIME3_CH3	------	PB0
    TIME3_CH4	------	PB1
    ----------------------------------------------------------------------- */

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 3;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //����Ԥ��Ƶ��Ԥ��Ƶ72����Ϊ1KHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//����ʱ�ӷ�Ƶϵ��������Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    		/* �������жϱ�־ */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 //ʹ��ͨ��1

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);	 //ʹ��CCR3�Ĵ�����Ӱ�ӼĴ�����ֱ�����������¼��ŷ������ģ�

    //TIM_ARRPreloadConfig(TIM3, ENABLE);			 // ʹ��TIM3���ؼĴ���ARR

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);                   //ʹ�ܶ�ʱ��3
}


void TIM3_Init(void)
{
    //TIM3_NVIC_Configuration();
    TIM3_GPIO_Config();
    TIM3_Mode_Config();
}
