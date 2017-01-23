/**
  ******************************************************************************
  * @file    pwm.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   Header for pwm.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported types ------------------------------------------------------------*/
/** @defgroup something
  * @{
  */ 
/**
  * @}
  */ 
/* Exported constants --------------------------------------------------------*/
/*
	PWM:T10:CH1:PF6:APB2
	PWM1:T11_CH1:PF7:APB2
	PWM2:T13:CH1:PF8:APB1
	PWM3:T4:CH2:PB7:APB1
	PWM4:T3:CH3:PC8:APB1
*/
/** @defgroup pwm timer period and prescaler define
  * @{  
//see APB1_TIMER_CLK 84Mhz and APB2_TIMER_CLK 168Mhzto calucate period and prescaler value

  */ 
#define PWM_T_PERIOD					20000  //timer overflow is 20ms
#define PWM_T_PSC							168
#define PWM1_T_PERIOD					20000  //timer overflow is 20ms
#define PWM1_T_PSC						168
#define PWM2_T_PERIOD					20000  //timer overflow is 20ms
#define PWM2_T_PSC						84
#define PWM3_T_PERIOD					20000  //timer overflow is 20ms
#define PWM3_T_PSC						84
#define PWM4_T_PERIOD					20000  //timer overflow is 20ms
#define PWM4_T_PSC						84
/**
  * @}
  */ 
	
/** @defgroup pwm numberX is timerX 
  * @{  
  */ 
#define PWM_TIM								TIMER_CHANNEL_PWM
#define PWM1_TIM							TIMER_CHANNEL_PWM1
#define PWM2_TIM							TIMER_CHANNEL_PWM2
#define PWM3_TIM							TIMER_CHANNEL_PWM3
#define PWM4_TIM							TIMER_CHANNEL_PWM4
/**
  * @}
  */ 

//pwm output value
#define PWM_OUT(value)        TIM_SetCompare1(PWM_TIM, value);
#define PWM1_OUT(value)       TIM_SetCompare1(PWM1_TIM, value);
#define PWM2_OUT(value)       TIM_SetCompare1(PWM2_TIM, value);
#define PWM3_OUT(value)       TIM_SetCompare2(PWM3_TIM, value);
#define PWM4_OUT(value)       TIM_SetCompare3(PWM4_TIM, value);

//get pwm value
#define GET_PWM_V             TIM_GetCapture1(PWM_TIM)
#define GET_PWM1_V            TIM_GetCapture1(PWM_TIM1)
#define GET_PWM2_V            TIM_GetCapture1(PWM_TIM2)
#define GET_PWM3_V            TIM_GetCapture2(PWM_TIM3)
#define GET_PWM4_V            TIM_GetCapture3(PWM_TIM4)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void pwm_tim_init(void);

#endif /* _PWM_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

