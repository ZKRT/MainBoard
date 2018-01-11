#ifndef __USART_H
#define __USART_H
#include "sys.h"

//#define USART_BUFFER_SIZE 295 

//extern uint16_t usart1_tx_buffer_tail;
//extern uint16_t usart1_tx_buffer_head;
//																		

//void uart_init(void);

//void usart1_tx_copyed(const char *str,uint16_t len);
//void usart1_tx_DMA(void);

//uint8_t usart1_rx_check(void);
//uint8_t usart1_rx_byte(void);

//void usart1_tx_send_msg(uint8_t* msg, uint16_t msg_size); //test yanly

////add by yanly
//void usart3_tx_copyed(const char *str,uint16_t len);
//void usart3_tx_DMA(void);
//uint8_t usart3_rx_check(void);
//uint8_t usart3_rx_byte(void);
//void usart1_tx_copyed(const char *str,uint16_t len);
//void usart1_tx_DMA(void);
//uint8_t usart1_rx_check(void);
//uint8_t usart1_rx_byte(void);
//void usart6_tx_copyed(const char *str,uint16_t len);
//void usart6_tx_DMA(void);
//uint8_t usart6_rx_check(void);
//uint8_t usart6_rx_byte(void);
//void uart4_tx_copyed(const char *str,uint16_t len);
//void uart4_tx_DMA(void);
//uint8_t uart4_rx_check(void);
//uint8_t uart4_rx_byte(void);

void usart_config(void);
#endif
