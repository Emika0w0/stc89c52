#include <reg51.h>           //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include <intrins.h>

#include <string.h>
#include <ctype.h>

#include "SoundPlay.h"
#include "music.h"

typedef unsigned int u16;     //���������ͽ�����������
typedef unsigned char u8;

#define leds P2



sbit k1 = P3 ^ 1;
sbit k2 = P3 ^ 0;
sbit k3 = P3 ^ 2;
sbit k4 = P3 ^ 3;

u8 key_id = 0;
u8 KeyValue = 0xFF;    //������Ŷ�ȡ���ļ�ֵ

#define GPIO_DIG P0
#define GPIO_KEY P1


// ��������·����
sbit beep = P1 ^ 5;
u16 x = 200 ;

// ����38��������Y0 ��·����a
sbit LSA = P2 ^ 2;
sbit LSB = P2 ^ 3;
sbit LSC = P2 ^ 4;


u8 code smgduan[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,  // 0 1 2 3 4 5 6 7 8 9
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3D, 0x76, 0x8F, 0x0E,  // A b c d E F G H I J
    0x75, 0x38, 0xB7, 0x54, 0x5C, 0x73, 0x67, 0x31, 0xC9, 0x78,  // K L M n o P q r S t
    0x3E, 0x1C, 0xFE, 0xE4, 0x6E, 0xDA, 0x40, 0x48, 0x80, 0x00   // U v W X Y Z - = . Null
};

sbit SRCLK = P3 ^ 6;
sbit RCLK = P3 ^ 5;   // ��������� reg52.h ͷ�ļ���ʹ�ã�����ʹ�� reg51.h
sbit SER = P3 ^ 4;

//    ���� "��" ��ģ
//    1 0 0 1 0 1 0 0
//    0 1 1 1 1 1 1 1
//    1 0 0 1 0 1 0 0
//    0 1 0 1 0 1 0 0
//    0 0 1 1 1 1 1 1
//    0 1 0 1 0 1 0 0
//    1 0 1 0 0 0 1 0
//    1 0 1 0 0 0 1 0
// ����-->�ң�ÿһ��ȡ�����ƻ���16����

u8 ledduan[] = {0xA3, 0x54, 0x4B, 0xFC, 0x48, 0xFC, 0x4B, 0x48};    // ���� �� ��ģ
u8 ledwei[] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};     // ���е�������



void Delay1ms(unsigned int count)
{
    unsigned int i, j;
    for (i = 0; i < count; i++)
        for (j = 0; j < 120; j++);
}

void delay(u16 i)
{
    while (i--);
}

// ɨ�� ���� K1-K4
void keypros();

// ɨ�� ������� S1-S16
void KeyDown(void);

void DigDisplay(char* str_7segment, u8 n);
void str2_7segment(char* s, u8 n);
void Hc595SendByte(u8 dat);

void test_matrix_key()
{
    LSA = LSB = LSC = 0;
    while (1) {
        KeyDown();         //�����жϺ���
        GPIO_DIG = smgduan[KeyValue]; //

        if (KeyValue == 15) {
            KeyValue = 0xFF;
            return ;
        }

    }
}

void hello_digdisplay()
{

    char* ps;
    u8 n = 0;
    u16 X = 400;
    u8 cnt = 0;
    char str[] = "Hello World! 0123456789AbcdEFGHIJKLMnoPqrStUvWXYZ-=.";

    n = strlen(str);

    str2_7segment(str, n);

    cnt = n - 8;
    ps = str;

    while (1) {

        while (X--) {
            DigDisplay(ps, 8);   //�������ʾ����
        }
        if (cnt--)
            ps++;
        else {
            cnt = n - 8;
            ps = str;
        }

        X = 200;

        // ����� S16 �����˳�����
        KeyDown();
        if (KeyValue == 15) {
            KeyValue = 0xFF;
            return ;
        }
    }

}

void test_8x8_LED()
{
    u8 i;
    while (1) {
        P0 = 0x7f;
        for (i = 0; i < 8; i++) {
            P0 = ledwei[i];       //λѡ
            Hc595SendByte(ledduan[i]);  //���Ͷ�ѡ����
            delay(100);        //��ʱ
            Hc595SendByte(0x00);  //����
        }

        // ����� S16 �����˳�����
        KeyDown();
        if (KeyValue == 15) {
            KeyValue = 0xFF;
            return ;
        }
    }
}

void main()
{
    u8 i = 0;
    InitialSound();

    while (1) {
        keypros();

        // K1 �������   ��ֻ����
        if (key_id == 1) {
            Play(Music_Two, 0, 3, 360);
            Delay1ms(500);

            key_id = 0;
        }

        // K2  Led �����  ��ʾ Hello World
        if (key_id == 2) {
            leds = 0xfe; // led:  1111 1110
            delay(50000);

            for (i = 0; i < 7; i++) { //��led����һλ
                leds = leds << 1;
                delay(50000);
            }

            leds = 0x7F; // led:  1111 1110
            delay(50000);

            for (i = 0; i < 7; i++) { //��led����һλ
                leds = leds >> 1;
                delay(50000);
            }

            for (i = 0 ; i != 3 ; i++) {
                leds = 0x0; // led: ȫ��
                delay(50000);
                leds = 0xff; // led: ȫϨ��
                delay(50000);
            }
            key_id = 0;

            hello_digdisplay();
        }


        // K3 �����  0-9 A-Z  ������ʾ  ���� beep ����
        if (key_id == 3) {

            LSA = 0;
            LSB = 0;
            LSC = 0; //����38��������Y0����͵�ƽ

            // ����� 0-9 A-Z ������ʾ
            for (i = 0 ; i != 40 ; i++) {
                P0 = smgduan[i];
                delay(55000);
                P0 = smgduan[39];
                delay(55000);
            }

            // ״̬��λ
            LSA = LSB = LSC = 1 ;
            P0 = 0xff;

            // beep ����
            x = 1000 ;
            while (x--) {
                beep = !beep;
                delay(100);
            }

            key_id = 0;

            test_8x8_LED();
        }

        // K4 ���� ɨ�������� S1-S16 ���Գ��� ���� beep ����
        if (key_id == 4) {

            test_matrix_key();

            x = 200 ;
            while (x--) {
                beep = !beep;
                delay(100);
            }
            key_id = 0;
        }


    }
}

// ɨ�� ���� K1-K4
void keypros()
{
    if (k1 == 0) {       // ���� K1 ;  key_id = 1
        delay(1000);
        if (k1 == 0) {
            key_id = 1;
        }
        while (!k1);
    }

    if (k2 == 0) {      // ���� K2 ;  key_id = 2
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
}

// ɨ�� ������� S1-S16
void KeyDown(void)
{
    char a = 0;
    GPIO_KEY = 0x0f;
    if (GPIO_KEY != 0x0f) {             //��ȡ�����Ƿ���
        delay(1000);                    //��ʱ10ms��������
        if (GPIO_KEY != 0x0f) {         //�ٴμ������Ƿ���
            //������
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
            //������
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
            while ((a < 50) && (GPIO_KEY != 0xf0)) { //��ⰴ�����ּ��
                delay(1000);
                a++;
            }
        }
    }
}

void DigDisplay(char* str_7segment, u8 n)
{
    u8 i;
    for (i = 0; i != n; i++) {
        switch (i) { //λѡ��ѡ�����������ܣ�
        case (7):
            LSA = 0;
            LSB = 0;
            LSC = 0;
            break;//��ʾ��0λ
        case (6):
            LSA = 1;
            LSB = 0;
            LSC = 0;
            break;//��ʾ��1λ
        case (5):
            LSA = 0;
            LSB = 1;
            LSC = 0;
            break;//��ʾ��2λ
        case (4):
            LSA = 1;
            LSB = 1;
            LSC = 0;
            break;//��ʾ��3λ
        case (3):
            LSA = 0;
            LSB = 0;
            LSC = 1;
            break;//��ʾ��4λ
        case (2):
            LSA = 1;
            LSB = 0;
            LSC = 1;
            break;//��ʾ��5λ
        case (1):
            LSA = 0;
            LSB = 1;
            LSC = 1;
            break;//��ʾ��6λ
        case (0):
            LSA = 1;
            LSB = 1;
            LSC = 1;
            break;//��ʾ��7λ
        }
        P0 = smgduan[*str_7segment]; //���Ͷ���
        delay(100); //���һ��ʱ��ɨ��
        P0 = 0x00; //����
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
            *s = 39;   // ���Ų���ʾ  Null

        if (isupper(*s))
            *s = tolower(*s);
        if (islower(*s))
            *s = *s - 'a' + 10 ;
        s++;
    }
}

/*******************************************************************************
* ������         : Hc595SendByte(u8 dat)
* ��������         : ��74HC595����һ���ֽڵ�����
    74595�Ŀ��ƶ�˵����
    SCK(11��)��������ʱ���ݼĴ�����������λ��QA-->QB-->QC-->...-->QH���½�����λ�Ĵ������ݲ��䡣
    RCK(12��)��������ʱ��λ�Ĵ��������ݽ������ݴ洢�Ĵ������½���ʱ�洢�Ĵ������ݲ��䡣
               ͨ���ҽ�RCK��Ϊ�͵�ƽ������λ��������RCK�˲���һ�������壬������ʾ���ݡ�
*******************************************************************************/
void Hc595SendByte(u8 dat)
{
    u8 a;
    SRCLK = 0;
    RCLK = 0;
    for (a = 0; a < 8; a++) {
        SER = dat >> 7;  // ��ȡ���λ  0xA3 :  1010 0011  ���� 1
        dat <<= 1;      //  ������λ���Ƴ����λ1��0  0100 0110

        SRCLK = 1;    // ������ʱ���ݼĴ�����������λ
        _nop_();
        _nop_();      // ��ʱ
        SRCLK = 0;    // �½�����λ�Ĵ������ݲ���
    }

    RCLK = 1;   // ������ʱ��λ�Ĵ��������ݽ������ݴ洢�Ĵ���
    _nop_();
    _nop_();    // ��ʱ
    RCLK = 0;   // �½���ʱ�洢�Ĵ������ݲ���
}
