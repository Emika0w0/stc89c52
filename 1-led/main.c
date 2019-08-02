// LED��˸ʵ�� �� �����

#include <reg52.h>           //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include <intrins.h>        //��ΪҪ�õ������ƺ��������Լ������ͷ�ļ�

#define leds P2

typedef unsigned int u16;     //���������ͽ�����������
typedef unsigned char u8;

sbit led = P2 ^ 0;   //����Ƭ����P2.0�˿ڶ���Ϊled
sbit led3 = P2 ^ 2;
sbit led5 = P2 ^ 4;
sbit led7 = P2 ^ 6;


void delay(u16 i);
void wait(u8 i);
void led_work(u8 i);


int main()
{
    u8 i = 0;

    while (1) {

        for (i = 0; i != 5; i++) {
            led_work(i % 2);
            wait(1); //��Լ s
        }


        leds = 0xfe; // led:  1111 1110
        wait(1); //��Լ 1s

        for (i = 0; i < 7; i++) { //��led����һλ
            leds = _crol_(leds, 1);
            delay(50000);
        }
        for (i = 0; i < 7; i++) { //��led����һλ
            leds = _cror_(leds, 1);
            delay(50000);
        }

    }

    return 0;
}


void delay(u16 i)
{
    while (i--);
}

void wait(u8 i)
{
    while (i--) {
        delay(50000); //��Լ��ʱ450ms
        delay(5000); //��Լ��ʱ45ms
        delay(50000); //��Լ��ʱ450ms
        delay(5000); //��Լ��ʱ45ms
    }
}

void led_work(u8 i)
{
    led = i;
    led3 = i;
    led5 = i;
    led7 = i;
}
