#include <reg51.h>           //此文件中定义了单片机的一些特殊功能寄存器
#include <intrins.h>
#include <string.h>
#include <ctype.h>
#include "hc6800v2.h"
#include "lcd1602a.h"


u8 key_id = 0;
u8 KeyValue = 0xFF;    //用来存放读取到的键值

void test_ds1302_main();
void test_ds1302_calendar();

void test_leds()
{
    u8 i;
    u8 led = 0x1;

    // D1 - D8  LED流水灯
    for (i = 0; i < 9; i++) {
        P2 = ~led;
        delay(55000);
        led = led << 1;
    }
    led = 0x1;

    // D9 - D16  LED流水灯
    P0 = 0x7f; // 8x8 LED
    for (i = 0; i < 9; i++) {
        Hc595SendByte(~led);
        delay(55000);        //延时
        led = led << 1;
    }
    led = 0x1;

    P2 = 0xFF;
}

void test_matrix_key()
{
    LSA = LSB = LSC = 0;
    while (1) {
        KeyDown();         //按键判断函数
        GPIO_DIG = smgduan[KeyValue]; //

        if (keypros(0) == 4)
            return ; // K4 键退出
    }
}

void hello_digdisplay()
{
    char* ps;
    u8 n = 0;
    u16 X = 400;
    u8 cnt = 0;
    char str[] = "Hello World I love ZheJiang";

    n = strlen(str);

    str2_7segment(str, n);

    cnt = n - 8;
    ps = str;

    while (1) {
        if (keypros(0) == 4)
            return ; // K4 键退出

        while (X--) {
            DigDisplay(ps, 8);   //数码管显示函数
        }
        if (cnt--)
            ps++;
        else {
            cnt = n - 8;
            ps = str;
        }

        X = 200;
    }
}


void test_8x8_LED()
{
    u8 i;
    while (1) {
        if (keypros(0) == 4)
            return ; // K4 键退出

        P0 = 0x7f;
        for (i = 0; i < 8; i++) {
            P0 = ledwei[i];       //位选
            Hc595SendByte(ledduan[i]);  //发送段选数据
            delay(100);        //延时
            Hc595SendByte(0x00);  //消隐
        }

    }
}

void test_smgduan()
{
    u8 i;
    LSA = 0;
    LSB = 0;
    LSC = 0; //控制38译码器的Y0输出低电平

    // 数码管 0-9 A-Z 计数显示
    for (i = 0 ; i != 40 ; i++) {
        if (keypros(0) == 4)
            return ; // K4 键退出

        P0 = smgduan[i];
        delay(55000);
        P0 = smgduan[39];
        delay(55000);
    }

    // 状态复位
    LSA = LSB = LSC = 1 ;
    P0 = 0xff;

}

void test_lcd1602a(void)
{
    uchar code tabel1[] = "Hello World!";
    uchar code tabel2[] = "I love ZheJiang";

    uchar* str1 = tabel1;
    uchar* str2 = tabel2;

    Lcd_Init();       // 第一步：初始化  (单片机先跟1602打个招呼)

    Lcd_Show(2, 0);   // 第二步：确定显示位置  (单片机告诉1602在哪里显示)

    while (*str1 != '\0') {
        LcdWrDat(*str1++);   // 第三部：确定显示内容  (单片机告诉1602显示什么内容)
    }

    // 显示坐标 0 行首，1 第2行
    Lcd_Show(0, 1);
    while (*str2 != '\0') {
        LcdWrDat(*str2++);
    }

}

void main()
{
    InitialSound();

    while (1) {
        keypros(0);

        // K1 按键点歌   LCD1602 测试 和  两只蝴蝶
        if (key_id == 1) {
            test_lcd1602a();

            Play(Music_Two, 0, 3, 360);
            Delay1ms(500);
        }

        // LCD1602 安装在版子上，会和数码管显示冲突
        // K2  Led 跑马灯  显示 Hello World
        if (key_id == 2) {

            hello_digdisplay();

            test_ds1302_main();

            test_leds();  // 跑马灯 P2 I/O 同 LCD1602  EN 端冲突，交换次序
            // 跑马灯 演示后，LCD1602 和 数码管会不正常
        }

        // K3 数码管  0-9 A-Z  计数显示  结束 beep 长声
        if (key_id == 3) {

            test_smgduan();

            // beep 长声
            beep_ring(1000);

            test_matrix_key();

            test_8x8_LED();

            test_ds1302_calendar();
        }

        // K4 返回当前程序
        if (key_id == 4) {
            beep_ring(200);
        }

    }
}

// 扫描 按键 K1-K4
u8 keypros(u8 n)
{
    key_id = n;

    if (k1 == 0) {       // 按键 K1 ;  key_id = 1
        delay(1000);
        if (k1 == 0) {
            key_id = 1;
        }
        while (!k1);
    }

    if (k2 == 0) {      // 按键 K2 ;  key_id = 2
        delay(1000);
        if (k2 == 0) {
            key_id = 2;
        }
        while (!k2);
    }

    if (k3 == 0) {
        delay(1000);
        if (k3 == 0) {
            key_id = 3;
        }
        while (!k3);
    }

    if (k4 == 0) {
        delay(1000);
        if (k4 == 0) {
            key_id = 4;
        }
        while (!k4);
    }

    return key_id;
}

// 扫描 矩阵键盘 S1-S16
u8 KeyDown(void)
{
    char a = 0;

    GPIO_KEY = 0x0f;
    if (GPIO_KEY != 0x0f) {             //读取按键是否按下
        delay(1000);                    //延时10ms进行消抖
        if (GPIO_KEY != 0x0f) {         //再次检测键盘是否按下
            //测试列
            GPIO_KEY = 0X0F;
            switch (GPIO_KEY) {
            case (0X07):
                KeyValue = 0;
                break;
            case (0X0b):
                KeyValue = 1;
                break;
            case (0X0d):
                KeyValue = 2;
                break;
            case (0X0e):
                KeyValue = 3;
                break;
            }
            //测试行
            GPIO_KEY = 0XF0;
            switch (GPIO_KEY) {
            case (0X70):
                KeyValue = KeyValue;
                break;
            case (0Xb0):
                KeyValue = KeyValue + 4;
                break;
            case (0Xd0):
                KeyValue = KeyValue + 8;
                break;
            case (0Xe0):
                KeyValue = KeyValue + 12;
                break;
            }
            while ((a < 50) && (GPIO_KEY != 0xf0)) { //检测按键松手检测
                delay(1000);
                a++;
            }
        }
    }

    return KeyValue;
}

void DigDisplay(char* str_7segment, u8 n)
{
    u8 i;
    for (i = 0; i != n; i++) {
        switch (i) { //位选，选择点亮的数码管，
        case (7):
            LSA = 0;
            LSB = 0;
            LSC = 0;
            break;//显示第0位
        case (6):
            LSA = 1;
            LSB = 0;
            LSC = 0;
            break;//显示第1位
        case (5):
            LSA = 0;
            LSB = 1;
            LSC = 0;
            break;//显示第2位
        case (4):
            LSA = 1;
            LSB = 1;
            LSC = 0;
            break;//显示第3位
        case (3):
            LSA = 0;
            LSB = 0;
            LSC = 1;
            break;//显示第4位
        case (2):
            LSA = 1;
            LSB = 0;
            LSC = 1;
            break;//显示第5位
        case (1):
            LSA = 0;
            LSB = 1;
            LSC = 1;
            break;//显示第6位
        case (0):
            LSA = 1;
            LSB = 1;
            LSC = 1;
            break;//显示第7位
        }
        P0 = smgduan[*str_7segment]; //发送段码
        delay(100); //间隔一段时间扫描
        P0 = 0x00; //消隐
        str_7segment++;
    }
}

void str2_7segment(char* s, u8 n)
{
    u8 i;
    for (i = 0 ; i != n; ++i) {
        if (isdigit(*s))
            *s -= '0' ;

        if (*s > 0x1F  && *s < 0x40)
            *s = 39;   // 符号不显示  Null

        if (isupper(*s))
            *s = tolower(*s);
        if (islower(*s))
            *s = *s - 'a' + 10 ;
        s++;
    }
}

/*******************************************************************************
* 函数名         : Hc595SendByte(u8 dat)
* 函数功能         : 向74HC595发送一个字节的数据
    74595的控制端说明：
    SCK(11脚)：上升沿时数据寄存器的数据移位。QA-->QB-->QC-->...-->QH；下降沿移位寄存器数据不变。
    RCK(12脚)：上升沿时移位寄存器的数据进入数据存储寄存器，下降沿时存储寄存器数据不变。
               通常我将RCK置为低点平，当移位结束后，在RCK端产生一个正脉冲，更新显示数据。
*******************************************************************************/
void Hc595SendByte(u8 dat)
{
    u8 a;
    SRCLK = 0;
    RCLK = 0;
    for (a = 0; a < 8; a++) {
        SER = dat >> 7;  // 获取最高位  0xA3 :  1010 0011  等于 1
        dat <<= 1;      //  数据移位，移除最高位1补0  0100 0110

        SRCLK = 1;    // 上升沿时数据寄存器的数据移位
        _nop_();
        _nop_();      // 延时
        SRCLK = 0;    // 下降沿移位寄存器数据不变
    }

    RCLK = 1;   // 上升沿时移位寄存器的数据进入数据存储寄存器
    _nop_();
    _nop_();    // 延时
    RCLK = 0;   // 下降沿时存储寄存器数据不变
}
