/**
  ******************************************************************************
  * @file    guorui.h 
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
#ifndef __GUORUI_H
#define __GUORUI_H 
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appprotocol.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//guorui gas type
typedef enum
{
	none_grst=0,
	co_grst=1,
	h2s_grst,
	o2_grst,
	ex_grst,
	no2_grst,
	no_grst,
	so2_grst,
	cl2_grst,
	nh3_grst,
	h2_grst,
	ph3_grst,
	ch2o_grst,
	o3_grst,
	f2_grst,
	hf_grst,
	hcl_grst,
	hbr_grst,
	c2h4o_grst,
	cocl2_grst,
	sih4_grst,
	clo2_grst,
	co2_grst,
	sf6_grst,
	voc_grst,
	ch4_grst,
	max_grst
}gr_gas_sensortype;

//guorui unit type
typedef enum
{
	PPM_grunit=0,
	PPB_grunit,
	PPM2_grunit,
	permillage_grunit,
	percent_grunit,	
	LELpercent_grunit,
	VOLpercent_grunit,
	mgdiviedm3_gruint,
	mgdiviedL_grunit,
	max_grunit
}gr_gas_unite;


struct gr_prcs_handles
{
	u8 gas_online_flag;        
	u8 gas_update_flag;  
};
/* Exported functions ------------------------------------------------------- */

typedef hb_gas_st guorui_gas_valuest;

extern guorui_gas_valuest guorui_gas_info;
extern struct gr_prcs_handles gr_handle;
#endif /* __APPCAN_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

