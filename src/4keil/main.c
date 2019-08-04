#include <reg52.h>           //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���

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
    0x3f, 0x06, 0x5b, 0x4f, 0x66,   // 0 1 2 3 4
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,   // 5 6 7 8 9
    0x77, 0x7c, 0x39, 0x5e, 0x79,   // A B C D E
    0x71, 0x00                      // F        NULL
};



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

        // K2  Led �����
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
        }


        // K3 ����� 0-F ������ʾ  ���� beep ����
        if (key_id == 3) {
            LSA = 0;
            LSB = 0;
            LSC = 0; //����38��������Y0����͵�ƽ

            // ����� 0-F ������ʾ
            for (i = 0 ; i != 17 ; i++) {
                P0 = smgduan[i];
                delay(55000);
                P0 = smgduan[16];
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
