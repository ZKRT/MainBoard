/**
  ******************************************************************************
  * @file    osprintf.h
  * @author  yanly
  * @version 
  * @date    
  * @brief   usart interupt implement send and receive (hardware os layer)
  ******************************************************************************
  */ 

#ifndef __OSUSART4_H
#define __OSUSART4_H

#include "hw_usart.h"

#define TX_485  1
#define RX_485  0

extern void RS485_DE(u8 rs485_t);
extern void os_usart4(void);
extern scommReturn_t  t_hwuart4_ReceiveMessage(scommRcvBuf_t *rxMsg);
extern scommReturn_t  t_hwuart4_SendMessage(scommTxBuf_t*  txMsg);
extern u8  u1_hwuart4_txing(void);
extern void os_reset_rcvstate_u4(void);
extern u8 os_get_uart_chls_u4(void);
extern void usart4_config_chg(void);
#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
