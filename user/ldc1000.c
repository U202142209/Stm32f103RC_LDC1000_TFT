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
        i = 12000; //自己定义
        while(i--) ;
    }
}

/***********************初始化spi***********************
*功    能: 初始化野火的spi
*形    参:  void
*返 回 值:void
*备    注:
*****************************************************************/
extern void MY_SPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStr;
    GPIO_InitTypeDef GPIO_InitStr;

    /*使能GPIOB,GPIOD,复用功能时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

    /*使能SPI1时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    /*对硬件stm32的SPI配置*/
    /*配置 SPI_LDC_SPI的 SCK,MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7 */
    GPIO_InitStr.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /*对从机LDC1000的控制角配置*/
    /*配置CE引脚，GPIOA^2和 CSN 引脚: NSS GPIOA^1*/
    GPIO_InitStr.GPIO_Pin = GPIO_LDC_CSN_Pin;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /*对硬件stm32的SPI配置*/
    /*配置SPI_LDC_SPI的IRQ引脚，GPIOA^3*/
    GPIO_InitStr.GPIO_Pin = GPIO_LDC_IQ_Pin;
    GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStr.GPIO_Mode = GPIO_Mode_IPU ;  //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStr);

    /* 这是自定义的宏，用于拉高csn引脚，LDC进入空闲状态 */
    LDC_CSN_HIGH();

    SPI_InitStr.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //双线全双工
    SPI_InitStr.SPI_Mode = SPI_Mode_Master;	 					//主模式
    SPI_InitStr.SPI_DataSize = SPI_DataSize_8b;	 				//数据大小8位
    SPI_InitStr.SPI_CPOL = SPI_CPOL_High;		 				//时钟极性，空闲时为低
    SPI_InitStr.SPI_CPHA = SPI_CPHA_2Edge;						//第1个边沿有效，上升沿为采样时刻
    SPI_InitStr.SPI_NSS = SPI_NSS_Soft;		   					//NSS信号由软件产生
    SPI_InitStr.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //8分频，9MHz
    SPI_InitStr.SPI_FirstBit = SPI_FirstBit_MSB;  				//高位在前
    SPI_InitStr.SPI_CRCPolynomial = 7;                          //CRC校验复位
    SPI_Init(SPI1, &SPI_InitStr);

    /* Enable SPI1  */
    SPI_Cmd(SPI1, ENABLE);
}

/***********************向LDC中读/写一个字节***********************
*功    能: 向LDC中读/写一个字节
*形    参:  写入的数据
*返 回 值:  读取得的数据
*备    注:
*****************************************************************/
u8 SPI_LDC_RW(u8 data) //向LDC中读写一个字节
{
    /* 当 SPI发送缓冲器非空时等待 */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    /* 通过 SPI2发送一字节数据 */
    SPI_I2S_SendData(SPI1, data);

    /* 当SPI接收缓冲器为空时等待 */
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(SPI1);
}

/***********************向LDC指定的寄存器写值***********************
*功    能: 向LDC指定的寄存器写值
*形    参: reg:LDC的命令+寄存器地址。
*    	   data:将要向寄存器写入的数据
*返 回 值: 寄存器的状态
*备    注:
*****************************************************************/
static void SPI_LDC_WriteReg(u8 reg, u8 data)
{
    // u8 status;

//    LDC_CE_LOW();//进入待机模式1，低功耗模式，此模式下LDC可以接收数据
    LDC_CSN_LOW();//拉低csn片选信号，使能LDC的spi传输

    SPI_LDC_RW(reg);//向LDC发送命令和寄存器的号

    SPI_LDC_RW(data);//向刚才指定的寄存器写入数据

    LDC_CSN_HIGH();//拉高csn片选信号，即释放LDC的spi传输完毕；
}

/***********************读取LDC指定的寄存器值***********************
*功    能: 读取LDC指定的寄存器值
*形    参: reg:LDC寄存器地址。
*返 回 值: 状态寄存器的数据
*备    注:
*****************************************************************/
static u8 SPI_LDC_ReadReg(u8 reg)
{
    u8 Reg_Value;

//    LDC_CE_LOW();//进入待机模式1，低功耗模式，此模式下LDC可以接收数据
    LDC_CSN_LOW();//拉低csn片选信号，使能LDC的spi传输

    SPI_LDC_RW(reg | SPI_RWBIT); //选择寄存器
    Reg_Value = SPI_LDC_RW(SPI_NOP);

    LDC_CSN_HIGH();//拉高csn片选信号，即释放LDC的spi传输完毕；

    return Reg_Value;
}

/********************************************************************
 * @brief：		LDC1000初始化配置,ps:在SPI中配置了数据位16个数据长度，故
 * 				在发送数据时可以将地址和值进行或运算一起发送出去
 * @param:		none
 * @return:		none
 *********************************************************************/
void LDC1000_init(void)
{
//	SPI_Write(LDC1000_CMD_RPMAX<<8|0xff);
    SPI_LDC_WriteReg(LDC1000_CMD_RPMAX, 0x09);					//配置Rp_MAX(0x01)寄存器
    SPI_LDC_WriteReg(LDC1000_CMD_RPMIN, 0x37);					//配置Rp_MIN(0x02)寄存器
    SPI_LDC_WriteReg(LDC1000_CMD_SENSORFREQ, 0xE2);						//配置Sensor Frequency(0x03)寄存器
    SPI_LDC_WriteReg(LDC1000_CMD_LDCCONFIG, 0x17);							//配置LDC Configuration(0x04)寄存器
    SPI_LDC_WriteReg(LDC1000_CMD_CLKCONFIG, 0x00);							//配置Clock Configuration(0x05)寄存器,
    //使用TBCLK作为时钟源																		//配置INTB为比较输出标志位(status of Comparator output)
    SPI_LDC_WriteReg(LDC1000_CMD_THRESHILSB, 0x50);						//配置Comparator Threshold High(0x06)寄存器低8位
    SPI_LDC_WriteReg(LDC1000_CMD_THRESHIMSB, 0x14);						//配置Comparator Threshold High(0x07)寄存器高8位
    SPI_LDC_WriteReg(LDC1000_CMD_THRESLOLSB, 0xC0);						//配置Comparator Threshold Low(0x08)寄存器低8位
    SPI_LDC_WriteReg(LDC1000_CMD_THRESLOMSB, 0x12);						//配置Comparator Threshold Low(0x09)寄存器高8位
    SPI_LDC_WriteReg(LDC1000_CMD_INTCONFIG, 0x02);							//配置INTB Pin Configuration(0x0A),
    SPI_LDC_WriteReg(LDC1000_CMD_PWRCONFIG, 0x01);							//配置Power Configuration(0x0B)寄存器，																	//为Active Mode,使能转化
}

/********************************************************************
 * @brief：		使用SPI读取LDC1000中的数据
 * @param:		none
 * @return:		none
 *********************************************************************/
void LDCRead(void)
{
//	LDC_CSN_LOW();
    ProximityData = 0;
    FrequencyData = 0;
    while(LDC_Read_IRQ() != 0);
    DataRcv[0] = SPI_LDC_ReadReg(LDC1000_CMD_PROXLSB);          			//写入将要读取的Proximity Data LSB寄存器地址(0x21)
    //printf("0 %d\r\n",DataRcv[0]);
    //SPI_Read(&DataRcv[0]);                       			//读取上述寄存器中的值，并存入DataRcv[0]
    ProximityData |= DataRcv[0] ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[1] = SPI_LDC_ReadReg(LDC1000_CMD_PROXMSB);					//写入将要读取的Proximity Data MSB寄存器地址(0x22)
    //printf("1 %d\r\n",DataRcv[1]);
    //SPI_Read(&DataRcv[1]);									//读取上述寄存器中的值，并存入DataRcv[1]
    ProximityData |= (DataRcv[1] << 8) ;									//组合成ProximityData
    while(LDC_Read_IRQ() != 0);
    DataRcv[2] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRLSB);					//写入将要读取的Frequency Counter Data LSB寄存器地址(0x23)
    //printf("2 %d\r\n",DataRcv[2]);
    //SPI_Read(&DataRcv[2]);									//读取上述寄存器中的值，并存入DataRcv[2]
    FrequencyData |= DataRcv[2] ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[3] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRMID);					//写入将要读取的Frequency Counter Data Mid-Byte寄存器地址(0x24)
    //printf("3 %d\r\n",DataRcv[3]);
    //SPI_Read(&DataRcv[3]);									//读取上述寄存器中的值，并存入DataRcv[3]
    FrequencyData |= (DataRcv[3] << 8) ;
    while(LDC_Read_IRQ() != 0);
    DataRcv[4] = SPI_LDC_ReadReg(LDC1000_CMD_FREQCTRMSB);					//写入将要读取的Frequency Counter Data MSB寄存器地址(0x25)
    //printf("4 %d\r\n\n\n",DataRcv[4]);
    //SPI_Read(&DataRcv[4]);									//读取上述寄存器中的值，并存入DataRcv[4]
    FrequencyData |= (DataRcv[4] << 16) ;									//组合成FrequencyData
//	LDC_CSN_HIGH();
}

extern void SPI_LDC_Init(void)
{
    TIM3_Init();
    MY_SPI_Init();
    LDC1000_init();
}

/*
 * 函数名：TIM3_NVIC_Configuration
 * 描述  ：TIM3中断优先级配置
 * 输入  ：无
 * 输出  ：无
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void TIM3_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM信号电平跳变值 */
    u16 CCR3_Val = 2;

    /* -----------------------------------------------------------------------
    TIME3 可以输出 4 路 PWM 波形：
    TIME3_CH1	------	PA6
    TIME3_CH2	------	PA7
    TIME3_CH3	------	PB0
    TIME3_CH4	------	PB1
    ----------------------------------------------------------------------- */

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 3;       //当定时器从0计数到999，即为1000次，为一个定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：预分频72，即为1KHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分频系数：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    		/* 清除溢出中断标志 */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);	 //使能通道1

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);	 //使用CCR3寄存器的影子寄存器（直到产生更新事件才发生更改）

    //TIM_ARRPreloadConfig(TIM3, ENABLE);			 // 使能TIM3重载寄存器ARR

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);                   //使能定时器3
}


void TIM3_Init(void)
{
    //TIM3_NVIC_Configuration();
    TIM3_GPIO_Config();
    TIM3_Mode_Config();
}
