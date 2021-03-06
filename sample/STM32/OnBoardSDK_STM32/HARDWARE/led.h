#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h" 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup LED Control Directory
  * @{
  */ 
#ifdef USE_LED_FUN	
#define _RUN_LED                                     PFout(11)
#define _ALARM_LED                                   PBout(15)
#define _HS_LED                                      PBout(14)
#define _FLIGHT_UART_TX_LED                          PGout(8)  
#define _FLIGHT_UART_RX_LED                          PCout(12)
#define _TEST_UART_TX_LED                            PAout(15)
#define _TEST_UART_RX_LED                            PDout(6)
#define _433M_UART_TX_LED                            PAout(15)
#define _433M_UART_RX_LED                            PDout(6)
#define _CAN_TX_LED                                  PGout(6) 
#define _CAN_RX_LED                                  PGout(7)
#else
#define _RUN_LED                                     led_none
#define _ALARM_LED                                   led_none
#define _HS_LED                                      led_none
#define _FLIGHT_UART_TX_LED                          led_none
#define _FLIGHT_UART_RX_LED                          led_none
#define _TEST_UART_TX_LED                            led_none
#define _TEST_UART_RX_LED                            led_none
#define _433M_UART_TX_LED                            led_none
#define _433M_UART_RX_LED                            led_none
#define _CAN_TX_LED                                  led_none
#define _CAN_RX_LED                                  led_none
#endif
/**
  * @}
  */ 

#ifdef HWTEST_FUN
//hw test run led light timeout
#define RUNLED_TIMEOUT_OFFSET                        1
#define RUN_LEN_NETINIT_NONE_TIMEOUT                 0
#define RUN_LED_NETINIT_TIMEOUT                      4+RUNLED_TIMEOUT_OFFSET //unit: 500ms, 2s
#define RUN_LEN_CLIENT_CONC_TIMEOUT                  2+RUNLED_TIMEOUT_OFFSET //unit: 500ms, 1s
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void led_init(void);
void led_test(void);
void led_process(void);
extern char led_none;
#ifdef HWTEST_FUN
extern u8 sys_led_timeout;
extern volatile u8 sys_led_flag;
#endif
#endif
