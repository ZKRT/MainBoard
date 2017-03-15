/**
  ******************************************************************************
  * @file    commonzkrt.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for commonzkrt.c module
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMONZKRT_H
#define __COMMONZKRT_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup something
  * @{
  */ 
  
/**
  * @}
  */ 
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void printf_uchar(unsigned char *data, u16 length);
char arry_max_item(const unsigned short *array_data, u16 item_num);

#endif /* __COMMONZKRT_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
