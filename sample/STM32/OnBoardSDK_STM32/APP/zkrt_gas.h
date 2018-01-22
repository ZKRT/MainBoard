/**
  ******************************************************************************
  * @file    zkrt_gas.h 
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
#ifndef __ZKRT_GAS_H
#define __ZKRT_GAS_H 
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
///////////////////////////////////////////////////////////
//zkrt gas type
typedef enum
{
	none_zkst=0,
	co_zkst=1,
	h2s_zkst,
	nh3_zkst,	
	co2_zkst,	
	cl2_zkst,
	so2_zkst,
	voc_zkst,
	ch4_zkst,
	o2_zkst,
	max_zkst
}zkrt_gas_sensortype;

//zkrt unit type
typedef enum
{
	umoldiviedmol_zkunit=0,
	VOLpercent_zkunit,
	LELpercent_zkunit,
	PPM_zkunit,
	mgdivied10cube_zkunit,
	percent_zkunit,
	tenpowernegative6_zkunit,
	PPB_zkunit,
	permillage_zkunit,
	mgdiviedm3_zkuint,
	mgdiviedL_zkunit,
	max_zkunit
}zkrt_gas_unite;

/* Exported functions ------------------------------------------------------- */
#define GASTYPE_MAP_BSA_INDEX  0
#define GASTYPE_MAP_GR_INDEX   1

u8 manu_gastype2zkrt(u8 manu, u8 type);
u8 manu_gasunittype2zkrt(u8 manu, u8 type);

#endif /* __APPCAN_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

