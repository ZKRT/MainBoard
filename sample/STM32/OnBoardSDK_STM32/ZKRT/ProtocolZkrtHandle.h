/**
  ******************************************************************************
  * @file    ProtocolZkrtHandle.h
  * @author  ZKRT
  * @version V1.0.0
  * @date    25-March-2017
  * @brief   Header for ProtocolZkrtHandle.c module
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTOCOLZKRTHANDLE_H
#define __PROTOCOLZKRTHANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/**
*   @brief  mainboard self msg to handle someting by protocol from mobile
  * @param1 u8* data,
  * @retval none
  */
typedef void (*MB_SELF_MSG_FUN)(u8*);


/* Exported constants --------------------------------------------------------*/
//mainboard handle num
#define OSTABCLE_MBDH_NUM                     0x01
#define UNDERCARRIAGE_MBDH_NUM                0x02
#define MAX_MBDH_NUM                          UNDERCARRIAGE_MBDH_NUM

/* Exported functions ------------------------------------------------------- */

extern const MB_SELF_MSG_FUN mb_self_handle_fun[MAX_MBDH_NUM];

#endif /* __PROTOCOLZKRTHANDLE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
