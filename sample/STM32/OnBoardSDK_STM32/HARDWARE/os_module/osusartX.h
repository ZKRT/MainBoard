/**
  ******************************************************************************
  * @file    osusartX.h
  * @author  yanly
  * @version 
  * @date    
  * @brief   usart interupt implement send and receive (hardware os layer)
  ******************************************************************************
  */ 

#ifndef __OSUSARTX_H
#define __OSUSARTX_H

/* Includes ------------------------------------------------------------------*/
#include "hw_usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

void os_usartX(void);
scommReturn_t  t_hwuartX_SendMessage(scommTxBuf_t*  txMsg, uint8_t ustate_item, USART_TypeDef* COM);
scommReturn_t  t_hwuartX_ReceiveMessage(scommRcvBuf_t *rxMsg, uint8_t ustate_item, USART_TypeDef* COM);
uint8_t  u1_hwuartX_txing(uint8_t ustate_item, USART_TypeDef* COM);
uint8_t whatis_arrynum_of_USART(USART_TypeDef* COM);
/**
  * @}
  */ 

/**
  * @}
  */ 

#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
