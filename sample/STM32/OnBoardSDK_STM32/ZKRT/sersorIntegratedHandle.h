/**
  ******************************************************************************
  * @file    sersorIntegratedHandle.h
  * @author  ZKRT
  * @version V1.0
  * @date    08-March-2017
  * @brief
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SERSORINTEGRATEDHANDLE_H
#define __SERSORINTEGRATEDHANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define SI_START_CODE                     0xEB
#define SI_END_CODE                       0xBE

#define SI_CANUP_DATALEN                  0x18

#define SI_HEARTBEAT_CMD                  0x12
/* Exported typedef ------------------------------------------------------------*/
typedef struct {
  uint16_t body_T;
  uint16_t m1_T;
  uint16_t m2_T;
  uint16_t m3_T;
  uint16_t m4_T;
  uint16_t left_T;
  uint16_t right_T;
} si_temp_data_st;
#pragma pack(push, 1)
typedef struct {
  uint16_t reserved;
  uint16_t front_D;
  uint16_t right_D;
  uint16_t back_D;
  uint16_t left_D;
  uint16_t body_T;
  uint16_t m1_T;
  uint16_t m2_T;
  uint16_t m3_T;
  uint16_t m4_T;
  uint16_t left_T;
  uint16_t right_T;
} si_data_st;
typedef struct {
  uint8_t start_code;
  uint8_t cmd_code;
  uint8_t datalen;
  uint8_t data[sizeof(si_data_st)];
  uint16_t crc;
  uint8_t end_code;
} sensorIntegratedInfo_st;
#pragma pack(pop)

/* Exported functions ------------------------------------------------------- */
void app_sersor_integrated_prcs(void);

#endif /* __SERSORINTEGRATEDHANDLE_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
