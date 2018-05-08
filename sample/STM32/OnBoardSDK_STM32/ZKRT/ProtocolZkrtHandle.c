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

///////////////////////////////////////////////////////////////////normal command
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
	u16 temp_distse, temp_vel;

	temp_distse = ((data[2]) << 8) + (data[1]);
	temp_vel    = ((data[4]) << 8) + (data[3]);

	//check para valid
	if ((temp_distse <= 1500) && (temp_distse >= 400))
	{}
	else
		return;

	if (temp_vel <= 20) //<=2m/s
	{}
	else
		return;

	GuidanceObstacleData.ob_enabled  = data[0];
	GuidanceObstacleData.ob_distance = temp_distse;
	GuidanceObstacleData.ob_velocity = temp_vel;

	if ((GuidanceObstacleData.ob_enabled != flash_buffer.avoid_ob_enabled) || (GuidanceObstacleData.ob_distance != flash_buffer.avoid_ob_distse)
	        || (GuidanceObstacleData.ob_velocity != flash_buffer.avoid_ob_velocity))
	{
		flash_buffer.avoid_ob_enabled  = GuidanceObstacleData.ob_enabled;
		flash_buffer.avoid_ob_distse   = GuidanceObstacleData.ob_distance;
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
//	u8 uce_adjust = 0; //0-stop, 1-uce down, 2-uce up;
//	u8 uce_control = 0;
//	zkrt_heartv2.uce_autoenabled = data[0];
////	zkrt_heartv2.uce_autodown_ae = ((data[2])<<8)+(data[1]); //Э���Ѹģ����ø�����
////	zkrt_heartv2.uce_autoup_ae = ((data[4])<<8)+(data[3]);
//	uce_adjust = data[5];
//	uce_control = data[6];
//
//	undercarriage_data.uce_autoenabled = zkrt_heartv2.uce_autoenabled;
////	undercarriage_data.uce_autodown_ae = zkrt_heartv2.uce_autodown_ae;
////	undercarriage_data.uce_autoup_ae = zkrt_heartv2.uce_autoup_ae;
//
//	//�ż�У׼ -�Զ��շ����ܹرպͽżܿ����޶���ʱ����
//	if((!undercarriage_data.uce_autoenabled)&&(!uce_control))
//	{
//		if(uce_adjust==1)
//		{
//			UDCAIE_LEFT_DOWN;
//			UDCAIE_RIGHT_DOWN;
//		}
//		else if(uce_adjust==2)
//		{
//			UDCAIE_LEFT_UP;
//			UDCAIE_RIGHT_UP;
//		}
//		else if(uce_adjust==3)
//		{
//			UDCAIE_LEFT_STOP;
//			UDCAIE_RIGHT_STOP;
//		}
//	}
//
//	//�żܿ��� -�Զ��շ����ܹرպͽż�У׼�޶���ʱ����
//	if((!undercarriage_data.uce_autoenabled)&&(!uce_adjust))
//	{
//		//�żܿ���
//		if(uce_control ==1)
//		{
//			//down
//			undercarriage_data.state_bya3height = downed_udcaie_rs;
//		}
//		else if(uce_control ==2)
//		{
//			//up
//			undercarriage_data.state_bya3height = uped_udcaie_rs;
//		}
//	}
//
//	//write flash
//	if(undercarriage_data.uce_autoenabled != flash_buffer.uce_autoenabled)
//	{
//	  flash_buffer.uce_autoenabled = undercarriage_data.uce_autoenabled;
//		STMFLASH_Write();
//	}
}
///////////////////////////////////////////////////////////////////common command
/**
  * @brief
  * @param  None
  * @retval None
  */
u8 common_data_handle(const zkrt_packet_t *spacket, zkrt_packet_t *rpacket)
{
	u8 res                                = 1;
	common_data_plst *scommon             = (common_data_plst *)(spacket->data);
	common_data_plst *rcommon             = (common_data_plst*)(rpacket->data);
	common_get_devinfo_plst *rother;

	//differnet msg handle by type num
	switch (scommon->type_num)
	{
	case TN_GETDEVINFO:
		rother                                = (common_get_devinfo_plst *)(rpacket->data + 1);
		rother->status                        = 0;
		if (spacket->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_GAS)
			memcpy(rother->model, DEV_MODEL_BSA, sizeof(DEV_MODEL_BSA));

		else
			memcpy(rother->model, DEV_MODEL_MB, sizeof(DEV_MODEL_MB));
		memcpy(rother->hw_version, DEV_HW_MB, 6);
		memcpy(rother->sw_version, DEV_SW_MB, 6);
		rpacket->length                       = sizeof(common_get_devinfo_plst) + 1;
		break;
	default:
		res                                   = 0;
		break;
	}
	//packet common handle
	if (res == 1)
	{
		memcpy(rpacket, spacket, ZK_HEADER_LEN);
		rcommon->type_num                     = scommon->type_num;
		rpacket->cmd                          = UAV_TO_APP;
		rpacket->end_code                     = _END_CODE;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////special command
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
