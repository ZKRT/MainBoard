/**
  ******************************************************************************
  * @file    osprintf.h
  * @author  yanly
  * @version 
  * @date    
  * @brief   usart interupt implement send and receive (hardware os layer)
  ******************************************************************************
  */ 

#ifndef __OSUSART5_H
#define __OSUSART5_H

#include "hw_usart.h"


extern void os_usart5(void);
extern scommReturn_t  t_hwuart5_ReceiveMessage(scommRcvBuf_t *rxMsg);
extern scommReturn_t  t_hwuart5_SendMessage(scommTxBuf_t*  txMsg);
extern u8  u1_hwuart5_txing(void);
extern void os_reset_rcvstate_u5(void);
extern u8 os_get_uart_chls_u5(void);
extern void usart5_config_chg(void);
#endif
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
