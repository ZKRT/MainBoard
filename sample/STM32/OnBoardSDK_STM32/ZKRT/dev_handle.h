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
	short temperature1;           //温度1
	short temperature2;           //温度2
	short temperature_low;        //温度下限
	short temperature_high;       //温度上限
	u8 status_t1;                 //温度1状态
	u8 status_t2;                 //温度2状态
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

