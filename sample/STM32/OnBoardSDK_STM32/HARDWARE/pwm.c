/**
  ******************************************************************************
  * @file    pwm.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   pwm module
  *          + (1) Use PWM_T10CH1(PF6), PWM1_T11_CH1(PF7), PWM2_T13_CH1(PF8), 
	*            PWM3_T4_CH2(PB7), PWM4_T3_CH3(PC8)  --by yanly
  *          + (2) �޸��������ã�����4��PWM_T10_CH1>>PB3, PWM1_T13_CH1>>PF8, 
	*            PWM2_T5_CH1>>PA0, PWM3_T11_CH1>>PF7, PWM4_T14_CH1>>PF9 --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pwm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  PWM��ʼ�� 
  * @param  None
  * @retval None
  */
void pwm_tim_init(void)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//ʱ��ʹ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13|RCC_APB1Periph_TIM14|RCC_APB1Periph_TIM5 ,ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOA, ENABLE); 
	
	//GPIO����
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_TIM11); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); 
	
	//GPIO��ʼ��
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//TIMER INIT
	TIM_TimeBaseStructure.TIM_CounterMode= TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
//	TIM_TimeBaseStructure.TIM_Period=PWM_T_PERIOD;   //�Զ���װ��ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler = PWM_T_PSC;  //��ʱ����Ƶ
//	TIM_TimeBaseInit(PWM_TIM,&TIM_TimeBaseStructure);//��ʼ����ʱ��
	
	TIM_TimeBaseStructure.TIM_Period=PWM1_T_PERIOD;   
	TIM_TimeBaseStructure.TIM_Prescaler = PWM1_T_PSC;  
	TIM_TimeBaseInit(PWM1_TIM,&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period=PWM2_T_PERIOD;   
	TIM_TimeBaseStructure.TIM_Prescaler = PWM2_T_PSC;  
	TIM_TimeBaseInit(PWM2_TIM,&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period=PWM3_T_PERIOD;  
	TIM_TimeBaseStructure.TIM_Prescaler = PWM3_T_PSC; 
	TIM_TimeBaseInit(PWM3_TIM,&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period=PWM4_T_PERIOD;  
	TIM_TimeBaseStructure.TIM_Prescaler = PWM4_T_PSC; 
	TIM_TimeBaseInit(PWM4_TIM,&TIM_TimeBaseStructure);	
	
	//��ʼ��TIM Channe PWMģʽ 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	
//	TIM_OC1Init(PWM_TIM, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC1Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC1Init(PWM2_TIM, &TIM_OCInitStructure);
	TIM_OC1Init(PWM3_TIM, &TIM_OCInitStructure); 
	TIM_OC1Init(PWM4_TIM, &TIM_OCInitStructure);
	
//	TIM_OC1PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM3_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM4_TIM, TIM_OCPreload_Enable); 
 
//  TIM_ARRPreloadConfig(PWM_TIM,ENABLE);//ARPEʹ�� 
	TIM_ARRPreloadConfig(PWM1_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM2_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM3_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM4_TIM,ENABLE);
	
//	TIM_Cmd(PWM_TIM, ENABLE);  //ʹ��TIM14
	TIM_Cmd(PWM1_TIM, ENABLE); 
	TIM_Cmd(PWM2_TIM, ENABLE);
	TIM_Cmd(PWM3_TIM, ENABLE);
	TIM_Cmd(PWM4_TIM, ENABLE);	
}

/** @deftest pwm output test
  * @{  
	TIM_SetCompareX(TIMx,1500);	//�޸ıȽ�ֵ���޸�ռ�ձ� 
	while(1) //ʵ�ֱȽ�ֵ��1500~1900��������1900���1900~0�ݼ���ѭ��
	{
		delay_ms(10);	 
		if(dir)
			led0pwmval++;//dir==1 led0pwmval����
		else 
			led0pwmval--;	//dir==0 led0pwmval�ݼ� 
		if(led0pwmval>1900)
			dir=0;//led0pwmval����󣬷���Ϊ�ݼ�
		if(led0pwmval==1100)
			dir=1;	//led0pwmval�ݼ���0�󣬷����Ϊ����
		TIM_SetCompareX(TIMx,led0pwmval);	//�޸ıȽ�ֵ���޸�ռ�ձ�
	}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/


