/**
  ******************************************************************************
  * @file    hwTestHandle.h
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   Header for hwTestHandle.c module
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HWTESTHANDLE_H
#define __HWTESTHANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/** @defgroup
  * @{
  */


/**
  * @}
  */
/* Exported constants --------------------------------------------------------*/

/** @defgroup hardware test protocol constants
  * @{
  */
//start code
#define STARTCODE_HWT                    0xEBEBEB
//end code
#define ENDCODE_HWT                      0xFEFEFE
//cmd num
#define LED_CMDN_HWT                     0x01
#define UART_CMDN_HWT                    0x02
#define CAN_CMDN_HWT                     0x03
#define PWM_CMDN_HWT                     0x04
#define AD_CMDN_HWT                      0x05
#define DS18B20_CMDN_HWT                 0x06
#define IIC_CMDN_HWT                     0x07
#define USB_CMDN_HWT                     0x08
#define SBUS_CMDN_HWT                    0x09
#define FLASH_CMDN_HWT                   0x0a
#define EXTERNAL_CMDN_HWT                0x0b
#define MAX_CMDN_HWT                     EXTERNAL_CMDN_HWT
//cmd type
#define SET_CMDT_HWT                     0x01
#define RESPOND_CMDT_HWT                 0x02
//test result
#define RESULT_OK_HWT                    0x55
#define RESULT_FAIL_HWT                  0xAA
#define RESULT_UNCONFIRM_HWT             0xCC
#define RESULT_FAIL_SUBM_TIMEOUT_HWT     0xFF

//dev name
#define MAINBOARD_NAME                   "Controller"  //the name length must < 16 bytes
/**
  * @}
  */


/* Exported functions ------------------------------------------------------- */

void hwtest_init(void);
u8 hwtest_data_handle(u8 *buf, u16 buflen, u8 *resbuf, u16 *resbuflen);

#endif /* __HWTESTHANDLE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
