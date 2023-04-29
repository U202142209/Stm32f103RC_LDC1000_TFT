/* --------------------------Includes ---------------------------------------------*/
#include "stm32f10x.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
#include "Picture.h"
#include "QDTFT_demo.h"
/* ----------------------End of Includes ---------------------------------------------*/

unsigned char Num[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//绘制测试菜单
//2D按键按钮示例
void Redraw_Mainmenu(void)
{

    Lcd_Clear(GRAY0);

    Gui_DrawFont_GBK16(16, 2, BLUE, GRAY0, "全动电子技术");
    Gui_DrawFont_GBK16(16, 20, RED, GRAY0, "液晶测试程序");

    DisplayButtonUp(15, 38, 113, 58); //x1,y1,x2,y2
    Gui_DrawFont_GBK16(16, 40, GREEN, GRAY0, "颜色填充测试");

    DisplayButtonUp(15, 68, 113, 88); //x1,y1,x2,y2
    Gui_DrawFont_GBK16(16, 70, BLUE, GRAY0, "文字显示测试");

    DisplayButtonUp(15, 98, 113, 118); //x1,y1,x2,y2
    Gui_DrawFont_GBK16(16, 100, RED, GRAY0, "图片显示测试");

    //Gui_DrawFont_GBK16(16,120,BLUE,GRAY0,"Welcome");
    Gui_DrawFont_GBK16(16, 140, RED, GRAY0, "Welcome");

    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[5]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[4]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[3]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[2]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[1]);
    delay_ms(1000);
    Gui_DrawFont_Num32(100, 125, RED, GRAY0, Num[0]);
}
//测试数码管字体
void Num_Test(void)
{
    u8 i = 0;
    Lcd_Clear(GRAY0);
    Gui_DrawFont_GBK16(16, 20, RED, GRAY0, "Num Test");
    delay_ms(1000);
    Lcd_Clear(GRAY0);

    for(i = 0; i < 10; i++)
    {
        Gui_DrawFont_Num32((i % 3) * 40, 32 * (i / 3) + 30, RED, GRAY0, Num[i + 1]);
        delay_ms(100);
    }

}
//中英文显示测试
void Font_Test(void)
{
    Lcd_Clear(GRAY0);
    Gui_DrawFont_GBK16(16, 10, BLUE, GRAY0, "文字显示测试");

    delay_ms(1000);
    Lcd_Clear(GRAY0);
    Gui_DrawFont_GBK16(16, 30, RED, GRAY0, "全动电子技术");
    Gui_DrawFont_GBK16(16, 50, BLUE, GRAY0, "专注液晶批发");
    Gui_DrawFont_GBK16(16, 70, RED, GRAY0, "全程技术支持");
    Gui_DrawFont_GBK16(0, 100, BLUE, GRAY0, "Tel:15989313508");
    Gui_DrawFont_GBK16(0, 130, RED, GRAY0, "www.qdtech.net");
    delay_ms(1500);
}
//简单刷屏测试
void Color_Test(void)
{
    u8 i = 1;
    Lcd_Clear(GRAY0);

    Gui_DrawFont_GBK16(20, 10, BLUE, GRAY0, "颜色填充测试");
    delay_ms(1200);

    while(i--)
    {
        Lcd_Clear(WHITE);
        delay_ms(500);
        Lcd_Clear(BLACK);
        delay_ms(500);
        Lcd_Clear(RED);
        delay_ms(500);
        Lcd_Clear(GREEN);
        delay_ms(500);
        Lcd_Clear(BLUE);
        delay_ms(500);
    }
}

//文字显示测试
//16位BMP 40X40 QQ图像取模数据
//Image2LCD取模选项设置
//水平扫描
//16位
//40X40
//不包含图像头数据
//自左至右
//自顶至底
//低位在前
void showimage(const unsigned char* p) //显示40*40 QQ图片
{
    int i, j, k;
    unsigned char picH, picL;
    Lcd_Clear(GRAY0);
    Gui_DrawFont_GBK16(16, 10, BLUE, GRAY0, "图片显示测试");
    delay_ms(1000);

    Lcd_Clear(GRAY0);
    for(k = 0; k < Y_MAX_PIXEL / 40; k++)
    {
        for(j = 0; j < X_MAX_PIXEL / 40; j++)
        {
            Lcd_SetRegion(40 * j, 40 * k, 40 * j + 39, 40 * k + 39);		//坐标设置
            for(i = 0; i < 40 * 40; i++)
            {
                picL = *(p + i * 2);	//数据低位在前
                picH = *(p + i * 2 + 1);
                Lcd_WriteData_16Bit(picH << 8 | picL);
            }
        }
    }
}
//综合测试函数
void QDTFT_Test_Demo(void)
{
    Lcd_Init();
    LCD_LED_SET;				//通过IO控制背光亮
    Redraw_Mainmenu();	//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
    Color_Test();				//简单纯色填充测试
    Num_Test();					//数码管字体测试
    Font_Test();				//中英文显示测试
    showimage(gImage_qq);//图片显示示例
    delay_ms(1500);
    LCD_LED_CLR;//IO控制背光灭
}

// 在指定的y轴位置逆序输出long整数值
void showLongDigit(u16 y, unsigned long n)
{

    u8 i = 0;
    while(n != 0)
    {
        Gui_DrawFont_Num32(i * 25, y, RED, GRAY0, Num[n % 10]);
        n = n / 10;
        i++;
    }
}


// 程序入口
void selftest()
{
	  SystemInit();	//System init.
		delay_init(72);//Delay init.
    Lcd_Init();
    LCD_LED_SET;				//通过IO控制背光亮
		// 自定义程序入口代码
    Lcd_Clear(GRAY0);
    while(1)
    {
        Gui_DrawFont_GBK16(16, 10, BLUE, GRAY0, "硬币盲盒检测实验");
        Gui_DrawFont_GBK16(16, 30, RED, GRAY0, "醉星辰");
        Gui_DrawFont_GBK16(16, 60, BLUE, GRAY0, "无盲盒 / 检测到盲盒");
        Gui_DrawFont_GBK16(16, 80, RED, GRAY0, "识别中...");
        Gui_DrawFont_GBK16(16, 100, BLUE, GRAY0, "识别成功");
        Gui_DrawFont_GBK16(16, 120, RED, GRAY0, "检测结果:");
        Gui_DrawFont_GBK16(16, 150, RED, GRAY0, "一元硬币在上,五角硬币在下");
        // delay_ms(3500);
        // 数字测试
        showLongDigit(180, 4561531);
        showLongDigit(220, 902241202);
    }

    // LCD_LED_CLR;//IO控制背光灭
}
