/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief  
  ******************************************************************************
  * @copy
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OSUSART_H
#define __OSUSART_H

#include "osusart1.h"
#include "osuart4.h"
#include "osuart5.h"

//#define  USART0_COM		0
#define USART1_COM		0
//#define  USART2_COM		0
//#define  USART3_COM		0
#define UART4_COM		1
#define UART5_COM		2

#define RS232_COM					UART5_COM
#define RS485_COM					UART4_COM
#define PLC_COM						USART1_COM


/******************************************************************************/
extern scommReturn_t  t_osscomm_ReceiveMessage(u8  *rxData, u16 *rxLen, u8 port);  //upper layer usart receive function  
extern scommReturn_t  t_osscomm_sendMessage(u8  *txData, u16 txLen, u8 port); //upper layer usart send function  
extern scommReturn_t  t_osscomm_sendMessage_mutex(u8  *txData, u16 txLen, u8 port);
extern void os_usart_init(void);
extern void os_reset_uart_rcvstate(u8 uPort);
extern u8 os_get_uart_channel_state(u8 uPort);
extern void usart_config_chg(u8 uPort);


//extern u8 plc_com_sendb[USART_TX_DATA_SIZE];
//extern u8 rs232_sendb[USART_TX_DATA_SIZE];
//extern u8 rs485_sendb[USART_TX_DATA_SIZE];
#endif 
