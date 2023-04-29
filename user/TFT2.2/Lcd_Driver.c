#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "delay.h"


/***************************************************************************************
STM32测试平台介绍:
开发板：正点原子MiniSTM32开发板
MCU ：STM32_F103_RBT6
晶振 ：12MHZ
主频 ：72MHZ
接线说明:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10连接至TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 连接至TFT --CS
#define LCD_RST     	  GPIO_Pin_12	//PB12连接至TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13连接至TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15连接至TFT - SDI
//VCC:可以接5V也可以接3.3V
//LED:可以接5V也可以接3.3V或者使用任意空闲IO控制(高电平使能)
//GND：接电源地
//说明：如需要尽可能少占用IO，可以将LCD_CS接地，LCD_LED接3.3V，LCD_RST接至单片机复位端，
//将可以释放3个可用IO
//接口定义在Lcd_Driver.h内定义，
//如需变更IO接法，请根据您的实际接线修改相应IO初始化LCD_GPIO_Init()
//-----------------------------------------------------------------------------------------
例程功能说明：
1.	简单刷屏测试
2.	英文显示测试示例
3.	中文显示测试示例
4.	数码管字体显示示例
5.	图片显示示例
6.	2D按键菜单示例
7.	本例程支持横屏/竖屏切换(开启宏USE_HORIZONTAL,详见Lcd_Driver.h)
8.	本例程支持软件模拟SPI/硬件SPI切换(开启宏USE_HARDWARE_SPI,详见Lcd_Driver.h)
**********************************************************************************************/


//---------------------------------function----------------------------------------------------//

/****************************************************************************
* 名    称：void LCD_GPIO_Init(void)
* 功    能：STM32_模拟SPI所用到的GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：初始化模拟SPI所用的GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}
/****************************************************************************
* 名    称：void  SPIv_WriteData(u8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(u8 Data)
{
    unsigned char i = 0;
    for(i = 8; i > 0; i--)
    {
        if(Data & 0x80)
            LCD_SDA_SET; //输出数据
        else LCD_SDA_CLR;

        LCD_SCL_CLR;
        LCD_SCL_SET;
        Data <<= 1;
    }
}

/****************************************************************************
* 名    称：u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx, u8 Byte)
{
    while((SPIx->SR & SPI_I2S_FLAG_TXE) == RESET);		//等待发送区空
    SPIx->DR = Byte;	 	//发送一个byte
    while((SPIx->SR & SPI_I2S_FLAG_RXNE) == RESET); //等待接收完一个byte
    return SPIx->DR;          	     //返回收到的数据
}

/****************************************************************************
* 名    称：void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
* 功    能：设置SPI的速度
* 入口参数：SPIx,SpeedSet
* 出口参数：无
* 说    明：SpeedSet:1,高速;0,低速;
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx, u8 SpeedSet)
{
    SPIx->CR1 &= 0XFFC7;
    if(SpeedSet == 1) //高速
    {
        SPIx->CR1 |= SPI_BaudRatePrescaler_2; //Fsck=Fpclk/2
    }
    else//低速
    {
        SPIx->CR1 |= SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
    }
    SPIx->CR1 |= 1 << 6; //SPI设备使能
}

/****************************************************************************
* 名    称：SPI2_Init(void)
* 功    能：STM32_SPI2硬件配置初始化
* 入口参数：无
* 出口参数：无
* 说    明：STM32_SPI2硬件配置初始化
****************************************************************************/
void SPI2_Init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    //配置SPI2管脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //SPI2配置选项
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    //使能SPI2
    SPI_Cmd(SPI2, ENABLE);
}

/****************************************************************************
* 名    称：Lcd_WriteIndex(u8 Index)
* 功    能：向液晶屏写一个8位指令
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
    LCD_CS_CLR;
    LCD_RS_CLR;
#if USE_HARDWARE_SPI
    SPI_WriteByte(SPI2, Index);
#else
    SPIv_WriteData(Index);
#endif
    LCD_CS_SET;
}

/****************************************************************************
* 名    称：Lcd_WriteData(u8 Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
#if USE_HARDWARE_SPI
    SPI_WriteByte(SPI2, Data);
#else
    SPIv_WriteData(Data);
#endif
    LCD_CS_SET;
}

/****************************************************************************
* 名    称：void LCD_WriteReg(u8 Index,u16 Data)
* 功    能：写寄存器数据
* 入口参数：Index,Data
* 出口参数：无
* 说    明：本函数为组合函数，向Index地址的寄存器写入Data值
****************************************************************************/
void LCD_WriteReg(u8 Index, u16 Data)
{
    Lcd_WriteIndex(Index);
    Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(u16 Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{
    Lcd_WriteData(Data >> 8);
    Lcd_WriteData(Data);
}

/****************************************************************************
* 名    称：void Lcd_Reset(void)
* 功    能：液晶硬复位函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化前需执行一次复位操作
****************************************************************************/
void Lcd_Reset(void)
{
    LCD_RST_CLR;
    delay_ms(100);
    LCD_RST_SET;
    delay_ms(50);
}
/****************************************************************************
* 名    称：void Lcd_Init(void)
* 功    能：液晶初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{
#if USE_HARDWARE_SPI //使用硬件SPI
    SPI2_Init();
#else
    LCD_GPIO_Init();//使用模拟SPI
#endif
    Lcd_Reset(); //Reset before LCD Init.

    //2.2inch TM2.2-G2.2 Init 20171020
    Lcd_WriteIndex(0x11);
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0xCF);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0XC1);
    Lcd_WriteData(0X30);

    Lcd_WriteIndex(0xED);
    Lcd_WriteData(0X64);
    Lcd_WriteData(0X03);
    Lcd_WriteData(0X12);
    Lcd_WriteData(0X81);

    Lcd_WriteIndex(0xE8);
    Lcd_WriteData(0X85);
    Lcd_WriteData(0X11);
    Lcd_WriteData(0X78);

    Lcd_WriteIndex(0xF6);
    Lcd_WriteData(0X01);
    Lcd_WriteData(0X30);
    Lcd_WriteData(0X00);

    Lcd_WriteIndex(0xCB);
    Lcd_WriteData(0X39);
    Lcd_WriteData(0X2C);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0X34);
    Lcd_WriteData(0X05);

    Lcd_WriteIndex(0xF7);
    Lcd_WriteData(0X20);

    Lcd_WriteIndex(0xEA);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0X00);

    Lcd_WriteIndex(0xC0);
    Lcd_WriteData(0X20);

    Lcd_WriteIndex(0xC1);
    Lcd_WriteData(0X11);

    Lcd_WriteIndex(0xC5);
    Lcd_WriteData(0X31);
    Lcd_WriteData(0X3C);

    Lcd_WriteIndex(0xC7);
    Lcd_WriteData(0XA9);

    Lcd_WriteIndex(0x3A);
    Lcd_WriteData(0X55);

    Lcd_WriteIndex(0x36);
#if USE_HORIZONTAL
    Lcd_WriteData(0xE8);//横屏参数
#else
    Lcd_WriteData(0x48);//竖屏参数
#endif

    Lcd_WriteIndex(0xB1);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0X18);

    Lcd_WriteIndex(0xB4);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0X00);

    Lcd_WriteIndex(0xF2);
    Lcd_WriteData(0X00);

    Lcd_WriteIndex(0x26);
    Lcd_WriteData(0X01);

    Lcd_WriteIndex(0xE0);
    Lcd_WriteData(0X0F);
    Lcd_WriteData(0X17);
    Lcd_WriteData(0X14);
    Lcd_WriteData(0X09);
    Lcd_WriteData(0X0C);
    Lcd_WriteData(0X06);
    Lcd_WriteData(0X43);
    Lcd_WriteData(0X75);
    Lcd_WriteData(0X36);
    Lcd_WriteData(0X08);
    Lcd_WriteData(0X13);
    Lcd_WriteData(0X05);
    Lcd_WriteData(0X10);
    Lcd_WriteData(0X0B);
    Lcd_WriteData(0X08);


    Lcd_WriteIndex(0xE1);
    Lcd_WriteData(0X00);
    Lcd_WriteData(0X1F);
    Lcd_WriteData(0X23);
    Lcd_WriteData(0X03);
    Lcd_WriteData(0X0E);
    Lcd_WriteData(0X04);
    Lcd_WriteData(0X39);
    Lcd_WriteData(0X25);
    Lcd_WriteData(0X4D);
    Lcd_WriteData(0X06);
    Lcd_WriteData(0X0D);
    Lcd_WriteData(0X0B);
    Lcd_WriteData(0X33);
    Lcd_WriteData(0X37);
    Lcd_WriteData(0X0F);

    Lcd_WriteIndex(0x29);
}



/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{
    Lcd_WriteIndex(0x2A);
    Lcd_WriteData_16Bit(Xpos);
    Lcd_WriteIndex(0x2B);
    Lcd_WriteData_16Bit(Ypos);
    Lcd_WriteIndex(0x2c);
}
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd)
{
    Lcd_WriteIndex(0x2A);
    Lcd_WriteData_16Bit(xStar);
    Lcd_WriteData_16Bit(xEnd);
    Lcd_WriteIndex(0x2B);
    Lcd_WriteData_16Bit(yStar);
    Lcd_WriteData_16Bit(yEnd);
    Lcd_WriteIndex(0x2c);
}


/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x, u16 y, u16 Data)
{
    Lcd_SetXY(x, y);
    Lcd_WriteData_16Bit(Data);

}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(u16 Color)
{
    unsigned int i;
    Lcd_SetRegion(0, 0, X_MAX_PIXEL - 1, Y_MAX_PIXEL - 1);
    LCD_CS_CLR;
    LCD_RS_SET;
    for(i = 0; i < X_MAX_PIXEL * Y_MAX_PIXEL; i++)
    {
        //Lcd_WriteData_16Bit(Color);
#if USE_HARDWARE_SPI
        SPI_WriteByte(SPI2, Color >> 8);
        SPI_WriteByte(SPI2, Color);
#else
        SPIv_WriteData(Color >> 8);
        SPIv_WriteData(Color);
#endif
    }
    LCD_CS_SET;
}

