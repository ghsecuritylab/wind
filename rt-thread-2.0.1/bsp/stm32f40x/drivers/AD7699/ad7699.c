#include "ad7699.h"
#include "dma.h"	
#include "stm32f4xx.h"                  // Device header
//////////////////////////////////////////////////////////////////////////////////
//��ʱԼ1us
void delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<25;i++)
		for(j=0;j<t;j++);
}
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////���ģʽSPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_MODE==0
//��ʼ��SPI FLASH��IO��
void AD7699_Init(void)
{ 
	GPIO_InitTypeDef  AD7699_IO;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	AD7699_IO.GPIO_Pin     = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;
	AD7699_IO.GPIO_OType   = GPIO_OType_PP;
	AD7699_IO.GPIO_Speed   = GPIO_Speed_100MHz;
	AD7699_IO.GPIO_Mode 	 = GPIO_Mode_OUT;
	AD7699_IO.GPIO_PuPd 	 = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &AD7699_IO);	
	
	AD7699_IO.GPIO_Pin     = GPIO_Pin_6;
	AD7699_IO.GPIO_OType   = GPIO_OType_OD;
  AD7699_IO.GPIO_Mode 	 = GPIO_Mode_IN;
  AD7699_IO.GPIO_PuPd 	 = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &AD7699_IO);	
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_7);												//AD7699_CS_H,AD7699_DIN_H
	GPIO_ResetBits(GPIOA,GPIO_Pin_5); 																//AD7699_CLK_L
	
//�ָ�AD7699��ؼĴ���Ϊ��������	
	AD7699_ReadWrite(0xFFFF);
	AD7699_ReadWrite(0xFFFF);
}  

unsigned short SPI_ReadWrite(unsigned short cmd)   
{
	unsigned char i;  
	unsigned short data;

	AD7699_CS_H;
	AD7699_CLK_L;
//	delay(3);
	AD7699_CS_L;
//	delay(3);
	for(i=0;i<16;i++)
	{		
		AD7699_CLK_L;
		
		if(cmd&0x8000)
			AD7699_DIN_H;
		else
			AD7699_DIN_L;
		cmd<<=1;		
//		delay(3);	
		
		AD7699_CLK_H;
//		delay(3);	
		
		data<<=1;
		if(AD7699_DOUT)
			data++;
	}
//	delay(1);
	AD7699_CS_H;
//	delay(1);
	return data;
} 

unsigned short AD7699_ReadWrite(unsigned short cmd)   
{   
  unsigned short dat;
	dat=SPI_ReadWrite(cmd);
	return dat;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////Ӳ��ģʽSPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#elif SPI_MODE==1
void AD7699_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  	SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);						  	//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);							  //ʹ��SPI1ʱ��
	
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode 	 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd 	 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  //GPIOA5,6,7��ʼ������
  GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	//PA5~7���ù������	
  GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF;											//���ù���
  GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;										//�������
  GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;								//100MHz
  GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_UP;											//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);															//��ʼ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1); 							//PA4����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 							//PA5����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 							//PA6����Ϊ SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 							//PA7����Ϊ SPI1
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);									//��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);								//ֹͣ��λSPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode 			= SPI_Mode_Master;									//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize 	= SPI_DataSize_16b;									//����SPI�����ݴ�С:SPI���ͽ���16λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 			= SPI_CPOL_Low;											//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA 			= SPI_CPHA_2Edge;										//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS 			= SPI_NSS_Soft;											//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  																//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
//  SPI_SSOutputCmd(SPI1,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
	
//�ָ�AD7699��ؼĴ���Ϊ��������	
	AD7699_ReadWrite(0xFFFF);
	AD7699_ReadWrite(0xFFFF);
}

void SPI1_SetSpeed(unsigned char SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); 	 	//�ж���Ч��
	SPI1->CR1&=0XFFC7;																									//λ3-5���㣬�������ò�����
	SPI1->CR1|=SPI_BaudRatePrescaler;																		//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); 																							//ʹ��SPI1
} 

unsigned short SPI_ReadWrite(unsigned short TxData) 
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);			//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); 	//ͨ������SPIx��������byte����
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 		//�ȴ�����������byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����		
}

unsigned short AD7699_ReadWrite(unsigned short cmd)   
{
	unsigned short data;
  AD7699_CS_H;
	AD7699_CS_L;
	data=SPI_ReadWrite(cmd);
	AD7699_CS_H;
	return data;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////DMAģʽ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
unsigned short SPI_Buffer_Tx[16];
unsigned short SPI_Buffer_Rx[16];

void SPI1_ReceiveSendByte( unsigned short num )
{	
		MYDMA_Enable(DMA2_Stream2,16);     										//��ʼһ��DMA����
		MYDMA_Enable(DMA2_Stream3,16);     										//��ʼһ��DMA����
	
		DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);						//���DMA2_Steam2������ɱ�־
		DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);						//���DMA2_Steam3������ɱ�־

    SPI1->DR ;																						//��һ���������SPI->DR

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//�ȴ���������  
	
    DMA_Cmd(DMA2_Stream2, ENABLE);                      	//����DMA���� 
    DMA_Cmd(DMA2_Stream3, ENABLE);                      	//����DMA����  
	
//		while(!DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2));//�ȴ�DMA2_Steam0�������

//		DMA_Cmd(DMA2_Stream2, DISABLE);                      	//�ر�DMA���� 
//    DMA_Cmd(DMA2_Stream3, DISABLE);                      	//�ر�DMA����
}

unsigned char SPI_DMA_Read(unsigned short* data,unsigned char bytesNumber)
{
    unsigned char byte = 0;
    for(byte = 0; byte < bytesNumber; byte++)
    {
       SPI_Buffer_Tx[byte]=0xFE4C;
    }
    AD7699_CS_H;
		AD7699_CS_L;
    SPI1_ReceiveSendByte(bytesNumber);
    AD7699_CS_H;
    for(byte = 0; byte < bytesNumber; byte++)
    {
       data[byte]=SPI_Buffer_Rx[byte];
    }		
    return bytesNumber;
}
#endif


