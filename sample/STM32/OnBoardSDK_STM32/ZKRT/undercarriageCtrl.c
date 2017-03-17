/**
  ******************************************************************************
  * @file    undercarriageCtrl.c 
  * @author  ZKRT
  * @version V1.0.0
  * @date    15-March-2017
  * @brief   undercarriageCtrl program body
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "undercarriageCtrl.h"
#include "ostmr.h"
#include "osqtmr.h"	

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void undercarriage_timer_task(void);
/* Private variables ---------------------------------------------------------*/
undercarriage_st undercarriage_data = {downed_udcaie_rs, 0, 0, downed_udcaie_rs};

/* Private functions ---------------------------------------------------------*/

/**
*   @brief  undercarriage_init
  * @parm   none
  * @retval none
  */
void undercarriage_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA| RCC_AHB1Periph_GPIOF, ENABLE);
	
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	delay_ms(200);										
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOF, &GPIO_InitStructure);		  
	
  UDCAIE_LEFT_STOP;
	UDCAIE_RIGHT_STOP;
	
//	GPIO_SetBits(GPIOA, GPIO_Pin_0);
//	GPIO_SetBits(GPIOF, GPIO_Pin_9 | GPIO_Pin_7 | GPIO_Pin_8);	//zkrt_debug
	
	t_ostmr_insertTask(undercarriage_timer_task, 1000, OSTMR_PERIODIC);
	
//	undercarriage_data.state_bya3height = downed_udcaie_rs;	//zkrt_debug
//	undercarriage_data.run_state = uped_udcaie_rs;	//zkrt_debug
}
/**
*   @brief  undercarriage_process
  * @parm   none
  * @retval none
  */
void undercarriage_process(void)
{
	if(undercarriage_data.state_bya3height + undercarriage_data.run_state == 1)  //‘ –Ì±‰ªØ
	{
		undercarriage_data.run_state = undercarriage_data.state_bya3height -1;
		undercarriage_data.run_timeout = UDCAIE_CHANGE_TIME;
		ZKRT_LOG(LOG_INOTICE, "undercarriage state changed\r\n"); 
	}
	
	switch(undercarriage_data.run_state)
	{
		case downed_udcaie_rs:
			break;
		case downing_udcaie_rs:
			if(undercarriage_data.run_timeoutflag)
			{
				undercarriage_data.run_timeoutflag = 0;
				UDCAIE_LEFT_STOP;
				UDCAIE_RIGHT_STOP;
				undercarriage_data.run_state = downed_udcaie_rs;
				ZKRT_LOG(LOG_INOTICE, "undercarriage downing over\n"); 
			}	
			else
			{
			  UDCAIE_LEFT_DOWN;
			  UDCAIE_RIGHT_DOWN;				
			}	
			break;
		case uping_udcaie_rs:
			if(undercarriage_data.run_timeoutflag)
			{
				undercarriage_data.run_timeoutflag = 0;
				UDCAIE_LEFT_STOP;
				UDCAIE_RIGHT_STOP;
				undercarriage_data.run_state = uped_udcaie_rs;
				ZKRT_LOG(LOG_INOTICE, "undercarriage upping over\n"); 
			}	
			else
			{
		    UDCAIE_LEFT_UP;
			  UDCAIE_RIGHT_UP;	
			}				
			break;
		case uped_udcaie_rs:
			break;
    default:break;
	}
}	
/**
*   @brief  undercarriage_timer_task
  * @parm   none
  * @retval none
  */
void undercarriage_timer_task(void)
{
	if(undercarriage_data.run_timeout)
	{	
		undercarriage_data.run_timeout--;
		if(!undercarriage_data.run_timeout)
			undercarriage_data.run_timeoutflag = 1;
	}
}


/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
