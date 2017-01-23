#include "timer_zkrt.h"

#ifdef USE_LWIP_FUN	
extern u32 lwip_localtime;
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
void lwip_timer_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_TIMER_LWIP,ENABLE); 
	
	TIM_TimeBaseInitStructure.TIM_Prescaler= 839;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period= 999;   //�Զ���װ��ֵ //���������Ϊ10ms����
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIMER_CHANNEL_LWIP,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIMER_CHANNEL_LWIP,TIM_IT_Update,ENABLE); //����ʱ�������ж�
	TIM_Cmd(TIMER_CHANNEL_LWIP,ENABLE); //ʹ�ܶ�ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel=TIMER_IRQ_CHANNEL_LWIP; //��ʱ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVIC_TIMER_PreemptionPriority_LWIP; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVIC_TIMER_SubPriority_LWIP;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//��ʱ���жϺ���
void LWIP_TIMER_Handler(void)
{
	if(TIM_GetITStatus(TIMER_CHANNEL_LWIP,TIM_IT_Update)==SET) //����ж�
	{
		lwip_localtime +=10; //��10
	}
	TIM_ClearITPendingBit(TIMER_CHANNEL_LWIP,TIM_IT_Update);  //����жϱ�־λ
}
//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	LWIP_TIMER_Handler();
}
#endif

//ͨ�ö�ʱ��4�жϳ�ʼ��
void TIM4_Int_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = 200-1; 	
	TIM_TimeBaseInitStructure.TIM_Prescaler=8400-1;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_Cmd(TIM4,ENABLE); 
}
