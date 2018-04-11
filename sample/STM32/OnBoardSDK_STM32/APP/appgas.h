/**
  ******************************************************************************
  * @file    appgas.h
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief
	*					 + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPGAS_H
#define __APPGAS_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void appgas_init(void);
void appgas_prcs(void);
u8 appgas_message_handle(zkrt_packet_t* recvpack, u8* respond_data, u8* reslen);
#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

