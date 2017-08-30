/**
  ******************************************************************************
  * @file    hw_config.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   hardware configure message
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported macro ------------------------------------------------------------*/

/** @defgroup hardware open or close control
  * @{
  */              
//#define _TEMPTURE_IO_           
#define _TEMPTURE_ADC_	                //暂时不关闭，在使用集成板采集温度时
//#define _TEMPTURE_NONE    
//#define USE_UART1_DMA
#define USE_CAN2_FUN                  
#define USE_LED_FUN			
#define USE_IIC_FUN  
#define USE_DJI_FUN
#define USE_OBSTACLE_AVOID_FUN
//#define USE_UNDERCARRIAGE_FUN         //暂时屏蔽，新硬件版本4.0控制脚有改动，待做
#define USE_SESORINTEGRATED             //使用集成板功能：采集温度和距离数据
//#define USE_UART3_TEST_FUN              //使用调试串口3进行功能性测试
//#define USE_OBSTACLE_TEST1            //使用避障测试1部分：避障数据和机体数据通过调试串口假定测试
//#define USE_OBSTACLE_TEST2							//避障测试2部分：避障距离数据通过调试串口假定测试 ---需要连接避温避障集成板，需要连接A3飞控,需要开启模拟飞行器
//#define USE_EXTRAM_FUN
//#define USE_USB_FUN
//#define USE_LWIP_FUN
//#define HWTEST_FUN

//unit test debug
//#define CanSend2SubModule_TEST  

/**
  * @}
  */ 

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/** @defgroup clock label
  * @{
  */ 
#define SYSTEM_CLK	              168
#define APB1_CLK		              42
#define APB2_CLK                  84
#define APB1_TIMER_CLK	          84
#define APB2_TIMER_CLK            168	
/**
  * @}
  */ 

/** @defgroup fast timer task config
  * @{
  */ 
#define	RCC_TIMER_1ST																RCC_APB1Periph_TIM12
#define	TIMER_CHANNEL_1ST														TIM12
#define	TIMER_IRQ_CHANNEL_1ST												TIM8_BRK_TIM12_IRQn
#define	NVIC_TIMER_PreemptionPriority_1ST						2
#define	NVIC_TIMER_SubPriority_1ST									1
/**
  * @}
  */ 

/** @defgroup quick fast timer config
  * @{
  */ 
#define	RCC_TIMER_2ND																RCC_APB1Periph_TIM14
#define	TIMER_CHANNEL_2ND														TIM14
#define	TIMER_IRQ_CHANNEL_2ND												TIM8_TRG_COM_TIM14_IRQn
#define	NVIC_TIMER_PreemptionPriority_2ND						2     //中断优先级最高
#define	NVIC_TIMER_SubPriority_2ND									0
/**
  * @}
  */ 
	
/** @defgroup lwip timer config
  * @{
  */ 
#define	RCC_TIMER_LWIP															RCC_APB1Periph_TIM2
#define	TIMER_CHANNEL_LWIP													TIM2
#define	TIMER_IRQ_CHANNEL_LWIP											TIM2_IRQn
#define	NVIC_TIMER_PreemptionPriority_LWIP					2     //中断优先级最高
#define	NVIC_TIMER_SubPriority_LWIP									2
/**
  * @}
  */ 
/** @defgroup pwm timer config
  * @{
  */ 
#define	TIMER_CHANNEL_PWM													  TIM10
#define	TIMER_CHANNEL_PWM1													TIM13
#define	TIMER_CHANNEL_PWM2													TIM5
#define	TIMER_CHANNEL_PWM3													TIM11
#define	TIMER_CHANNEL_PWM4													TIM14

/**
  * @}
  */ 

/** @defgroup nvic priority config
  * @{
  */ 
//network nvic
#define NVIC_PPRIORITY_NETWORK											0
#define NVIC_SUBPRIORITY_NETWORK                    0
//can1 nvic
#define NVIC_PPRIORITY_CAN											    0
#define NVIC_SUBPRIORITY_CAN                        3
//can2 nvic
#define NVIC_PPRIORITY_CAN2											    0
#define NVIC_SUBPRIORITY_CAN2                       4
//dji usart nvic
#define NVIC_PPRIORITY_DJIUSART											3
#define NVIC_SUBPRIORITY_DJIUSART	                  2
//433m uart nvic 
#define NVIC_PPRIORITY_433MUT											  1
#define NVIC_SUBPRIORITY_433MUT                     1
//test usart nvic 
#define NVIC_PPRIORITY_TESTUT											  1
#define NVIC_SUBPRIORITY_TESTUT                     2
//sbus/ppm uart nvic 
#define NVIC_PPRIORITY_SBUSUT											  1
#define NVIC_SUBPRIORITY_SBUSUT                     3	
/**
  * @}
  */ 
	
	
/** @defgroup pin define 
  * @{
  */ 
/**
  * @}
  */ 


/* Exported functions ------------------------------------------------------- */

#endif /* __HW_CONFIG_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

