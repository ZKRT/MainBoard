#include "hw_usart.h"

USART_TypeDef* COM_USART[COMn] = {EVAL_COM1, EVAL_COM4, EVAL_COM5}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM4_TX_GPIO_PORT, EVAL_COM5_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM4_RX_GPIO_PORT, EVAL_COM5_RX_GPIO_PORT};
 
const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK, EVAL_COM4_CLK, EVAL_COM5_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK, EVAL_COM4_TX_GPIO_CLK, EVAL_COM5_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK, EVAL_COM4_RX_GPIO_CLK, EVAL_COM5_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN, EVAL_COM4_TX_PIN, EVAL_COM5_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN, EVAL_COM4_RX_PIN, EVAL_COM5_RX_PIN};		

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:    
  *     @arg COM1
  *     @arg COM2  
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

	switch(COM)
	{
		case COM1:
			RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
			break;
		
		case COM4:
			RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
			break;
		
		case COM5:
			RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
			break;
		
		default:
//				        //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
//    /* Enable the USART2 Pins Software Remapping */
//		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);	
			break;
	}	

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}
