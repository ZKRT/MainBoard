/**
  ******************************************************************************
  * @file    lwipapp.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   lwipapp modules
  *          + (1) 
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sys.h" 
#include "lwip_comm.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "lwipapp.h"
 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  LWIP 应用主流程
  * @param  None
  * @retval None
  */
void lwip_prcs(void)
{
	if((lwipdev.netstatus != NET_INIT_OK)&&(lwipdev.linkstatus == LINK_UP)) //网络没有初始化成功，并且网线有连接
	{
		lwip_comm_init();
		return;
	}
	//zkrt_todo: 网络事件业务处理
	if((lwipdev.netstatus == NET_INIT_OK)&&
		(lwipdev.linkstatus == LINK_UP)&&
		((lwipdev.dhcpstatus==2)||(lwipdev.dhcpstatus!=0xff)))
	{
		//tcp_client_prcs();
		tcp_server_prcs();			
	}
	
#ifdef HWTEST_FUN			
	tcp_server_hwt_led_control();
#endif	

	lwip_periodic_handle(); //固定调用
}
/**
  * @brief  LWIP 流程初始化
  * @param  None
  * @retval NOne
  */
void lwip_prcs_init(void)
{
//	tcp_client_para_init();
	tcp_server_para_init();
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
