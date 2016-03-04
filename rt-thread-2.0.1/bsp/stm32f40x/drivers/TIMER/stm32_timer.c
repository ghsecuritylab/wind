#include "stm32_timer.h"
#include "stm32f4xx.h"                  // Device header

extern float v[8];
extern unsigned int t;
extern unsigned short ad7699_data[16];
unsigned int tim;
//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{
	unsigned char i;
	if(TIM3->SR&0X0001)//����ж�
	{	
		tim	=t;
		t=0;
	}	
//	if(tim==0xffffffff)
//		tim=0;
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(unsigned short arr,unsigned short psc)
{	
	NVIC_InitTypeDef TIM3_NVIC;
	
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  		//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  		//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	
	TIM3_NVIC.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	TIM3_NVIC.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	TIM3_NVIC.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	TIM3_NVIC.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&TIM3_NVIC);  	
}














