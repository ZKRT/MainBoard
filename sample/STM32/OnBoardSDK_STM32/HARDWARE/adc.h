/**
  ******************************************************************************
  * @file    adc.h
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   Header for adc.c module
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H
#define __ADC_H	

/* Includes ------------------------------------------------------------------*/
#include "sys.h" 

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/** @defmodify modify by yanly for remove voltage check
  * @{

#if defined _TEMPTURE_IO_
#define ADC_BUFFER_SIZE   30 
#elif defined _TEMPTURE_ADC_
#define ADC_BUFFER_SIZE   50 
  * @}
  */ 

#define ADC_USE_CHANNEL		2
#define ADC_CHECK_SUM			10  //每次使用10次ADC采集结果来计算平均值
#define ADC_BUFFER_SIZE   20  //ADC_BUFFER_SIZE = ADC_USE_CHANNEL*ADC_CHECK_SUM //modify by yanly for [2]

/** @defmodify modify by yanly for remove voltage check
  * @{

#define _25V_value 0
#define _5V_value  1
#define _5A_value  2
#define _T1_value  3
#define _T2_value  4
  * @}
  */ 	
	
/** @defgroup adc buffer item number //add by yanly for [2]
  * @{
	ex: adc1_rx_buffer[_T1_value]
  */ 
#define _T1_value  0
#define _T2_value  1
#define _25V_value 0xff  //nothing yanly
#define _5V_value  0xfe
#define _5A_value  0xfd
/**
  * @}
  */ 

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void ADC1_Init(void); 				//ADC通道初始化
uint16_t ADC1_get_value(uint8_t read_type);

#endif /*__ADC_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
