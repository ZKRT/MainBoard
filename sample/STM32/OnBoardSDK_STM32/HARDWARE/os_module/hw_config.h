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
	
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f10x.h"

//timer4
#define	RCC_TIMER_1ST																RCC_APB1Periph_TIM4
#define	TIMER_CHANNEL_1ST														TIM4
#define	TIMER_IRQ_CHANNEL_1ST												TIM4_IRQn
#define	NVIC_TIMER_PreemptionPriority_1ST						0
#define	NVIC_TIMER_SubPriority_1ST									1
//timer2
#define	RCC_TIMER_2ND																RCC_APB1Periph_TIM2
#define	TIMER_CHANNEL_2ND														TIM2
#define	TIMER_IRQ_CHANNEL_2ND												TIM2_IRQn
#define	NVIC_TIMER_PreemptionPriority_2ND						0     //中断优先级最高
#define	NVIC_TIMER_SubPriority_2ND									0

//usart
#define COMn                             				3

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1_STR                    "USART1"
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port4, connected to USART4
 */ 
#define EVAL_COM4_STR                    "USART4"
#define EVAL_COM4                        UART4
#define EVAL_COM4_CLK                    RCC_APB1Periph_UART4
#define EVAL_COM4_TX_PIN                 GPIO_Pin_10
#define EVAL_COM4_TX_GPIO_PORT           GPIOC
#define EVAL_COM4_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM4_RX_PIN                 GPIO_Pin_11
#define EVAL_COM4_RX_GPIO_PORT           GPIOC
#define EVAL_COM4_RX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM4_IRQn                   UART4_IRQn

/**
 * @brief Definition for COM port5, connected to USART5
 */ 
#define EVAL_COM5_STR                    "USART5"
#define EVAL_COM5                        UART5
#define EVAL_COM5_CLK                    RCC_APB1Periph_UART5
#define EVAL_COM5_TX_PIN                 GPIO_Pin_12
#define EVAL_COM5_TX_GPIO_PORT           GPIOC
#define EVAL_COM5_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM5_RX_PIN                 GPIO_Pin_2
#define EVAL_COM5_RX_GPIO_PORT           GPIOD
#define EVAL_COM5_RX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM5_IRQn                   UART5_IRQn

#endif /* __HW_CONFIG_H */
