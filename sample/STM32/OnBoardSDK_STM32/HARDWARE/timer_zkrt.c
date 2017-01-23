#include "timer_zkrt.h"

#ifdef USE_LWIP_FUN	
extern u32 lwip_localtime;
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void lwip_timer_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_TIMER_LWIP,ENABLE); 
	
	TIM_TimeBaseInitStructure.TIM_Prescaler= 839;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period= 999;   //自动重装载值 //这里计算结果为10ms计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIMER_CHANNEL_LWIP,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIMER_CHANNEL_LWIP,TIM_IT_Update,ENABLE); //允许定时器更新中断
	TIM_Cmd(TIMER_CHANNEL_LWIP,ENABLE); //使能定时器
	
	NVIC_InitStructure.NVIC_IRQChannel=TIMER_IRQ_CHANNEL_LWIP; //定时器中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVIC_TIMER_PreemptionPriority_LWIP; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVIC_TIMER_SubPriority_LWIP;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//定时器中断函数
void LWIP_TIMER_Handler(void)
{
	if(TIM_GetITStatus(TIMER_CHANNEL_LWIP,TIM_IT_Update)==SET) //溢出中断
	{
		lwip_localtime +=10; //加10
	}
	TIM_ClearITPendingBit(TIMER_CHANNEL_LWIP,TIM_IT_Update);  //清除中断标志位
}
//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	LWIP_TIMER_Handler();
}
#endif

//通用定时器4中断初始化
void TIM4_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM4时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 200-1; 	
	TIM_TimeBaseInitStructure.TIM_Prescaler=8400-1;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM4,ENABLE); 
}
