/*! @file BspUsart.cpp
 *  @version 3.1.7
 *  @date Jul 01 2016
 *
 *  @brief
 *  Usart helper functions and ISR for board STM32F4Discovery
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#include "BspUsart.h"
#include "DJI_API.h"
#include "DJI_HardDriver.h"
#include "DJI_Flight.h"

#ifdef __cplusplus
extern "C"
{
#include "zkrt.h"
#include "tempture.h"
#include "djiapp.h"
#include "sys.h"
}
#endif //__cplusplus


extern int Rx_Handle_Flag;
//int k=0;
using namespace DJI::onboardSDK;
extern CoreAPI defaultAPI;
extern CoreAPI *coreApi;
extern Flight flight;
extern FlightData flightData;

unsigned char come_data;
extern unsigned char Rx_length;
extern int Rx_adr;
extern int Rx_Handle_Flag;
extern unsigned char Rx_buff[];
extern zkrt_packet_t main_dji_rev;


extern "C" void DMAX_init(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel, uint32_t peripheral_addr, uint32_t memory_addr,uint32_t direction,
                          uint16_t buff_size, uint32_t peripheral_size, uint32_t memory_size, uint32_t dma_mode, uint32_t dma_prrty);
extern "C" void NVICX_init(uint8_t pre_prrty, uint8_t sub_prrty);
#if 1
/***********************************************************************
		         中科瑞泰消防无人机函数定义

		函数 名称：大疆串口初始化函数
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
#if 1
void USART1_Config(void)
{
  /*************对GPIO的配置*****************/
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  //时钟配置
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

  //USART端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //PA9、PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化琍PA9，PA10


  //复用映射配置
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);//GPIOA9 复用为USART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//GPIOA10复用为USART1

  //USART1的初始化设置
  USART_InitStructure.USART_BaudRate = 230400;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /***********串口1**********/
  USART_Init(USART1, &USART_InitStructure); //初始化串口
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  USART_Cmd(USART1, ENABLE);  //使能串口1
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET)
    ;
}
void USARTxNVIC_Config()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PPRIORITY_DJIUSART;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SUBPRIORITY_DJIUSART;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void Usart_DJI_Config()
{
  USART1_Config();
  USARTxNVIC_Config();
}
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

	
	//void USART1_IRQHandler(void) //zkrt_todo: have to modify by yanly
//{
//  if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
//  {
//    come_data=USART_ReceiveData(USART1);
//    coreApi->byteHandler(come_data); //Data from M100 were committed to "byteHandler"
//    //  if (come_data==0x55)
//    //   printf("0x %x   ",come_data);
//  }
//}
#ifdef __cplusplus
}
#endif //__cplusplus
