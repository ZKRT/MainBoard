/**
  ******************************************************************************
  * @file    ProtocolZkrtHandle.c 
  * @author  ZKRT
  * @version V1.0.0
  * @date    25-March-2017
  * @brief   ProtocolZkrtHandle program body
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ProtocolZkrtHandle.h"
#include "obstacleAvoid.h"
#include "heartBeatHandle.h"
#include "undercarriageCtrl.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void avoid_osstacle_ptl_hf(u8 *data);
void undercarriage_ptl_hf(u8 *data);
/* Private variables ---------------------------------------------------------*/
//
const MB_SELF_MSG_FUN mb_self_handle_fun[MAX_MBDH_NUM] = {
	avoid_osstacle_ptl_hf,
	undercarriage_ptl_hf
}; 

/* Private functions ---------------------------------------------------------*/

/**
*   @brief  avoid_osstacle_ptl_hf
  * @parm   data
  * @retval none
  */
void avoid_osstacle_ptl_hf(u8 *data)
{
	u16 temp_distse;
	zkrt_heartv2.avoid_ob_enabled = data[0];
	temp_distse = ((data[2])<<8)+(data[1]);
	
	if(temp_distse <2000)
		zkrt_heartv2.avoid_ob_distse = temp_distse;
	
	zkrt_heartv2.avoid_ob_velocity = ((data[4])<<8)+(data[3]);
	GuidanceObstacleData.ob_enabled = zkrt_heartv2.avoid_ob_enabled;
	GuidanceObstacleData.ob_distance = zkrt_heartv2.avoid_ob_distse;
	GuidanceObstacleData.ob_velocity = zkrt_heartv2.avoid_ob_velocity;
	
	if((GuidanceObstacleData.ob_enabled != flash_buffer.avoid_ob_enabled)||(GuidanceObstacleData.ob_distance!=flash_buffer.avoid_ob_distse)
		||(GuidanceObstacleData.ob_velocity!=flash_buffer.avoid_ob_velocity))
	{
	  flash_buffer.avoid_ob_enabled = GuidanceObstacleData.ob_enabled;
    flash_buffer.avoid_ob_distse = GuidanceObstacleData.ob_distance;
		flash_buffer.avoid_ob_velocity = GuidanceObstacleData.ob_velocity;
		STMFLASH_Write();
	}
}

/**
*   @brief  undercarriage_ptl_hf
  * @parm   data
  * @retval none
  */
void undercarriage_ptl_hf(u8 *data)
{
	u8 uce_adjust = 0; //0-stop, 1-uce down, 2-uce up;
	u8 uce_control = 0;
	zkrt_heartv2.uce_autoenabled = data[0];
	zkrt_heartv2.uce_autodown_ae = ((data[2])<<8)+(data[1]);
	zkrt_heartv2.uce_autoup_ae = ((data[4])<<8)+(data[3]);
	uce_adjust = data[5];
	uce_control = data[6];
	
	undercarriage_data.uce_autoenabled = zkrt_heartv2.uce_autoenabled;
	undercarriage_data.uce_autodown_ae = zkrt_heartv2.uce_autodown_ae;
	undercarriage_data.uce_autoup_ae = zkrt_heartv2.uce_autoup_ae;
	
	//脚架校准 -自动收发功能关闭和脚架控制无动作时允许
	if((!undercarriage_data.uce_autoenabled)&&(!uce_control))
	{
		if(uce_adjust==1)
		{
			UDCAIE_LEFT_DOWN;
			UDCAIE_RIGHT_DOWN;
		}
		else if(uce_adjust==2)
		{
			UDCAIE_LEFT_UP;
			UDCAIE_RIGHT_UP;
		}		
		else if(uce_adjust==3)
		{
			UDCAIE_LEFT_STOP;
			UDCAIE_RIGHT_STOP;
		}
	}
	
	//脚架控制 -自动收发功能关闭和脚架校准无动作时允许
	if((!undercarriage_data.uce_autoenabled)&&(!uce_adjust))
	{
		//脚架控制
		if(uce_control ==1)
		{
			//down
			undercarriage_data.state_bya3height = downed_udcaie_rs;
		}
		else if(uce_control ==2)
		{
			//up
			undercarriage_data.state_bya3height = uped_udcaie_rs;
		}
	}
	
	//write flash
	if(undercarriage_data.uce_autoenabled != flash_buffer.uce_autoenabled)
	{
	  flash_buffer.uce_autoenabled = undercarriage_data.uce_autoenabled;
		STMFLASH_Write();
	}
}

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
