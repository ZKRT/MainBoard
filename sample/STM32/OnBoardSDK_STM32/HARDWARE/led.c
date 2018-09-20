/**
  ******************************************************************************
  * @file    led.c
  * @author  ZKRT
  * @version V2.0
  * @date    2017-08-30
  * @brief   led
  *           + (1)...
								PC0��RUN
								PC1��ALRM
								PC2��H/S
								PD0��USART1_TX
								PD1��USART1_RX
								PD2��USART2_TX
								PD3��USART2_RX
								PD4��CAN1_RX
								PD5��CAN1_TX

								PA8����Ӧ5V��ѹ��ʹ�ܡ�0�رգ�1ʹ��
								PE2����Ӧ����ģ��1�Ĺ���ʹ�ܡ�0�رգ�1ʹ��
								PE9����Ӧ����ģ��2�Ĺ���ʹ�ܡ�0�رգ�1ʹ��

  *           + (2)�޸�LED����; ����5V��ѹ��ʹ�ܡ�����ģ��1�Ĺ���ʹ�ܺͱ���ģ��2��
	*             ����ʹ�ܡ�	--161214 by yanly
								PE4��RUN
								PE5��ALRM
								PE6��H/S
								PC13��USART1_TX
								PC12��USART1_RX
								PC14��USART6_TX
								PC15��USART6_RX
								PA5��CAN1_RX
								PA4��CAN1_TX
  *           + (3)�޸�LED����;  --170306 by yanly
								PF11��RUN
								PB15��ALRM
								PB14��H/S
								PG8��USART1_TX
								PC12��USART1_RX
								PA15��USART6_TX
								PD6��USART6_RX
								PG6��CAN1_RX
								PG7��CAN1_TX	
								
  *           + (4)�޸�LED����;  --170830 by yanly
								PF11��RUN
								PB15��ALRM
								PB14��H/S
								PG8��USART1_TX
								PC12��USART1_RX
								PD8��USART6_TX
								PD7��USART6_RX
								PG6��CAN1_RX
								PG7��CAN1_TX									
  *         
 ===============================================================================
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */

/* Includes ------------------------------------------------------------------*/
#include "led.h"
#include "ostmr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char led_none;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  led_mstask.
  * @param  None
  * @retval None
  */
void led_mstask(void)
{
}
/**
  * @brief  led_init.
  * @param  None
  * @retval None
  */
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	delay_ms(500);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_SetBits(GPIOD, GPIO_Pin_7 | GPIO_Pin_8);
	GPIO_SetBits(GPIOF, GPIO_Pin_11);
	GPIO_SetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);

	//	t_ostmr_insertTask();
}

/**
  * @brief  led_test. ����LED��һ����˸һ�Σ�����led
  * @param  None
  * @retval None
  */
void led_test(void)
{
	char i = 0;
	//all high
	//	_RUN_LED = 1;
	//	_ALARM_LED = 1;
	//	_HS_LED = 1;
	//	_FLIGHT_UART_TX_LED = 1;
	//	_FLIGHT_UART_RX_LED = 1;
	//	_OBSTACLE_AVOIDANCE_TX_LED = 1;
	//	_OBSTACLE_AVOIDANCE_RX_LED = 1;
	//	_CAN_RX_LED = 1;
	//	_CAN_TX_LED = 1;
	//all low
	//	_RUN_LED = 0;
	//	_ALARM_LED = 0;
	//	_HS_LED = 0;
	//	_FLIGHT_UART_TX_LED = 0;
	//	_FLIGHT_UART_RX_LED = 0;
	//	_OBSTACLE_AVOIDANCE_TX_LED = 0;
	//	_OBSTACLE_AVOIDANCE_RX_LED = 0;
	//	_CAN_RX_LED = 0;
	//	_CAN_TX_LED = 0;
	while (i < 30)
	{
		i++;
		delay_ms(500);
		_RUN_LED = !_RUN_LED;
		_ALARM_LED = !_ALARM_LED;
		_HS_LED = !_HS_LED;
		_FLIGHT_UART_TX_LED = !_FLIGHT_UART_TX_LED;
		_FLIGHT_UART_RX_LED = !_FLIGHT_UART_RX_LED;
		_OBSTACLE_AVOIDANCE_TX_LED = !_OBSTACLE_AVOIDANCE_TX_LED;
		_OBSTACLE_AVOIDANCE_RX_LED = !_OBSTACLE_AVOIDANCE_RX_LED;
		_CAN_RX_LED = !_CAN_RX_LED;
		_CAN_TX_LED = !_CAN_TX_LED;
		PFout(9) = !PFout(9);
		PFout(10) = !PFout(10);
	}
	GPIO_ResetBits(GPIOF, GPIO_Pin_10 | GPIO_Pin_9);
	_RUN_LED = 1;
	_ALARM_LED = 1;
	_HS_LED = 1;
	_FLIGHT_UART_TX_LED = 1;
	_FLIGHT_UART_RX_LED = 1;
	_OBSTACLE_AVOIDANCE_TX_LED = 1;
	_OBSTACLE_AVOIDANCE_RX_LED = 1;
	_CAN_RX_LED = 1;
	_CAN_TX_LED = 1;
}
/**
  * @brief  led_process. led�ƿ���
  * @param  None
  * @retval None
  */
void led_process(void)
{
	if (MAVLINK_TX_INIT_VAL - TimingDelay >= 5000) //5s��ʱ��֮��һֱ���������߼�
	{
		if ((mavlink_send_flag - TimingDelay) >= 800)
		{
			mavlink_send_flag = TimingDelay;
		}
	}
	//run led control
	if ((mavlink_send_flag - TimingDelay) < 50) //�뷢������������һ�����mavlink_send_flag������������������mavlink_send_flag
	{
		_RUN_LED = 0;
	}
	else if ((mavlink_send_flag - TimingDelay) < 800)
	{
		_RUN_LED = 1;
	}
	//flight led control
	if ((usart1_tx_flag - TimingDelay) > 50)
	{
		_FLIGHT_UART_TX_LED = 1;
	}
	if ((usart1_rx_flag - TimingDelay) > 50)
	{
		_FLIGHT_UART_RX_LED = 1;
	}
	//can led control
	if ((can_rx_flag - TimingDelay) > 50)
	{
		_CAN_RX_LED = 1;
	}
	if ((can_tx_flag - TimingDelay) > 50)
	{
		_CAN_TX_LED = 1;
	}
	//433m led control
	if ((obstacle_avoidance_tx_flag - TimingDelay) > 50)
	{
		_OBSTACLE_AVOIDANCE_TX_LED = 1;
	}
	if ((obstacle_avoidance_rx_flag - TimingDelay) > 50)
	{
		_OBSTACLE_AVOIDANCE_RX_LED = 1;
	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
