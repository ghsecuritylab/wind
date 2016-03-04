#ifndef __ad7699_H
#define __ad7699_H

//ʹ�����ģ��SPI����Ӳ��SPI
#define SPI_MODE 		1

//AD7699��ؼĴ�������
#define CFG 					0xFFEC//�ⲿ��׼Դ,����ο�GND,8��ͨ��ѭ��ɨ��
#define CFG1 					0xEF2C//�ڲ���׼Դ,����ο�COM,8��ͨ��ѭ��ɨ��
#define AD7699_CFG1 	0xF140//�ⲿ��׼Դ,����ο�GND,ͨ��1
#define AD7699_CFG2 	0xF340//�ⲿ��׼Դ,����ο�GND,ͨ��2
#define AD7699_CFG3 	0xF540//�ⲿ��׼Դ,����ο�GND,ͨ��3
#define AD7699_CFG4 	0xF740//�ⲿ��׼Դ,����ο�GND,ͨ��4
#define AD7699_CFG5 	0xF940//�ⲿ��׼Դ,����ο�GND,ͨ��5
#define AD7699_CFG6 	0xFB40//�ⲿ��׼Դ,����ο�GND,ͨ��6
#define AD7699_CFG7 	0xFD40//�ⲿ��׼Դ,����ο�GND,ͨ��7
#define AD7699_CFG8 	0xFF40//�ⲿ��׼Դ,����ο�GND,ͨ��8


//AD7699��׼��ѹԴ
#define Vref 		5.000
#define Vref1 	4.096

#define AD7699_CS_H				GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define AD7699_CS_L				GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define AD7699_CLK_H			GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define AD7699_CLK_L			GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define AD7699_DIN_H			GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define AD7699_DIN_L			GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define AD7699_DOUT				GPIOA->IDR&(1<<6)
//#define AD7699_DOUT				GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)

void AD7699_Init(void);
unsigned short SPI_ReadWrite(unsigned short cmd); 
unsigned short AD7699_ReadWrite(unsigned short cmd); 
void SPI1_SetSpeed(unsigned char SPI_BaudRatePrescaler);

#endif