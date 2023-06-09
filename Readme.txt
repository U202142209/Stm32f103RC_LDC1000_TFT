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
*相关说明：该程序实现了STM32F103单片机使用硬件SPI对LDC1000的控制
*注意事项：用户在使用该库的时候必须先调用初始化SPI_LDC_Init（）函数，对LDC1000
*		   进行配置，然后可以调用LDC_Check（），进行连接监测，正常返回SUCESS,失败
*			返回ERROR,进行这样的操作后，用户可调用USER_LDC_MODE_TX（）或USER_LDC_MODE_RX。
*			进行读写配置，然后需调用LDC_TX_DATA（）或LDC_RX_DATA（）发送或接收数据
*
*接口定义：SCK，MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7（不可移植）
		CSN -- A1 ,CE --A2，IQ--A3，(这三个可移植)
		LDC10xx EVM模块接线：
		+5V接DC-5V电源；
		GND接DC-地；
		ECK默认不接，板载8M有源晶振时基时钟,如需使用其他外部时钟，请去掉R3；
		INTB接单片机PA3；
		SCK接单片机PA5；
		CSB接单片机PA1；
		SDO接单片机PA7；
		SDI接单片机PA6；
		VIO接SPI通信电平电源，例如5V单片机接+5V，3.3V单片机接+3.3V；
		GND接单片机地。

		串口接线：
		波特率：115200 停止位1，无校验 数据位8
		TXD接单片机PA10；
		RXD接单片机PA9;


请在上电之前，检查好接线是否正确。
*
******************************************************************************/
