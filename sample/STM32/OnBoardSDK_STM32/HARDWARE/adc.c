/**
  ******************************************************************************
  * @file    adc.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   ADC信号采集监测
  *           + 电压监测  
  *           + [2]修改温度探头AD采集引脚 PB1 PB0，去除25V，5V电压，5A电流检测 --161213 by yanly
  *         
    @verbatim
 ===============================================================================
                     ##### How to use this api #####
 ===============================================================================
    [..] 
  
    @endverbatim
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "stm32f4xx_adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 
/* Private macro -------------------------------------------------------------*/
//#define ADCTEMP_VENDER_1		
#define ADCTEMP_VENDER_2

/* Private variables ---------------------------------------------------------*/
volatile uint16_t adc1_rx_buffer[ADC_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ADC1_Init.
  * @param  None
  * @retval None
  */
void  ADC1_Init(void)
{
	GPIO_InitTypeDef  		GPIO_InitStructure;	
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;   
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	 //for adc1（改动一：DMA需要开启时钟）	
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOA和GPIOC的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//GPIOB的时钟 //modify by yanly
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	DMAX_init(DMA2_Stream4, DMA_Channel_0, (uint32_t)&ADC1->DR, (uint32_t)adc1_rx_buffer, DMA_DIR_PeripheralToMemory, 
	ADC_BUFFER_SIZE, DMA_PeripheralDataSize_HalfWord, DMA_MemoryDataSize_HalfWord, DMA_Mode_Circular, DMA_Priority_Medium);
	
	DMA_Cmd(DMA2_Stream4, ENABLE);       
	
/** @defmodify modify by yanly for remove voltage check
  * @{
	#if defined _TEMPTURE_IO_	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//PA4，25.2V；PA5，5V
	#elif defined _TEMPTURE_ADC_
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;//PA4，25.2V；PA5，5V；PA6，温度0；PA7，温度1
	#endif
  * @}
  */ 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //add by yanly //PB0: temperature 0, PB1: temperature 1
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	
/** @defmodify modify by yanly for remove voltage check
  * @{
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
  * @}
  */ 
	
	GPIO_Init(GPIOB, &GPIO_InitStructure); //add by yanly for [2]
	
/** @defmodify modify by yanly for remove voltage check
  * @{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
  * @}
  */ 	
  
	ADC_DeInit();
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  ADC_CommonInit(&ADC_CommonInitStructure);//初始化
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //ENABLE;//DISABLE; //非连续转换，单次转换，每次获取前要保证有开启ADC转换：ADC_SoftwareStartConv(ADC1); 每调用一次转换，只转换一次，dma缓冲区只有一处发生数据变化。
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = ADC_BUFFER_SIZE/10; 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
	
/** @defmodify modify by yanly for remove voltage check
  * @{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_480Cycles);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_480Cycles);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15,3, ADC_SampleTime_480Cycles);

#if defined _TEMPTURE_ADC_
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_480Cycles);	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 5, ADC_SampleTime_480Cycles);	
#endif	
  * @}
  */ 	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_480Cycles);	//add by yanly for [2]
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_480Cycles);	
	
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);//开启ADC1转换器
}
/**
  * @brief  ADC1_get_value.
  * @param  read_type
  * @retval sum
  */
uint16_t ADC1_get_value(uint8_t read_type)
{
	uint8_t count = 0;//计数10次
	uint32_t sum = 0;	 //求和
	
	for (count = 0; count < ADC_CHECK_SUM; count++)
	{
		sum += adc1_rx_buffer[count*(ADC_USE_CHANNEL)+read_type];
	}
	sum /= 10;
//	ZKRT_LOG(LOG_NOTICE, "sum= %d \r\n",sum);
	switch (read_type)
	{
		case _25V_value:
			sum = sum*67925/9216;
		break;
		case _5V_value:
			sum = sum*715/512;
		break;
		case _5A_value:
			sum = (sum*4125)/3328;
		break;
		case _T1_value:
		case _T2_value:
#ifdef ADCTEMP_VENDER_2
			sum=((((float)sum*(3.3/(4096))-0.6)/0.0025)-2.5)*10;
#endif
#ifdef ADCTEMP_VENDER_1
			sum = sum*1650/1024; //why? yanly
#endif		
		break;
		default: break;
	}
	return sum;
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
