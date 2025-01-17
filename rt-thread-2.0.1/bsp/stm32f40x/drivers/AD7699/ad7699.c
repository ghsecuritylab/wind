#include "ad7699.h"
#include "dma.h"	
#include "stm32f4xx.h"                  // Device header
//////////////////////////////////////////////////////////////////////////////////
//延时约1us
void delay(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<25;i++)
		for(j=0;j<t;j++);
}
////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////软件模式SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#if SPI_MODE==0
//初始化SPI FLASH的IO口
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
	
//恢复AD7699相关寄存器为出厂设置	
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
//////////////////////////////////////////硬件模式SPI///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
#elif SPI_MODE==1
void AD7699_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  	SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);						  	//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,  ENABLE);							  //使能SPI1时钟
	
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode 	 	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd 	 	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  //GPIOA5,6,7初始化设置
  GPIO_InitStructure.GPIO_Pin 		= GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	//PA5~7复用功能输出	
  GPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF;											//复用功能
  GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;										//推挽输出
  GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;								//100MHz
  GPIO_InitStructure.GPIO_PuPd 		= GPIO_PuPd_UP;											//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);															//初始化
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1); 							//PA4复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); 							//PA5复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); 							//PA6复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); 							//PA7复用为 SPI1
 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);									//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);								//停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode 			= SPI_Mode_Master;									//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize 	= SPI_DataSize_16b;									//设置SPI的数据大小:SPI发送接收16位帧结构
	SPI_InitStructure.SPI_CPOL 			= SPI_CPOL_Low;											//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA 			= SPI_CPHA_2Edge;										//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS 			= SPI_NSS_Soft;											//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  																//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
//  SPI_SSOutputCmd(SPI1,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
//恢复AD7699相关寄存器为出厂设置	
	AD7699_ReadWrite(0xFFFF);
	AD7699_ReadWrite(0xFFFF);
}

void SPI1_SetSpeed(unsigned char SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); 	 	//判断有效性
	SPI1->CR1&=0XFFC7;																									//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;																		//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); 																							//使能SPI1
} 

unsigned short SPI_ReadWrite(unsigned short TxData) 
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);			//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); 	//通过外设SPIx发送两个byte数据
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); 		//等待接收完两个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据		
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
//////////////////////////////////////////DMA模式///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
unsigned short SPI_Buffer_Tx[16];
unsigned short SPI_Buffer_Rx[16];

void SPI1_ReceiveSendByte( unsigned short num )
{	
		MYDMA_Enable(DMA2_Stream2,16);     										//开始一次DMA传输�
		MYDMA_Enable(DMA2_Stream3,16);     										//开始一次DMA传输�
	
		DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2);						//清除DMA2_Steam2传输完成标志
		DMA_ClearFlag(DMA2_Stream3,DMA_FLAG_TCIF3);						//清除DMA2_Steam3传输完成标志

    SPI1->DR ;																						//读一次数，清空SPI->DR

		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待发送区空  
	
    DMA_Cmd(DMA2_Stream2, ENABLE);                      	//开启DMA传输 
    DMA_Cmd(DMA2_Stream3, ENABLE);                      	//开启DMA传输  
	
//		while(!DMA_GetFlagStatus(DMA2_Stream2,DMA_FLAG_TCIF2));//等待DMA2_Steam0传输完成

//		DMA_Cmd(DMA2_Stream2, DISABLE);                      	//关闭DMA传输 
//    DMA_Cmd(DMA2_Stream3, DISABLE);                      	//关闭DMA传输
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


