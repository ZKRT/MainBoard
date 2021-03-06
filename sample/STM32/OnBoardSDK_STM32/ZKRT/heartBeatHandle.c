/**
  ******************************************************************************
  * @file    heartBeatHandle.c 
  * @author  ZKRT
  * @version V1.0.0
  * @date    25-March-2017
  * @brief   heartBeatHandle program body
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "heartBeatHandle.h"
#include "ostmr.h"
#include "osqtmr.h"	
#include "obstacleAvoid.h"
#include "undercarriageCtrl.h"

extern uint8_t msg_smartbat_dji_buffer[30];

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void hb_tmr_task(void);
static void hbv2_pack(void);
static u8 zkrt_heartbeat_timer_pack(void);

/* Private variables ---------------------------------------------------------*/

timer_upload_st hb_timer={0};    //心跳包定时结构体
heartv2_st zkrt_heartv2 = {0};   //定时发送心跳包v2的数据体缓存
zkrt_packet_t _zkrt_packet_hb;   //发送到DJI透传数据的心跳包缓存  
u8 timer_up_seq = 0;             //上传数据包序列
/* Private functions ---------------------------------------------------------*/


/**
*   @brief  heartbeat_parm_init 
  * @parm   none
  * @retval none
  */
void heartbeat_parm_init(void)
{
	//obstacle data
	memcpy(zkrt_heartv2.ob_distse_v, GuidanceObstacleData.g_distance_value, sizeof(zkrt_heartv2.ob_distse_v));
	zkrt_heartv2.avoid_ob_enabled = GuidanceObstacleData.ob_enabled;
	zkrt_heartv2.avoid_ob_distse = GuidanceObstacleData.ob_distance;
	zkrt_heartv2.avoid_ob_velocity = GuidanceObstacleData.ob_velocity;
	
	//undercarriage data
	zkrt_heartv2.uce_state = 0;
	if(undercarriage_data.run_state ==uped_udcaie_rs)
		zkrt_heartv2.uce_state = 1;
	zkrt_heartv2.uce_autoenabled = undercarriage_data.uce_autoenabled;
//	zkrt_heartv2.uce_angle = undercarriage_data.uce_angle;	
//	zkrt_heartv2.uce_autodown_ae = undercarriage_data.uce_autodown_ae;
//	zkrt_heartv2.uce_autoup_ae = undercarriage_data.uce_autoup_ae;
	
	//gas data
	zkrt_heartv2.gas_num5 = 0;
	zkrt_heartv2.gas_num6 = 0;
	zkrt_heartv2.gas_num7 = 0;
	zkrt_heartv2.gas_num8 = 0;

	//timer data
	hb_timer.timer_hbv2_packflag =0;
	hb_timer.timer_hbv2_packcnt = TIMER_HBV2PACK_TIMEOUT+5000; //zkrt_notice: 一开始延时等待
	hb_timer.timer_msg_upflag = 0;
	hb_timer.timer_msg_timecnt = 	TIMER_MSG_TIMEOUT + 5000;

	//heartbeat data
  _zkrt_packet_hb.cmd = UAV_TO_APP;
  _zkrt_packet_hb.command= DEFAULT_NUM;

  _zkrt_packet_hb.UAVID[0] = now_uav_type;                 //哪种飞机
  _zkrt_packet_hb.UAVID[1] = (uint8_t)(now_uav_num&0xff);	//飞机编号
  _zkrt_packet_hb.UAVID[2] = (uint8_t)(now_uav_num>>8);
  _zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEART;						//设备类型
  _zkrt_packet_hb.UAVID[4] = DEFAULT_NUM;									//设备编号
  _zkrt_packet_hb.UAVID[5] = DEFAULT_NUM;	
	
	t_systmr_insertQuickTask(hb_tmr_task, 1, OSTMR_PERIODIC);
}

/**
*   @brief  heartbeat_ctrl 心跳包处理
  * @parm   none
  * @retval 0-不需要发包，1-需要发包
  */
u8 zkrt_heartbeat_pack(void)
{
	u8 send_flag =0;
	
	//定时做心跳包V2包组包，心跳包V1包沿用旧代码，在其它地方有处理
	hbv2_pack();
	
	//定时组包
	send_flag = zkrt_heartbeat_timer_pack();
	
	return send_flag;
}
/**
*   @brief  zkrt_heartbeat_timer_pack  心跳包定时发送判断并组包
  * @parm   none
  * @retval 0-不需要发包，1-需要发包
  */
static u8 zkrt_heartbeat_timer_pack(void)
{
	u8 sendflag = 0;
	/*需要定时上传的数据：hbv1, hbv2*/
	if(hb_timer.timer_msg_upflag)
	{
		sendflag = 1;
		hb_timer.timer_msg_upflag = 0;
		
		switch(timer_up_seq)
		{
			case Hbv1_Seq:
				timer_up_seq = Hbv2_Seq;
			  hb_timer.timer_msg_timecnt = TIMER_MSG_NEXT_TIMEOUT;
			  //pack
				_zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEART;						//设备类型
			  msg_smartbat_dji_buffer[1] = (uint8_t)(tempture0&0xff);
			  msg_smartbat_dji_buffer[2] = (uint8_t)(tempture0>>8);
			  msg_smartbat_dji_buffer[4] = (uint8_t)(tempture1&0xff);
			  msg_smartbat_dji_buffer[5] = (uint8_t)(tempture1>>8);

       //将温度上下限保存下来
			  msg_smartbat_dji_buffer[6] = (uint8_t)(glo_tempture_low&0xff);
			  msg_smartbat_dji_buffer[7] = (uint8_t)(glo_tempture_low>>8);
			  msg_smartbat_dji_buffer[8] = (uint8_t)(glo_tempture_high&0xff);
			  msg_smartbat_dji_buffer[9] = (uint8_t)(glo_tempture_high>>8);
			  memcpy((void *)(_zkrt_packet_hb.data), (void *)(msg_smartbat_dji_buffer), 30);
			  break;
			case Hbv2_Seq:
				timer_up_seq = Hbv1_Seq;
			  hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
			  //pack	
			  _zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEARTV2;						//设备类型
			  memcpy((void *)(_zkrt_packet_hb.data), (void *)(&zkrt_heartv2), 30);
			  break;
			default:
				timer_up_seq = Hbv1_Seq;
			  hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
				break;	
		}
		zkrt_final_encode(&_zkrt_packet_hb);
	}
	return sendflag;
}
/**
*   @brief  hbv2_pack  心跳包2组包
  * @parm   none
  * @retval none
  */
static void hbv2_pack(void)
{
	if(hb_timer.timer_hbv2_packflag)
	{
		//obstacle data copy
		memcpy(zkrt_heartv2.ob_distse_v, GuidanceObstacleData.g_distance_value, sizeof(zkrt_heartv2.ob_distse_v));
		//undercarriage data copy
		if(undercarriage_data.run_state ==downed_udcaie_rs)
		  zkrt_heartv2.uce_state = 0;
		if(undercarriage_data.run_state ==uped_udcaie_rs)
		  zkrt_heartv2.uce_state = 1;		
//		zkrt_heartv2.uce_angle = undercarriage_data.uce_angle;  //协议已改，不用该属性
		
		//clear timer flag
		hb_timer.timer_hbv2_packflag = 0;
		hb_timer.timer_hbv2_packcnt = TIMER_HBV2PACK_TIMEOUT;
	}
}
/**
*   @brief  hb_tmr_task  心跳包定时任务
  * @parm   none
  * @retval none
  */
static void hb_tmr_task(void)
{
	if(hb_timer.timer_hbv2_packcnt>0)
	{
		hb_timer.timer_hbv2_packcnt--;
		if(!hb_timer.timer_hbv2_packcnt)
		{
			hb_timer.timer_hbv2_packflag =1;
		}	
	}
	if(hb_timer.timer_msg_timecnt>0)
	{
		hb_timer.timer_msg_timecnt--;
		if(!hb_timer.timer_msg_timecnt)
		{
			hb_timer.timer_msg_upflag =1;
		}	
	}	
}
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
