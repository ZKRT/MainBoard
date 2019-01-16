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
#include "dev_handle.h"
#include "led.h"
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void hb_tmr_task(void);
static void hbv2_pack(void);
static u8 zkrt_heartbeat_timer_pack(u8* data, u8* datalen);

/* Private variables ---------------------------------------------------------*/

timer_upload_st hb_timer = {0};  //心跳包定时结构体
//zd_heartv2_st zkrt_heartv2 = {0};   //定时发送心跳包v2的数据体缓存
//zd_heartv1_st zkrt_heartv1 = {0};   //定时发送心跳包v1的数据体缓存
zd_heartv3_3_st zkrt_heartbeat;
zkrt_packet_t _zkrt_packet_hb;   //发送到DJI透传数据的心跳包缓存
u8 timer_up_seq = 0;             //上传数据包序列
/* Private functions ---------------------------------------------------------*/


/**
*   @brief  heartbeat_parm_init
  * @parm   none
  * @retval none
  */
void heartbeat_parm_init(void) {
	hb_timer.timer_msg_upflag = 0;
	hb_timer.timer_msg_timecnt = 	TIMER_MSG_TIMEOUT + 5000;
	t_systmr_insertQuickTask(hb_tmr_task, 1, OSTMR_PERIODIC);
}

/**
*   @brief  heartbeat_ctrl 心跳包处理
  * @parm   none
  * @retval 0-不需要发包，1-需要发包
  */
u8 zkrt_heartbeat_pack(u8* data, u8* datalen) {
	u8 send_flag = 0;

//	//定时做心跳包V2包组包，心跳包V1包沿用旧代码，在其它地方有处理
//	hbv2_pack();

	//定时组包
	send_flag = zkrt_heartbeat_timer_pack(data, datalen);

	return send_flag;
}
/**
*   @brief    心跳包定时发送判断并组包
  * @parm   none
  * @retval 0-不需要发包，1-需要发包
  */
static u8 zkrt_heartbeat_timer_pack(u8* data, u8* datalen) {
	u8 sendflag = 0;
	/*需要定时上传的数据：hbv1, hbv2*/
	if (hb_timer.timer_msg_upflag) {
		sendflag = 1;
		hb_timer.timer_msg_upflag = 0;

		switch (timer_up_seq) {
		case Hbv1_Seq:
			timer_up_seq = Hbv1_Seq;
			hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
			break;
		default:
			timer_up_seq = Hbv1_Seq;
			hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
			break;
		}
	}
	return sendflag;
}
//static u8 zkrt_heartbeat_timer_pack(u8* data, u8* datalen) //old v3.2
//{
//	u8 sendflag = 0;
//	/*需要定时上传的数据：hbv1, hbv2*/
//	if(hb_timer.timer_msg_upflag)
//	{
//		sendflag = 1;
//		hb_timer.timer_msg_upflag = 0;
//
//		switch(timer_up_seq)
//		{
//			case Hbv1_Seq:
//				timer_up_seq = Hbv2_Seq;
//			  hb_timer.timer_msg_timecnt = TIMER_MSG_NEXT_TIMEOUT;
//			  //pack
//				_zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEART;						//设备类型
//			  zkrt_heartv1.t1 = zkrt_devinfo.temperature1;
//			  zkrt_heartv1.t2 = zkrt_devinfo.temperature2;
//       //将温度上下限保存下来
//			  zkrt_heartv1.t_low = zkrt_devinfo.temperature_low;
//			  zkrt_heartv1.t_high = zkrt_devinfo.temperature_high;
//			  zkrt_heartv1.t1_status = zkrt_devinfo.status_t1;
//			  zkrt_heartv1.t2_status = zkrt_devinfo.status_t2;
//			  memcpy((void *)(_zkrt_packet_hb.data), (void *)&zkrt_heartv1, sizeof(zd_heartv1_st));
//			  _zkrt_packet_hb.length = sizeof(zd_heartv1_st);
//			  break;
//			case Hbv2_Seq:
//				timer_up_seq = Hbv1_Seq;
//			  hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
//			  //pack
//			  _zkrt_packet_hb.UAVID[3] = DEVICE_TYPE_HEARTV2;						//设备类型
//			  memcpy((void *)(_zkrt_packet_hb.data), (void *)(&zkrt_heartv2), sizeof(zd_heartv2_st));
//			  _zkrt_packet_hb.length = sizeof(zd_heartv2_st);
//			  break;
//			default:
//				timer_up_seq = Hbv1_Seq;
//			  hb_timer.timer_msg_timecnt = TIMER_MSG_TIMEOUT;
//				break;
//		}
//		*datalen = zkrt_final_encode(data, &_zkrt_packet_hb);
//	}
//	return sendflag;
//}
/**
*   @brief  hbv2_pack  心跳包2组包
  * @parm   none
  * @retval none
  */
static void hbv2_pack(void) {
//	if(hb_timer.timer_hbv2_packflag)
//	{
//		//obstacle data copy
//		memcpy(zkrt_heartv2.ob_distse_v, GuidanceObstacleData.g_distance_value, sizeof(zkrt_heartv2.ob_distse_v));
//		//undercarriage data copy
//		if(undercarriage_data.run_state ==downed_udcaie_rs)
//		  zkrt_heartv2.uce_state = 0;
//		if(undercarriage_data.run_state ==uped_udcaie_rs)
//		  zkrt_heartv2.uce_state = 1;
////		zkrt_heartv2.uce_angle = undercarriage_data.uce_angle;  //协议已改，不用该属性
//
//		//clear timer flag
//		hb_timer.timer_hbv2_packflag = 0;
//		hb_timer.timer_hbv2_packcnt = TIMER_HBV2PACK_TIMEOUT;
//	}
}
/**
*   @brief  hb_tmr_task  心跳包定时任务
  * @parm   none
  * @retval none
  */
static void hb_tmr_task(void) {
//	if(hb_timer.timer_hbv2_packcnt>0)
//	{
//		hb_timer.timer_hbv2_packcnt--;
//		if(!hb_timer.timer_hbv2_packcnt)
//		{
//			hb_timer.timer_hbv2_packflag =1;
//		}
//	}
	if (hb_timer.timer_msg_timecnt > 0) {
		hb_timer.timer_msg_timecnt--;
		if (!hb_timer.timer_msg_timecnt) {
			hb_timer.timer_msg_upflag = 1;
		}
	}
}
int send_uart1_data(char*buf, int len)
{
  char* p = (char*)buf;

  if (NULL == buf)
  {
    return 0;
  }

  int sent_byte_count = 0;
  while (len--)
  {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      ;
    USART_SendData(USART1, *p++);
    ++sent_byte_count;
  }
  
  _FLIGHT_UART_TX_LED = 0;
  usart1_tx_flag = TimingDelay;
  
  return sent_byte_count;	
}

// static
unsigned char calc_checksum_v2(unsigned char* data, int size) {
	short i;
	unsigned int total = 0;
	for(i = 0; i < size; i++)
		total += data[i];

	return total;
}
uint8_t sendapiosbdata[16];
void send_ostacle_data_to_api_usart_hb(void) {
	if (zkrt_heartbeat_pack(NULL, NULL)) 
	{
		sendapiosbdata[0] = 0xeb;
		sendapiosbdata[1] = 0;
		sendapiosbdata[2] = 0;
		sendapiosbdata[3] = 0x09;
		sendapiosbdata[4] = 0x00;
		sendapiosbdata[5] = GuidanceObstacleData.online_flag;
		memcpy(sendapiosbdata+6, &GuidanceObstacleData.g_distance_value[GE_DIR_FRONT], 2);
		memcpy(sendapiosbdata+8, &GuidanceObstacleData.g_distance_value[GE_DIR_BACK], 2);
		memcpy(sendapiosbdata+10, &GuidanceObstacleData.g_distance_value[GE_DIR_LEFT], 2);
		memcpy(sendapiosbdata+12, &GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT], 2);
		sendapiosbdata[14] = calc_checksum_v2(sendapiosbdata, 14);
		sendapiosbdata[15] = 0xbe;
		send_uart1_data((char*)sendapiosbdata, sizeof(sendapiosbdata));
	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
