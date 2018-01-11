/**
  ******************************************************************************
  * @file    appgas.c 
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief   
	*					 + (1) init
	*                       
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "appgas.h"
#include "baoshian.h"
#include "osusart.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  appcan_init
  * @param  None
  * @retval None
  */
void appgas_init(void)
{
	baoshian_init();
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void appgas_prcs(void)
{
	baoshian_prcs();
}
