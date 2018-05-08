/**
  ******************************************************************************
  * @file    dev_handle.h
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEV_HANDLE_H
#define __DEV_HANDLE_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appprotocol.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
typedef zd_heartv3_3_st* dev_inof_sp;
//zkrt device info struct
typedef struct
{
	dev_inof_sp devself;
	short temperature1;           //�¶�1
	short temperature2;           //�¶�2
	short temperature_low;        //�¶�����
	short temperature_high;       //�¶�����
	u8 status_t1;                 //�¶�1״̬
	u8 status_t2;                 //�¶�2״̬
} zk_dev_info_st;
/* Exported functions ------------------------------------------------------- */
extern zk_dev_info_st zkrt_devinfo;

void appdev_init(void);
void appdev_prcs(void);
#endif /* __DEV_HANDLE_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

