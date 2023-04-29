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
*相关说明：该程序实现了STM32单片对LDC24L01的一系列操作
*注意事项：用户在使用该库的时候必须先调用初始化SPI_LDC_Init（）函数，对LDC24L01
*		   进行配置，然后可以调用LDC_Check（），进行连接监测，正常返回SUCESS,失败
*			返回ERROR,进行这样的操作后，用户可调用USER_LDC_MODE_TX（）或USER_LDC_MODE_RX。
*			进行读写配置，然后需调用LDC_TX_DATA（）或LDC_RX_DATA（）发送或接收数据
*
*接口定义：SCK，MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7（不可移植）
		CSN -- A1 ,CE --A2，IQ--A3，(这三个可移植)
*
******************************************************************************/
#ifndef _LDC24L01_H_
#define _LDC24L01_H_

#include <stm32f10x.h>

/****************代码移植区*******************/
/*
*由于硬件连接stm32的SPI1的SCK，MISO,MOSI引脚，GPIOA^5,GPIOA^6,GPIOA^7
*无法更改所以不留移植
*/
#define GPIO_LDC GPIOA
#define GPIO_LDC_CSN_Pin GPIO_Pin_1
#define GPIO_LDC_CE_Pin  GPIO_Pin_2
#define GPIO_LDC_IQ_Pin  GPIO_Pin_3

#define SPI_NOP             0XFF//空指令用来读状态寄存器

/****************寄存器地址区*******************/
#define LDC1000_CMD_REVID 		0x00
#define LDC1000_CMD_RPMAX 		0x01
#define LDC1000_CMD_RPMIN 		0x02
#define LDC1000_CMD_SENSORFREQ 	0x03
#define LDC1000_CMD_LDCCONFIG 	0x04
#define LDC1000_CMD_CLKCONFIG 	0x05
#define LDC1000_CMD_THRESHILSB 	0x06
#define LDC1000_CMD_THRESHIMSB 	0x07
#define LDC1000_CMD_THRESLOLSB 	0x08
#define LDC1000_CMD_THRESLOMSB 	0x09
#define LDC1000_CMD_INTCONFIG 	0x0A
#define LDC1000_CMD_PWRCONFIG 	0x0B
#define LDC1000_CMD_STATUS	 	0x20
#define LDC1000_CMD_PROXLSB 	0x21
#define LDC1000_CMD_PROXMSB 	0x22
#define LDC1000_CMD_FREQCTRLSB	0x23
#define LDC1000_CMD_FREQCTRMID	0x24
#define LDC1000_CMD_FREQCTRMSB	0x25

// LDC BITMASKS
#define LDC1000_BIT_AMPLITUDE    0x18
#define LDC1000_BIT_RESPTIME     0x07
#define LDC1000_BIT_CLKSEL       0x02
#define LDC1000_BIT_CLKPD        0x01
#define LDC1000_BIT_INTMODE      0x07
#define LDC1000_BIT_PWRMODE      0x01
#define LDC1000_BIT_STATUSOSC    0x80
#define LDC1000_BIT_STATUSDRDYB  0x40
#define LDC1000_BIT_STATUSWAKEUP 0x20
#define LDC1000_BIT_STATUSCOMP   0x10

#define TEST_RPMAX_MAX 	0x13			  /**< maximum calibration value for RPMAX */
#define TEST_RPMAX_MIN 	0x10			  /**< minimum calibration value for RPMAX */
#define TEST_RPMAX_INIT TEST_RPMAX_MIN+1  /**< RPMAX initial value */
#define TEST_RPMIN_MAX 	0x3D   			  /**< maximum calibration value for RPMIN */
#define TEST_RPMIN_MIN 	0x3A    		  /**< minimum calibration value for RPMIN */
#define TEST_RPMIN_INIT TEST_RPMIN_MIN+1  /**< RPMIN initial value */
// Final Test Range
#define TEST_RP_MSB_MAX 0x12	/**< maximum value for proximity data */
#define TEST_RP_MSB_MIN 0x0A	/**< minimum value for proximity data */
#define TEST_FC_MAX 0x0D5D		/**< maximum value for frequency counter */
#define TEST_FC_MIN 0x0D39		/**< minimum value for frequency counter */


/****************宏定义区*******************/
#define LDC_CSN_HIGH()     GPIO_SetBits(GPIO_LDC, GPIO_LDC_CSN_Pin)
#define LDC_CSN_LOW()      GPIO_ResetBits(GPIO_LDC, GPIO_LDC_CSN_Pin)		 //csn置低
#define LDC_Read_IRQ()		GPIO_ReadInputDataBit (GPIO_LDC, GPIO_LDC_IQ_Pin) //中断引脚


/****************函数声明区*******************/
extern void SPI_LDC_Init(void);//spi的初始化
u8 SPI_LDC_RW(u8 data);
static void SPI_LDC_WriteReg(u8 reg,u8 data);//向reg寄存器中写入data
static u8 SPI_LDC_ReadReg(u8 reg);//读取指定状态寄存器的值
void LDC1000_init(void);
void LDCRead(void);
extern void MY_SPI_Init(void);
void TIM3_NVIC_Configuration(void);
static void TIM3_GPIO_Config(void);
static void TIM3_Mode_Config(void);
void TIM3_Init(void);

#endif
