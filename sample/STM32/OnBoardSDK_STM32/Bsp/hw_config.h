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
//#define _TEMPTURE_ADC_	                
//#define _TEMPTURE_NONE    
//#define USE_UART1_DMA
#define USE_CAN2_FUN                  
#define USE_LED_FUN			
#define USE_IIC_FUN  
#define USE_DJI_FUN
#define USE_OBSTACLE_AVOID_FUN
//#define USE_UNDERCARRIAGE_FUN         //��ʱ���Σ���Ӳ���汾4.0���ƽ��иĶ�������
#define USE_SESORINTEGRATED             //ʹ�ü��ɰ幦�ܣ��ɼ��¶Ⱥ;�������
//#define USE_UART3_TEST_FUN              //ʹ�õ��Դ���3���й����Բ���
//#define USE_OBSTACLE_TEST1            //ʹ�ñ��ϲ���1���֣��������ݺͻ�������ͨ�����Դ��ڼٶ�����
//#define USE_OBSTACLE_TEST2							//���ϲ���2���֣����Ͼ�������ͨ�����Դ��ڼٶ����� ---��Ҫ���ӱ��±��ϼ��ɰ壬��Ҫ����A3�ɿ�,��Ҫ����ģ�������
//#define USE_EXTRAM_FUN
//#define USE_USB_FUN
//#define USE_LWIP_FUN
//#define USE_PELCOD_FUN
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
#define SYSTEM_CLK                168
#define APB1_CLK                  42
#define APB2_CLK                  84
#define APB1_TIMER_CLK	          84
#define APB2_TIMER_CLK            168	
/**
  * @}
  */ 

/** @defgroup fast timer task config
  * @{
  */ 
#define	FTTC_RCC_CLK                                            RCC_APB1Periph_TIM12
#define	FTTC_TIM_NUM                                            TIM12
#define FTTC_TIMER_CLK                                          APB1_TIMER_CLK
#define	TIMER_IRQ_CHANNEL_1ST                                   TIM8_BRK_TIM12_IRQn
#define	NVIC_TIMER_PreemptionPriority_1ST                       0
#define	NVIC_TIMER_SubPriority_1ST                              1
/**
  * @}
  */ 

/** @defgroup quick fast timer config
  * @{
  */ 
#define	QTTC_RCC_CLK                                            RCC_APB1Periph_TIM14
#define	QTTC_TIM_NUM                                            TIM14
#define QTTC_TIMER_CLK                                          APB1_TIMER_CLK
#define	TIMER_IRQ_CHANNEL_2ND                                   TIM8_TRG_COM_TIM14_IRQn
#define	NVIC_TIMER_PreemptionPriority_2ND                       0     //�ж����ȼ����
#define	NVIC_TIMER_SubPriority_2ND                              2
/**
  * @}
  */ 
	
/** @defgroup lwip timer config
  * @{
  */ 
#define	RCC_TIMER_LWIP                                          RCC_APB1Periph_TIM2
#define	TIMER_CHANNEL_LWIP                                      TIM2
#define	TIMER_IRQ_CHANNEL_LWIP                                  TIM2_IRQn
#define	NVIC_TIMER_PreemptionPriority_LWIP                      2     //�ж����ȼ����
#define	NVIC_TIMER_SubPriority_LWIP                             2
/**
  * @}
  */ 
/** @defgroup pwm timer config
  * @{
  */ 
#define	TIMER_CHANNEL_PWM                                       TIM10
#define	TIMER_CHANNEL_PWM1                                      TIM13
#define	TIMER_CHANNEL_PWM2                                      TIM5
#define	TIMER_CHANNEL_PWM3                                      TIM11
#define	TIMER_CHANNEL_PWM4                                      TIM14

/**
  * @}
  */ 

/** @defgroup nvic priority config
  * @{
  */ 
//network nvic
#define NVIC_PPRIORITY_NETWORK											0
#define NVIC_SUBPRIORITY_NETWORK                                        0
//can1 nvic
#define NVIC_PPRIORITY_CAN											    0
#define NVIC_SUBPRIORITY_CAN                                            3
//can2 nvic
#define NVIC_PPRIORITY_CAN2											    0
#define NVIC_SUBPRIORITY_CAN2                                           4
//dji usart nvic
#define NVIC_PPRIORITY_DJIUSART											1
#define NVIC_SUBPRIORITY_DJIUSART	                                    1
//uart2 nvic 
#define NVIC_PPRIORITY_U2                        1
#define NVIC_SUBPRIORITY_U2                      2
//uart3 nvic 
#define NVIC_PPRIORITY_U3                        1
#define NVIC_SUBPRIORITY_U3                      3
//uart4 nvic 
#define NVIC_PPRIORITY_U4                        1
#define NVIC_SUBPRIORITY_U4                      4
//uart6 nvic 
#define NVIC_PPRIORITY_U6                        1
#define NVIC_SUBPRIORITY_U6                      5
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

