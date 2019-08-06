/**************************************************************************************
*                     ��̬�������ʾʵ��                                                 *
ʵ���������س��������ܾ�̬��ʾ0��
ע�������������õ���ģ����ʾ�����Խ�74HC595ģ���ϵ�JP595�̽�Ƭ�ε���
***************************************************************************************/

#include <reg52.h>           //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���

typedef unsigned int u16;     //���������ͽ�����������
typedef unsigned char u8;

sbit LSA = P2 ^ 2;
sbit LSB = P2 ^ 3;
sbit LSC = P2 ^ 4;

u8 code smgduan[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,  // 0 1 2 3 4 5 6 7 8 9
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3D, 0x76, 0x8F, 0x0E,  // A b c d E F G H I J
    0x75, 0x38, 0xB7, 0x54, 0x5C, 0x73, 0x67, 0x31, 0xC9, 0x78,  // K L M n o P q r S t
    0x3E, 0x1C, 0xFE, 0xE4, 0x6E, 0xDA, 0x40, 0x48, 0x80, 0x00   // U v W X Y Z - = . Null
};

void delay(u16 i);

void main()
{
    u8 i = 0;

    LSA = 0;
    LSB = 0;
    LSC = 0; //����38��������Y0����͵�ƽ

    while (1) {
        for (i = 0 ; i != 40 ; i++) {
            P0 = smgduan[i];
            delay(55000);
            delay(55000);
            P0 = smgduan[38];
            delay(55000);
        }
    }
}

void delay(u16 i)
{
    while (i--);
}
