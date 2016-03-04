#include "dma.h"	

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ�����  
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,unsigned int chx,unsigned int par,unsigned int mar,unsigned int mode,unsigned short ndtr)
{  
	DMA_InitTypeDef  DMA_InitStructure;
	
	if((unsigned int)DMA_Streamx>(unsigned int)DMA2)														//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);												//DMA2ʱ��ʹ�� 
		
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);												//DMA1ʱ��ʹ�� 
	}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}													//�ȴ�DMA������ 
	
  /* ���� DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  																			//ͨ��ѡ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = par;															//DMA�����ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = mar;																//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = mode;																						//���赽�洢��ģʽѡ��
  DMA_InitStructure.DMA_BufferSize = ndtr;																		//���ݴ����� 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//�������ݳ���:16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//�洢�����ݳ���:16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;																// ʹ����ͨģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;												//�е����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;									//�洢��ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;					//����ͻ�����δ���
  DMA_Init(DMA_Streamx, &DMA_InitStructure);																	//��ʼ��DMA Stream
} 
//����һ��DMA����
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,unsigned short ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          	//���ݴ�����  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      	//����DMA���� 
}	 

//����DMA�ж�
void DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef   DMA_NVIC;
/* Enable and set DMA Interrupt */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	DMA_NVIC.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	DMA_NVIC.NVIC_IRQChannelPreemptionPriority = 0x00;
	DMA_NVIC.NVIC_IRQChannelSubPriority = 0x00;
	DMA_NVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&DMA_NVIC);
}

void DMA2_Stream2_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2)==SET)		   //�˶��Ƿ���DMA�ж�
	{
		if(DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2)==SET); //DMA2_Steam0�����Ƿ����
		{
			DMA_Cmd(DMA2_Stream2, DISABLE);                      	 //�ر�DMA���� 
			DMA_Cmd(DMA2_Stream3, DISABLE);                      	 //�ر�DMA����
		}
	}
	DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);				 //����жϱ�־λ
}

