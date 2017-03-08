/**
  ******************************************************************************
  * @file    pwm.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   pwm module
  *          + (1) Use PWM_T10CH1(PF6), PWM1_T11_CH1(PF7), PWM2_T13_CH1(PF8), 
	*            PWM3_T4_CH2(PB7), PWM4_T3_CH3(PC8)  --by yanly
  *          + (2) 修改引脚配置：串口4的PWM_T10_CH1>>PB3, PWM1_T13_CH1>>PF8, 
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
  * @brief  PWM初始化 
  * @param  None
  * @retval None
  */
void pwm_tim_init(void)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//时钟使能
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13|RCC_APB1Periph_TIM14|RCC_APB1Periph_TIM5 ,ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOA, ENABLE); 
	
	//GPIO复用
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM10); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource7,GPIO_AF_TIM11); 
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); 
	
	//GPIO初始化
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//TIMER INIT
	TIM_TimeBaseStructure.TIM_CounterMode= TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
//	TIM_TimeBaseStructure.TIM_Period=PWM_T_PERIOD;   //自动重装载值
//	TIM_TimeBaseStructure.TIM_Prescaler = PWM_T_PSC;  //定时器分频
//	TIM_TimeBaseInit(PWM_TIM,&TIM_TimeBaseStructure);//初始化定时器
	
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
	
	//初始化TIM Channe PWM模式 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
//	TIM_OC1Init(PWM_TIM, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1Init(PWM1_TIM, &TIM_OCInitStructure);
	TIM_OC1Init(PWM2_TIM, &TIM_OCInitStructure);
	TIM_OC1Init(PWM3_TIM, &TIM_OCInitStructure); 
	TIM_OC1Init(PWM4_TIM, &TIM_OCInitStructure);
	
//	TIM_OC1PreloadConfig(PWM_TIM, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
	TIM_OC1PreloadConfig(PWM1_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM2_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM3_TIM, TIM_OCPreload_Enable); 
	TIM_OC1PreloadConfig(PWM4_TIM, TIM_OCPreload_Enable); 
 
//  TIM_ARRPreloadConfig(PWM_TIM,ENABLE);//ARPE使能 
	TIM_ARRPreloadConfig(PWM1_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM2_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM3_TIM,ENABLE);
	TIM_ARRPreloadConfig(PWM4_TIM,ENABLE);
	
//	TIM_Cmd(PWM_TIM, ENABLE);  //使能TIM14
	TIM_Cmd(PWM1_TIM, ENABLE); 
	TIM_Cmd(PWM2_TIM, ENABLE);
	TIM_Cmd(PWM3_TIM, ENABLE);
	TIM_Cmd(PWM4_TIM, ENABLE);	
}

/** @deftest pwm output test
  * @{  
	TIM_SetCompareX(TIMx,1500);	//修改比较值，修改占空比 
	while(1) //实现比较值从1500~1900递增，到1900后从1900~0递减，循环
	{
		delay_ms(10);	 
		if(dir)
			led0pwmval++;//dir==1 led0pwmval递增
		else 
			led0pwmval--;	//dir==0 led0pwmval递减 
		if(led0pwmval>1900)
			dir=0;//led0pwmval到达后，方向为递减
		if(led0pwmval==1100)
			dir=1;	//led0pwmval递减到0后，方向改为递增
		TIM_SetCompareX(TIMx,led0pwmval);	//修改比较值，修改占空比
	}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/


