/**
  ******************************************************************************
  * @file    led.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   led
  *           + (1)...
								PC0：RUN
								PC1：ALRM
								PC2：H/S
								PD0：USART1_TX
								PD1：USART1_RX
								PD2：USART2_TX
								PD3：USART2_RX
								PD4：CAN1_RX
								PD5：CAN1_TX

								PA8：对应5V电压的使能。0关闭，1使能
								PE2：对应爆闪模块1的供电使能。0关闭，1使能
								PE9：对应爆闪模块2的供电使能。0关闭，1使能

  *           + (2)修改LED引脚; 屏蔽5V电压的使能、爆闪模块1的供电使能和爆闪模块2的
	*             供电使能。	--161214 by yanly
								PE4：RUN
								PE5：ALRM
								PE6：H/S
								PC13：USART1_TX
								PC12：USART1_RX
								PC14：USART6_TX
								PC15：USART6_RX
								PA5：CAN1_RX
								PA4：CAN1_TX
  *         
 ===============================================================================
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "led.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef USE_LED_FUN
char led_none;
#endif
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defmodify modify by yanly for (2)
  * @{
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //yanly test
	
	
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	
	delay_ms(500);										
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);		  

	GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
	GPIO_SetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_9);
	
	//test yanly
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOF, GPIO_Pin_10 | GPIO_Pin_9);
}
  * @}
  */ 

/**
  * @brief  led_init.
  * @param  None
  * @retval None
  */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //zkrt_todo: have to remove //yanly test
	
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	delay_ms(500);										
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		  
	
	GPIO_SetBits(GPIOE, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
	
	//zkrt_todo: have to remove //yanly test
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
//	GPIO_Init(GPIOF, &GPIO_InitStructure);
//	GPIO_ResetBits(GPIOF, GPIO_Pin_10 | GPIO_Pin_9);
}

/**
  * @brief  led_test. 所有LED灯一秒闪烁一次，测试led
  * @param  None
  * @retval None
  */
void led_test(void)
{
	char i =0;
//all high	
//	_RUN_LED = 1;
//	_ALARM_LED = 1;
//	_HS_LED = 1;
//	_FLIGHT_UART_TX_LED = 1;
//	_FLIGHT_UART_RX_LED = 1;
//	_433M_UART_TX_LED = 1;
//	_433M_UART_RX_LED = 1;
//	_CAN_RX_LED = 1;
//	_CAN_TX_LED = 1;	
//all low
//	_RUN_LED = 0;
//	_ALARM_LED = 0;
//	_HS_LED = 0;
//	_FLIGHT_UART_TX_LED = 0;
//	_FLIGHT_UART_RX_LED = 0;
//	_433M_UART_TX_LED = 0;
//	_433M_UART_RX_LED = 0;
//	_CAN_RX_LED = 0;
//	_CAN_TX_LED = 0;	
	while(i<30)
	{
		i++;
		delay_ms(500);
		_RUN_LED = !_RUN_LED;
		_ALARM_LED = !_ALARM_LED;
		_HS_LED = !_HS_LED;
		_FLIGHT_UART_TX_LED = !_FLIGHT_UART_TX_LED;
		_FLIGHT_UART_RX_LED = !_FLIGHT_UART_RX_LED;
		_433M_UART_TX_LED = !_433M_UART_TX_LED;
		_433M_UART_RX_LED = !_433M_UART_RX_LED;
		_CAN_RX_LED  =!_CAN_RX_LED;
		_CAN_TX_LED = !_CAN_TX_LED;	
		PFout(9) = ! PFout(9);
		PFout(10) = ! PFout(10);
	}
	GPIO_ResetBits(GPIOF, GPIO_Pin_10 | GPIO_Pin_9);
	_RUN_LED = 1;
	_ALARM_LED = 1;
	_HS_LED = 1;
	_FLIGHT_UART_TX_LED = 1;
	_FLIGHT_UART_RX_LED = 1;
	_433M_UART_TX_LED = 1;
	_433M_UART_RX_LED = 1;
	_CAN_RX_LED = 1;
	_CAN_TX_LED = 1;	
}
/**
  * @brief  led_process. led灯控制
  * @param  None
  * @retval None
  */
void led_process(void)
{
	//run led control
	if ((mavlink_send_flag-TimingDelay) < 50)  //与发送心跳包共用一个标记mavlink_send_flag，在心跳包处理里置mavlink_send_flag
	{
		_RUN_LED = 0;
	}
	else if ((mavlink_send_flag-TimingDelay) < 800)
	{
		_RUN_LED = 1;
	}
	//flight led control
	if ((usart1_tx_flag-TimingDelay) > 50)
	{
		_FLIGHT_UART_TX_LED = 1;
	}
	if ((usart1_rx_flag-TimingDelay) > 50)
	{
		_FLIGHT_UART_RX_LED = 1;
	}
	//can led control
	if ((can_rx_flag-TimingDelay)  > 50)
	{
		_CAN_RX_LED = 1;
	}
	if ((can_tx_flag-TimingDelay)  > 50)
	{
		_CAN_TX_LED = 1;
	}	
	//433m led control
	if ((u433m_tx_flag-TimingDelay) > 50)
	{
		_433M_UART_TX_LED = 1;
	}
	if ((u433m_rx_flag-TimingDelay) > 50)
	{
		_433M_UART_RX_LED = 1;
	}
}	
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
