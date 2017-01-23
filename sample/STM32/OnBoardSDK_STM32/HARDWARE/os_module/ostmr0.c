/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version  
  * @date    
  * @brief   //timerX init，interrupt ,this is timer3,hwtmr0 is no need care just name
						 供ostmr.c调用,定时器初始化、中断的底层驱动
  ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "ostmr0.h"


#define	SYSTEM_TIMER_BASE_VALUE	5000


static volatile u16 _sysTimerCnt50Ms;        /* system timer counter in ms unit */
static volatile u16 _sysTimerCntSec;         /* system timer counter in sec unit */
static volatile u16 _taskSeparator5ms;       /* to separate 10ms tasks into 5ms period */
static volatile u16 _taskSeparator50ms;      /* to separate 100ms tasks into 5ms period */
static volatile u16 _taskSeparator500ms;     /* to separate 1000ms tasks into 5ms period */

static volatile vfp_t _fastTask[NUM_OF_HW_TMR_SEPARATOR], _slowTask[NUM_OF_HW_TMR_SEPARATOR], _secTask[NUM_OF_HW_TMR_SEPARATOR];

//static void system_timer_init();
//void TIM3_IRQHandler(void);


void hwtmr0_enable(void) 
{
	TIM_Cmd(TIMER_CHANNEL_1ST, ENABLE);  
}

void hwtmr0_disable(void)
{
	TIM_Cmd(TIMER_CHANNEL_1ST, DISABLE); 
}
void hwtmr0_init(void)
{
  u32        __i;
   
  _taskSeparator5ms   = TRUE_P;
  _taskSeparator50ms  = TRUE_P;
  _taskSeparator500ms = TRUE_P; 
  _sysTimerCnt50Ms    = 0;
  _sysTimerCntSec     = 0;   
  
  for(__i =0; __i<NUM_OF_HW_TMR_SEPARATOR; __i++)
  { 
    _fastTask[__i] = NULL;
    _slowTask[__i] = NULL;
    _secTask[__i]  = NULL;
  }
}


//void hwtmr0_irqHandler()
void TIM8_BRK_TIM12_IRQHandler()
{  
	if (TIM_GetITStatus(TIMER_CHANNEL_1ST, TIM_IT_Update) != RESET)
	{
		_sysTimerCnt50Ms++;
		_sysTimerCntSec++; 
		
		/* 10ms task */
		if(_fastTask[_taskSeparator5ms] != NULL)
		{
			(*_fastTask[_taskSeparator5ms])();
			/* alter _taskSeparator bit */
			_taskSeparator5ms = (~_taskSeparator5ms) & 0x00000001;
		}
		
		/* 100ms (normal) task */
		if(_sysTimerCnt50Ms >= 10)
		{
			if(_slowTask[_taskSeparator50ms] != NULL)
			{            
				(*_slowTask[_taskSeparator50ms])();
			}
			_sysTimerCnt50Ms = 0;            
			_taskSeparator50ms = (~_taskSeparator50ms) & 0x00000001;   
		}
		
		/* 1sec (slow) task */
		if(_sysTimerCntSec >= 100)
		{
			if(_secTask[_taskSeparator500ms] != NULL)
			{
				(*_secTask[_taskSeparator500ms])();
			} 
			_sysTimerCntSec = 0;
			_taskSeparator500ms = (~_taskSeparator500ms) & 0x00000001;    
		} 
		TIM_ClearITPendingBit(TIMER_CHANNEL_1ST, TIM_IT_Update);
	}	
}

static void TimerInit_1st(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_TIMER_1ST, ENABLE);

  /* Time base configuration */ //5ms base //这里定时器的时钟为84Mhz

  TIM_TimeBaseStructure.TIM_Period = (5000 - 1); 
	
  TIM_TimeBaseStructure.TIM_Prescaler = (APB1_TIMER_CLK - 1);
	
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;

  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIMER_CHANNEL_1ST, &TIM_TimeBaseStructure);

//  TIM_ClearITPendingBit(TIMER_CHANNEL_1ST, TIM_IT_Update);

  /* TIM IT enable */
  TIM_ITConfig(TIMER_CHANNEL_1ST, TIM_IT_Update, ENABLE);

  /* TIM5 enable counter */
  TIM_Cmd(TIMER_CHANNEL_1ST, ENABLE);  
}
/*******************************************************************************
* Function Name  : TIM_NVIC_Configuration
* Description    : Configures the used IRQ Channels and sets their priority.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_NVIC_Configuration_1st(void)
{ 
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Set the Vector Table base address at 0x08000000 */
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);

  /* Enable the TIM5 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIMER_IRQ_CHANNEL_1ST;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_TIMER_PreemptionPriority_1ST;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_TIMER_SubPriority_1ST;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
  
}
bool_t b_hwtmr0_setting( vfp_t fast[], vfp_t slow[], vfp_t sec[])
{
	u32            __i;
	for(__i =0; __i<NUM_OF_HW_TMR_SEPARATOR; __i++)
	{
		_fastTask[__i] = fast[__i];
		_slowTask[__i] = slow[__i];
		_secTask[__i]  = sec[__i];
	}

	/* default system timer settings */
	/* timer 0 interrupt period is 10ms */
	//hwvic_irqInstaller(TIMER0_INT, hwtmr0_irqHandler, PRIORITY_MIDDLE);
	//����timer funciton
	//system_timer_init();
	
  TimerInit_1st(); //5ms 基准 Timer3
  TIM_NVIC_Configuration_1st();
	

	return (TRUE_P);
 
}


