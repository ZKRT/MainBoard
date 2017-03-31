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

timer_upload_st hb_timer={0};    //��������ʱ�ṹ��
heartv2_st zkrt_heartv2 = {0};   //��ʱ����������v2�������建��
zkrt_packet_t _zkrt_packet_hb;   //���͵�DJI͸�����ݵ�����������  
u8 timer_up_seq = 0;             //�ϴ����ݰ�����
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
	zkrt_heartv2.uce_angle = undercarriage_data.uce_angle;	
	zkrt_heartv2.uce_autodown_ae = undercarriage_data.uce_autodown_ae;
	zkrt_heartv2.uce_autoup_ae = undercarriage_data.uce_autoup_ae;
	
	//timer data
	hb_timer.timer_hbv2_packflag =0;
	hb_timer.timer_hbv2_packcnt = TIMER_HBV2PACK_TIMEOUT+5000; //zkrt_notice: һ��ʼ��ʱ�ȴ�
	hb_timer.timer_msg_upflag = 0;
	hb_timer.timer_msg_timecnt = 	TIMER_MSG_TIMEOUT + 5000;

	//heartbeat data
  _zkrt_packet_hb.cmd = UAV_TO_APP;
  _zkrt_packet_hb.command= DEFAULT_NUM;

  _zkrt_packet_hb.UAVID[0] = now_uav_type;                 //���ַɻ�
  _zkrt_packet_hb.UAVID[1] = (uint8_t)(now_uav_num&0xff);	//�ɻ����
  _zkrt_packet_hb.UAVID[2] = (uint8_t)(now_uav_num>>8);
  _zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEART;						//�豸����
  _zkrt_packet_hb.UAVID[4] = DEFAULT_NUM;									//�豸���
  _zkrt_packet_hb.UAVID[5] = DEFAULT_NUM;	
	
	t_systmr_insertQuickTask(hb_tmr_task, 1, OSTMR_PERIODIC);
}

/**
*   @brief  heartbeat_ctrl ����������
  * @parm   none
  * @retval 0-����Ҫ������1-��Ҫ����
  */
u8 zkrt_heartbeat_pack(void)
{
	u8 send_flag =0;
	
	//��ʱ��������V2�������������V1�����þɴ��룬�������ط��д���
	hbv2_pack();
	
	//��ʱ���
	send_flag = zkrt_heartbeat_timer_pack();
	
	return send_flag;
}
/**
*   @brief  zkrt_heartbeat_timer_pack  ��������ʱ�����жϲ����
  * @parm   none
  * @retval 0-����Ҫ������1-��Ҫ����
  */
static u8 zkrt_heartbeat_timer_pack(void)
{
	u8 sendflag = 0;
	/*��Ҫ��ʱ�ϴ������ݣ�hbv1, hbv2*/
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
				_zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEART;						//�豸����
			  msg_smartbat_dji_buffer[1] = (uint8_t)(tempture0&0xff);
			  msg_smartbat_dji_buffer[2] = (uint8_t)(tempture0>>8);
			  msg_smartbat_dji_buffer[4] = (uint8_t)(tempture1&0xff);
			  msg_smartbat_dji_buffer[5] = (uint8_t)(tempture1>>8);

       //���¶������ޱ�������
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
			  _zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEARTV2;						//�豸����
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
*   @brief  hbv2_pack  ������2���
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
		zkrt_heartv2.uce_angle = undercarriage_data.uce_angle;
		
		//clear timer flag
		hb_timer.timer_hbv2_packflag = 0;
		hb_timer.timer_hbv2_packcnt = TIMER_HBV2PACK_TIMEOUT;
	}
}
/**
*   @brief  hb_tmr_task  ��������ʱ����
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
