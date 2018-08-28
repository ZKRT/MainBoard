/**
  ******************************************************************************
  * @file    usart.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   串口配置
  *          + (1) ..
  *          + (2) 修改串口引脚 u4tx: PC10, u4rx: PC11, u2tx修改成u6tx: PC6, u2rx修改为u6rx: PC7  --by yanly161213
  *
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "sbus.h"
#include "led.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////DMA UART CONFIG
///*********USART1************/
//volatile uint8_t usart1_tx_buffer[USART_BUFFER_SIZE];//tx缓冲器
//volatile uint8_t usart1_rx_buffer[USART_BUFFER_SIZE];//rx缓冲器
//uint16_t usart1_tx_buffer_tail = 0;
//uint16_t usart1_tx_buffer_head = 0;
//uint16_t usart1_rx_buffer_pos = 0;
/////*********USART2************/
////volatile uint8_t usart2_tx_buffer[USART_BUFFER_SIZE];
////volatile uint8_t usart2_rx_buffer[USART_BUFFER_SIZE];
////uint16_t usart2_tx_buffer_tail = 0;
////uint16_t usart2_tx_buffer_head = 0;
////uint16_t usart2_rx_buffer_pos = 0;
///*********USART6************/ //modify by yanly
//volatile uint8_t usart6_tx_buffer[USART_BUFFER_SIZE];
//volatile uint8_t usart6_rx_buffer[USART_BUFFER_SIZE];
//volatile uint16_t usart6_tx_buffer_tail = 0;
//volatile uint16_t usart6_tx_buffer_head = 0;
//volatile uint16_t usart6_rx_buffer_pos = 0;
///*********USART3************/
//volatile uint8_t usart3_tx_buffer[USART_BUFFER_SIZE];
//volatile uint8_t usart3_rx_buffer[USART_BUFFER_SIZE];
//uint16_t usart3_tx_buffer_tail = 0;
//uint16_t usart3_tx_buffer_head = 0;
//uint16_t usart3_rx_buffer_pos = 0;
///*********USART4************/
//volatile uint8_t uart4_tx_buffer[USART_BUFFER_SIZE];
//volatile uint8_t uart4_rx_buffer[USART_BUFFER_SIZE];
//uint16_t uart4_tx_buffer_tail = 0;
//uint16_t uart4_tx_buffer_head = 0;
//uint16_t uart4_rx_buffer_pos = 0;

///* Private functions ---------------------------------------------------------*/
////初始化串口
//void uart_init(void)
//{
//  /*************对GPIO的配置*****************/
//  GPIO_InitTypeDef GPIO_InitStructure;
//  USART_InitTypeDef USART_InitStructure;
//
//  //时钟配置
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3|RCC_APB1Periph_UART4,ENABLE);
//#ifdef  USE_UART1_DMA
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
//#endif
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);  //for USART3/4
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);  //for USART6

//#ifdef  USE_UART1_DMA
//  //usart1tx dma
//  DMAX_init(DMA2_Stream7, DMA_Channel_4, (uint32_t)&USART1->DR, (uint32_t)usart1_tx_buffer, DMA_DIR_MemoryToPeripheral,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_High);
//
//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
//  NVICX_init(NVIC_PPRIORITY_DJIUSART, NVIC_SUBPRIORITY_DJIUSART);
//  DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
//
//  DMA_SetCurrDataCounter(DMA2_Stream7, 0);
//
//  //usart1rx dma
//  DMAX_init(DMA2_Stream5, DMA_Channel_4, (uint32_t)&USART1->DR, (uint32_t)usart1_rx_buffer, DMA_DIR_PeripheralToMemory,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Circular, DMA_Priority_VeryHigh);

//  DMA_Cmd(DMA2_Stream5, ENABLE);
//  usart1_rx_buffer_pos = DMA_GetCurrDataCounter(DMA2_Stream5);
//#endif

//  //433M usart6tx dma
//  DMAX_init(DMA2_Stream6, DMA_Channel_5, (uint32_t)&USART6->DR, (uint32_t)usart6_tx_buffer, DMA_DIR_MemoryToPeripheral,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_High);
//
//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream6_IRQn;
//  NVICX_init(NVIC_PPRIORITY_433MUT, NVIC_SUBPRIORITY_433MUT);
//  DMA_ITConfig(DMA2_Stream6, DMA_IT_TC, ENABLE);
//
//  DMA_SetCurrDataCounter(DMA2_Stream6, 0);
//
//  //433M usart6rx dma
//  DMAX_init(DMA2_Stream1, DMA_Channel_5, (uint32_t)&USART6->DR, (uint32_t)usart6_rx_buffer, DMA_DIR_PeripheralToMemory,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Circular, DMA_Priority_VeryHigh);

//  DMA_Cmd(DMA2_Stream1, ENABLE);
//  usart6_rx_buffer_pos = DMA_GetCurrDataCounter(DMA2_Stream1);
//#ifdef  USE_UART3_DMA
//  //test usart3tx dma
//  DMAX_init(DMA1_Stream3, DMA_Channel_4, (uint32_t)&USART3->DR, (uint32_t)usart3_tx_buffer, DMA_DIR_MemoryToPeripheral,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_High);
//
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
//  NVICX_init(NVIC_PPRIORITY_TESTUT, NVIC_SUBPRIORITY_TESTUT);
//  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
//
//  DMA_SetCurrDataCounter(DMA1_Stream3, 0);
//
//  //test usart3rx dma
//  DMAX_init(DMA1_Stream1, DMA_Channel_4, (uint32_t)&USART3->DR, (uint32_t)usart3_rx_buffer, DMA_DIR_PeripheralToMemory,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Circular, DMA_Priority_VeryHigh);

//  DMA_Cmd(DMA1_Stream1, ENABLE);
//  usart3_rx_buffer_pos = DMA_GetCurrDataCounter(DMA1_Stream1);
//#endif
//  //sbus/ppm usart4tx dma
//  DMAX_init(DMA1_Stream4, DMA_Channel_4, (uint32_t)&UART4->DR, (uint32_t)uart4_tx_buffer, DMA_DIR_MemoryToPeripheral,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Normal, DMA_Priority_High);
//
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
//  NVICX_init(NVIC_PPRIORITY_SBUSUT, NVIC_SUBPRIORITY_SBUSUT);
//  DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
//
//  DMA_SetCurrDataCounter(DMA1_Stream4, 0);
//
//  //sbus/ppm usart4rx dma
//  DMAX_init(DMA1_Stream2, DMA_Channel_4, (uint32_t)&UART4->DR, (uint32_t)uart4_rx_buffer, DMA_DIR_PeripheralToMemory,
//  USART_BUFFER_SIZE, DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte, DMA_Mode_Circular, DMA_Priority_VeryHigh);

//  DMA_Cmd(DMA1_Stream2, ENABLE);
//  uart4_rx_buffer_pos = DMA_GetCurrDataCounter(DMA1_Stream2);
//
//  //USART端口配置
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 ;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//  GPIO_Init(GPIOC,&GPIO_InitStructure);
//
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//  GPIO_Init(GPIOB,&GPIO_InitStructure);
//
//#ifdef  USE_UART1_DMA
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
//  GPIO_Init(GPIOA,&GPIO_InitStructure);
//#endif
//
//  //复用映射配置
//#ifdef  USE_UART1_DMA
//  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9, GPIO_AF_USART1);
//  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10, GPIO_AF_USART1);
//#endif
//  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
//  GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6, GPIO_AF_UART4);
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7, GPIO_AF_UART4);
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10, GPIO_AF_USART6);
//  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11, GPIO_AF_USART6);
//
//  //USART初始化设置
//  USART_InitStructure.USART_BaudRate = 57600;
//  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//  USART_InitStructure.USART_StopBits = USART_StopBits_1;
//  USART_InitStructure.USART_Parity = USART_Parity_No;
//  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//
//  /***********433M串口6*********/
//  USART_InitStructure.USART_BaudRate = 115200;
//  USART_Init(USART6, &USART_InitStructure);
//  USART_DMACmd(USART6,USART_DMAReq_Tx,ENABLE);
//  USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
//  USART_Cmd(USART6, ENABLE);
//  USART_ClearFlag(USART6, USART_FLAG_TC);
//
//  /***********TEST串口3*************/
//#ifdef  USE_UART3_DMA
//  USART_InitStructure.USART_BaudRate = 115200;
//  USART_Init(USART3, &USART_InitStructure);
//  USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);
//  USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
//  USART_Cmd(USART3, ENABLE);
//  USART_ClearFlag(USART3, USART_FLAG_TC);
//#else
//  USART_InitStructure.USART_BaudRate = 115200;
//  USART_Init(USART3, &USART_InitStructure);
//  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//  USART_Cmd(USART3, ENABLE);  //使能串口1
//  while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) != SET)
//    ;
//  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//  NVICX_init(NVIC_PPRIORITY_TESTUT, NVIC_SUBPRIORITY_TESTUT);
//#endif
//  /***********SBUS/PPM串口4*************/
//  USART_InitStructure.USART_BaudRate = 100000;
//  USART_Init(UART4, &USART_InitStructure);
//  USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);
//  USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);
//  USART_Cmd(UART4, ENABLE);
//  USART_ClearFlag(UART4, USART_FLAG_TC);
//
//#ifdef  USE_UART1_DMA
//  /***********串口1*************/
//  USART_Init(USART1, &USART_InitStructure);
//  USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
//  USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
//  USART_Cmd(USART1, ENABLE);
//  USART_ClearFlag(USART1, USART_FLAG_TC);
//#endif

//}


//////test yan
////volatile u8 usart1_txing = 0;
////void usart1_tx_send_msg(uint8_t* msg, uint16_t msg_size)
////{
////  u16 i;
////  while(usart1_txing);
////  DMA_Cmd(DMA2_Stream7, DISABLE);                      //关闭DMA传输
////  for(i=0; i<msg_size; i++)
////    usart1_tx_buffer[i] = msg[i];
////  while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE) //确保DMA可以被设置
////  DMA_SetCurrDataCounter(DMA2_Stream7,msg_size); //数据传输量
////  DMA_Cmd(DMA2_Stream7, ENABLE);                      //开启DMA传输
////  usart1_txing = 1;
////}
////void DMA2_Stream7_IRQHandler(void)
////{
////  if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET)
////  {
////    DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
////    DMA_Cmd(DMA2_Stream7,DISABLE);
////    usart1_txing = 0;
////  }
////}

//void usart1_tx_copyed(const char *str,uint16_t len)
//{
//  uint16_t count;
//
//  for (count = 0; count < len; count++)
//  {
//    usart1_tx_buffer[usart1_tx_buffer_head]= (uint8_t)str[count];
//
//    usart1_tx_buffer_head++;
//    if (usart1_tx_buffer_head == USART_BUFFER_SIZE)
//    {
//      usart1_tx_buffer_head = 0;
//    }
//  }
//}
//void usart1_tx_DMA(void)
//{
//  while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
//  {
//  }
//
//  DMA2_Stream7->M0AR = (uint32_t)&usart1_tx_buffer[usart1_tx_buffer_tail];
//
//  if(usart1_tx_buffer_tail < usart1_tx_buffer_head)
//  {
//    DMA_SetCurrDataCounter(DMA2_Stream7, usart1_tx_buffer_head - usart1_tx_buffer_tail);
//    usart1_tx_buffer_tail = usart1_tx_buffer_head;
//  }
//  else
//  {
//    DMA_SetCurrDataCounter(DMA2_Stream7, USART_BUFFER_SIZE - usart1_tx_buffer_tail);
//    usart1_tx_buffer_tail = 0;
//  }

//  DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
//
//  DMA_Cmd(DMA2_Stream7, ENABLE);
//}
//void DMA2_Stream7_IRQHandler(void)
//{
//  DMA_Cmd(DMA2_Stream7, DISABLE);
//
//  while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
//  {
//  }
//
//  if (usart1_tx_buffer_tail != usart1_tx_buffer_head)
//    usart1_tx_DMA();
//
//  DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
//#ifndef HWTEST_FUN
//  _FLIGHT_UART_TX_LED = 0;
//  usart1_tx_flag = TimingDelay;
//#endif
//}
//uint8_t usart1_rx_check(void)
//{
//  if (usart1_rx_buffer_pos == DMA_GetCurrDataCounter(DMA2_Stream5))
//    return 0;
//  else
//  {
//#ifndef HWTEST_FUN
//    _FLIGHT_UART_RX_LED = 0;
//    usart1_rx_flag = TimingDelay;
//#endif
//    return 1;
//  }
//}
//uint8_t usart1_rx_byte(void)
//{
//  uint8_t ch;
//
//  ch = usart1_rx_buffer[USART_BUFFER_SIZE - usart1_rx_buffer_pos];
//
//  usart1_rx_buffer_pos--;
//  if (usart1_rx_buffer_pos == 0)
//  {
//    usart1_rx_buffer_pos = USART_BUFFER_SIZE;
//  }

//  return ch;
//}
////usart3
//void usart3_tx_copyed(const char *str,uint16_t len)
//{
//  uint16_t count;
//
//  for (count = 0; count < len; count++)
//  {
//    usart3_tx_buffer[usart3_tx_buffer_head]= (uint8_t)str[count];
//
//    usart3_tx_buffer_head++;
//    if (usart3_tx_buffer_head == USART_BUFFER_SIZE)
//    {
//      usart3_tx_buffer_head = 0;
//    }
//  }
////  test_copyed_count++;
//}
//void usart3_tx_DMA(void)
//{
//  while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE)
//  {
//  }
//
//  DMA1_Stream3->M0AR = (uint32_t)&usart3_tx_buffer[usart3_tx_buffer_tail];
//
//  if(usart3_tx_buffer_tail < usart3_tx_buffer_head)
//  {
//    DMA_SetCurrDataCounter(DMA1_Stream3, usart3_tx_buffer_head - usart3_tx_buffer_tail);
//    usart3_tx_buffer_tail = usart3_tx_buffer_head;
//  }
//  else
//  {
//    DMA_SetCurrDataCounter(DMA1_Stream3, USART_BUFFER_SIZE - usart3_tx_buffer_tail);
//    usart3_tx_buffer_tail = 0;
//  }

//  DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);
//
//  DMA_Cmd(DMA1_Stream3, ENABLE);
////  test_tx_dma_count++;
//}
//void DMA1_Stream3_IRQHandler(void)  //tx it
//{
//  DMA_Cmd(DMA1_Stream3, DISABLE);
//
//  while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE)
//  {
//  }
//
//  if (usart3_tx_buffer_tail != usart3_tx_buffer_head)
//    usart3_tx_DMA();
//
//  DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);
//
////  test_inter_count++;
//}
//uint8_t usart3_rx_check(void)
//{
//  if (usart3_rx_buffer_pos == DMA_GetCurrDataCounter(DMA1_Stream1))
//    return 0;
//  else
//  {
//    return 1;
//  }
//}
//uint8_t usart3_rx_byte(void)
//{
//  uint8_t ch;
//
//  ch = usart3_rx_buffer[USART_BUFFER_SIZE - usart3_rx_buffer_pos];
//
//  usart3_rx_buffer_pos--;
//  if (usart3_rx_buffer_pos == 0)
//  {
//    usart3_rx_buffer_pos = USART_BUFFER_SIZE;
//  }

//  return ch;
//}
////usart6
//void usart6_tx_copyed(const char *str,uint16_t len)
//{
//  uint16_t count;
//
//  for (count = 0; count < len; count++)
//  {
//    usart6_tx_buffer[usart6_tx_buffer_head]= (uint8_t)str[count];
//
//    usart6_tx_buffer_head++;
//    if (usart6_tx_buffer_head == USART_BUFFER_SIZE)
//    {
//      usart6_tx_buffer_head = 0;
//    }
//  }
//}
//void usart6_tx_DMA(void)
//{
//  while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE)
//  {
//  }
//
//  DMA2_Stream6->M0AR = (uint32_t)&usart6_tx_buffer[usart6_tx_buffer_tail];
//
//  if(usart6_tx_buffer_tail < usart6_tx_buffer_head)
//  {
//    DMA_SetCurrDataCounter(DMA2_Stream6, usart6_tx_buffer_head - usart6_tx_buffer_tail);
//    usart6_tx_buffer_tail = usart6_tx_buffer_head;
//  }
//  else
//  {
//    DMA_SetCurrDataCounter(DMA2_Stream6, USART_BUFFER_SIZE - usart6_tx_buffer_tail);
//    usart6_tx_buffer_tail = 0;
//  }

//  DMA_ClearFlag(DMA2_Stream6,DMA_FLAG_TCIF6);
//
//  DMA_Cmd(DMA2_Stream6, ENABLE);
//}
//void DMA2_Stream6_IRQHandler(void)  //tx it
//{
//  DMA_Cmd(DMA2_Stream6, DISABLE);
//
//  while (DMA_GetCmdStatus(DMA2_Stream6) != DISABLE)
//  {
//  }
//
//  if (usart6_tx_buffer_tail != usart6_tx_buffer_head)
//    usart6_tx_DMA();
//
//  DMA_ClearITPendingBit(DMA2_Stream6,DMA_IT_TCIF6);
//#ifndef HWTEST_FUN
//  _OBSTACLE_AVOIDANCE_TX_LED = 0;
//  obstacle_avoidance_tx_flag = TimingDelay;
//#endif
//}
//uint8_t usart6_rx_check(void)
//{
//  if (usart6_rx_buffer_pos == DMA_GetCurrDataCounter(DMA2_Stream1))
//    return 0;
//  else
//  {
//#ifndef HWTEST_FUN
//    if(obstacle_avoidance_rx_flag >= TimingDelay+1000)  //zkrt_notice: 数据接收太快，灯频闪，所以这里改为延时1s才亮
//    {
//      _OBSTACLE_AVOIDANCE_RX_LED = 0;
//      obstacle_avoidance_rx_flag = TimingDelay;
//    }
//#endif
//    return 1;
//  }
//}
//uint8_t usart6_rx_byte(void)
//{
//  uint8_t ch;
//
//  ch = usart6_rx_buffer[USART_BUFFER_SIZE - usart6_rx_buffer_pos];
//
//  usart6_rx_buffer_pos--;
//  if (usart6_rx_buffer_pos == 0)
//  {
//    usart6_rx_buffer_pos = USART_BUFFER_SIZE;
//  }

//  return ch;
//}
////uart4
//void uart4_tx_copyed(const char *str,uint16_t len)
//{
//  uint16_t count;
//
//  for (count = 0; count < len; count++)
//  {
//    uart4_tx_buffer[uart4_tx_buffer_head]= (uint8_t)str[count];
//
//    uart4_tx_buffer_head++;
//    if (uart4_tx_buffer_head == USART_BUFFER_SIZE)
//    {
//      uart4_tx_buffer_head = 0;
//    }
//  }
//}
//void uart4_tx_DMA(void)
//{
//  while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE)
//  {
//  }
//
//  DMA1_Stream4->M0AR = (uint32_t)&uart4_tx_buffer[uart4_tx_buffer_tail];
//
//  if(uart4_tx_buffer_tail < uart4_tx_buffer_head)
//  {
//    DMA_SetCurrDataCounter(DMA1_Stream4, uart4_tx_buffer_head - uart4_tx_buffer_tail);
//    uart4_tx_buffer_tail = uart4_tx_buffer_head;
//  }
//  else
//  {
//    DMA_SetCurrDataCounter(DMA1_Stream4, USART_BUFFER_SIZE - uart4_tx_buffer_tail);
//    uart4_tx_buffer_tail = 0;
//  }

//  DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
//
//  DMA_Cmd(DMA1_Stream4, ENABLE);
//}
//void DMA1_Stream4_IRQHandler(void)  //tx it
//{
//  DMA_Cmd(DMA1_Stream4, DISABLE);
//
//  while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE)
//  {
//  }
//
//  if (uart4_tx_buffer_tail != uart4_tx_buffer_head)
//    uart4_tx_DMA();
//
//  DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
//
////  test_inter_count++;
//}
//uint8_t uart4_rx_check(void)
//{
//  if (uart4_rx_buffer_pos == DMA_GetCurrDataCounter(DMA1_Stream2))
//    return 0;
//  else
//  {
//    return 1;
//  }
//}
//uint8_t uart4_rx_byte(void)
//{
//  uint8_t ch;
//
//  ch = uart4_rx_buffer[USART_BUFFER_SIZE - uart4_rx_buffer_pos];
//
//  uart4_rx_buffer_pos--;
//  if (uart4_rx_buffer_pos == 0)
//  {
//    uart4_rx_buffer_pos = USART_BUFFER_SIZE;
//  }

//  return ch;
//}
////
////////////////////////////////////////////////////////////////////////////INTER UART CONFIG
#include "sys.h"
#include "hw_usart.h"
#include "usart.h"
#include "osusart.h"
/**
  * @brief  just init usart2, usart3, uart4, usart6
  * @param  None
  * @retval None
  */
static void uart_gpio_init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //nvic config
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PPRIORITY_U2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SUBPRIORITY_U2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PPRIORITY_U3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SUBPRIORITY_U3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PPRIORITY_U4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SUBPRIORITY_U4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PPRIORITY_U6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_SUBPRIORITY_U6;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //clk config
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 | RCC_APB1Periph_UART4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  //port config
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}
void usart_config(void) {
  USART_InitTypeDef USART_InitStructure;
  uint8_t UsartIndex = 0;

  uart_gpio_init();

  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Prepare all uart to receive a data packet */
  for (UsartIndex = 0; UsartIndex < USART_MAX_INDEX; UsartIndex++) {
    USART_InitStructure.USART_BaudRate = uart_baudrate[UsartIndex];
    USART_Init(UsartInstance[UsartIndex], &USART_InitStructure);
    /* Enable 8xUSARTs Receive interrupts */
    USART_ITConfig(UsartInstance[UsartIndex], USART_IT_RXNE, ENABLE);
    USART_ITConfig(UsartInstance[UsartIndex], USART_IT_TXE, DISABLE);
    /* Enable the NUMxUSARTs */
    USART_Cmd(UsartInstance[UsartIndex], ENABLE);
    //USART_ClearFlag(UsartInstance[UsartIndex], USART_FLAG_TC);
    USART_ClearFlag(UsartInstance[UsartIndex], USART_FLAG_RXNE);
  }
}

/**
*@
*/
