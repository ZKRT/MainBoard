/**
  ******************************************************************************
  * @file    appcan.h
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
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
#ifndef __APPCAN_H
#define __APPCAN_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//can message handle
typedef struct
{
    u8 dev_support_en[DEVICE_NUMBER];  //0-disable, 1-enable  «∑Ò÷ß≥÷
//

} can_msg_handle_st;


/* Exported functions ------------------------------------------------------- */
void appcan_init(void);
void appcan_prcs(void);
#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

