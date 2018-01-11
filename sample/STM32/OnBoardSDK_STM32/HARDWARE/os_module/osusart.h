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

#include "hw_usart.h"

scommReturn_t  t_osscomm_ReceiveMessage(uint8_t  *rxData, uint16_t *rxLen, USART_TypeDef* COM);  //upper layer usart receive function  
scommReturn_t  t_osscomm_sendMessage(uint8_t  *txData, uint16_t txLen, USART_TypeDef* COM); //upper layer usart send function  
void os_usart_init(void);

#endif 
