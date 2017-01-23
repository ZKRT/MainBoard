/*! @file BspUsart.h
 *  @version 3.1.7
 *  @date Jul 01 2016
 *
 *  @brief
 *  Usart helper functions and ISR for board STM32F4Discovery
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#ifndef _BSPUSART_H
#define _BSPUSART_H
#include <stm32f4xx.h>
//#include "stdarg.h"
#include "stdio.h"
void USART1_Config(void);
void USARTxNVIC_Config(void);
void Usart_DJI_Config(void);
void NVIC_Config(void);
void Rx_buff_Handler() ;
void main_dji_recv(void);
#endif  //_USART_H
