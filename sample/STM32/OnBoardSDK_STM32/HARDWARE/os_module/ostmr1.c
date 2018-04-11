/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    2014/10/31
  * @brief   //timerX init,interrupt ,this is timer4, base 1ms, hwtmr2 is no need care just name
  ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ostmr1.h"



static volatile vfp_t _qTask;
static volatile vfp_t _fTask;

static volatile uint16_t _sysTimerCnt10ms;

//void TIM2_IRQHandler(void);
static void TimerInit_2nd(void);
static void TIM_NVIC_Configuration_2nd(void);

void hwtmr2_enable(void)
{
  TIM_Cmd(QTTC_TIM_NUM, ENABLE);  
}

void hwtmr2_disable(void)
{
  TIM_Cmd(QTTC_TIM_NUM, DISABLE); 
}

void hwtmr2_init(void)
{
  _qTask= NULL;    
  _fTask = NULL;
  _sysTimerCnt10ms = 0;
}
#ifdef __cplusplus  //zkrt_test
extern "C" {
#endif //__cplusplus
//void hwtmr2_irqHandler(void)
void TIM8_TRG_COM_TIM14_IRQHandler()  
{  
	if (TIM_GetITStatus(QTTC_TIM_NUM, TIM_IT_Update) != RESET)
	{
		_sysTimerCnt10ms++;
		/* 1ms task */
		(*_qTask)();
		if(_sysTimerCnt10ms >= 10)
		{
			(*_fTask)();
			_sysTimerCnt10ms = 0;
		}
		TIM_ClearITPendingBit(QTTC_TIM_NUM, TIM_IT_Update);
	}	
}
#ifdef __cplusplus
}
#endif //__cplusplus

bool_t b_hwtmr2_setting( vfp_t quick, vfp_t fast)
{
	_qTask= quick;
	_fTask = fast;

	/* default system timer settings */
	TimerInit_2nd(); // base:1ms , Timer2
	TIM_NVIC_Configuration_2nd();

	return (TRUE);
}

static void TimerInit_2nd(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(QTTC_RCC_CLK, ENABLE);
  
  /* Time base configuration */  //这里定时器的时钟频率为84Mhz
	
  TIM_TimeBaseStructure.TIM_Period = (1000 - 1); //1ms base 
 
  TIM_TimeBaseStructure.TIM_Prescaler = (QTTC_TIMER_CLK - 1); 

  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(QTTC_TIM_NUM, &TIM_TimeBaseStructure);

  TIM_ClearITPendingBit(QTTC_TIM_NUM, TIM_IT_Update);

  /* TIM IT enable */
  TIM_ITConfig(QTTC_TIM_NUM, TIM_IT_Update, ENABLE);

  /* TIM5 enable counter */
  TIM_Cmd(QTTC_TIM_NUM, ENABLE);  
}
/*******************************************************************************
* Function Name  : TIM_NVIC_Configuration
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void TIM_NVIC_Configuration_2nd(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ_CHANNEL_2ND;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIMER_PreemptionPriority_2ND;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIMER_SubPriority_2ND;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
  
}
