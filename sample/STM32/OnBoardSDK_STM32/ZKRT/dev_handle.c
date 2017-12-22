/**
  ******************************************************************************
  * @file    dev_handle.c 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   �豸��Ϣ����
	*					 + (1) init
	*                       
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "dev_handle.h"
#include "heartBeatHandle.h"
#include "obstacleAvoid.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
zk_dev_info_st zkrt_devinfo;
/* Private functions ---------------------------------------------------------*/
static void app_dev_state_handle(void);
/**
  * @brief  
  * @param  None
  * @retval None
  */
void appdev_init(void)
{
	zkrt_devinfo.temperature1 = TEMPTURE_HIGH_EXTRA;  //��ʼ��ʱ����Ч��ֵ
	zkrt_devinfo.temperature2 = TEMPTURE_HIGH_EXTRA;  //��ʼ��ʱ����Ч��ֵ
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void appdev_prcs(void)
{
	//dev state handle
	app_dev_state_handle();	
	
}
/**
  * @brief  the subdevice online state and info handle and that is receive from can com
  * @param  None
  * @retval None
  */
static void app_dev_state_handle(void)
{
	/*����can�յ���ģ����Ϣ���ж��ĸ�ģ�����ߺ󣬽������������߱����λ*/	
	if ((posion_recv_flag-TimingDelay)  >= DV_ONLINE_TIMEOUT)				
	{
		zkrt_heartv1.dev_online_s.valuebit.gas = DV_OFFLINE;
		//info reset
		zkrt_heartv1.gas_status.int8u = 0;
		zkrt_heartv1.gas_num1 = 0;
		zkrt_heartv1.gas_num2 = 0;
		zkrt_heartv1.gas_num3 = 0;
		zkrt_heartv1.gas_num4 = 0;
		zkrt_heartv1.gas_v1 = 0;
		zkrt_heartv1.gas_v2 = 0;
		zkrt_heartv1.gas_v3 = 0;
		zkrt_heartv1.gas_v4 = 0;
		zkrt_heartv2.gas_num5 = 0;
		zkrt_heartv2.gas_num6 = 0;
		zkrt_heartv2.gas_num7 = 0;
		zkrt_heartv2.gas_num8 = 0;
		zkrt_heartv2.gas_v5 = 0;
		zkrt_heartv2.gas_v6 = 0;
		zkrt_heartv2.gas_v7 = 0;
		zkrt_heartv2.gas_v8 = 0;
	}
	if ((throw_recv_flag-TimingDelay)  >= DV_ONLINE_TIMEOUT)
	{
		zkrt_heartv1.dev_online_s.valuebit.throwd = DV_OFFLINE;
		//info reset	
		zkrt_heartv1.feedback_s.valuebit.throw1_s =0;
		zkrt_heartv1.feedback_s.valuebit.throw2_s =0;
		zkrt_heartv1.feedback_s.valuebit.throw3_s =0;
	}
	if ((camera_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		zkrt_heartv1.dev_online_s.valuebit.siglecamera = DV_OFFLINE;
		//info reset
	}
	if ((irradiate_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		zkrt_heartv1.dev_online_s.valuebit.irradiate = DV_OFFLINE;
		//info reset
		zkrt_heartv1.feedback_s.valuebit.irradiate_s = 0;
	}	
	if ((phone_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		zkrt_heartv1.dev_online_s.valuebit.megaphone = DV_OFFLINE;
		//info reset
	}
	if ((threemodeling_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)  //��1��ʼ�㣬��12λ����Device_Status�ĵڶ����ֽڵĵ�4λ,����0XF7���롣
	{
		zkrt_heartv1.dev_online_s.valuebit.threemodelling = DV_OFFLINE;
		//info reset
		zkrt_heartv1.feedback_s.valuebit.threemodeling_photo_s = 0;
	}
	if ((multicamera_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)   
	{
		zkrt_heartv1.dev_online_s.valuebit.multicamera = DV_OFFLINE;
		//info reset
	}	
	//temperature online check
	if((zkrt_devinfo.status_t1 == TEMP_INVALID)&&(zkrt_devinfo.status_t2 == TEMP_INVALID))  //zkrt_notice: �����쳣���¶ȴ�����������
	{
		zkrt_heartv1.dev_online_s.valuebit.temperuture = DV_OFFLINE;
	}
	else
	{
		zkrt_heartv1.dev_online_s.valuebit.temperuture = DV_ONLINE;
	}
	//obstacle online check and temperature check again
#ifdef USE_SESORINTEGRATED		
	if(GuidanceObstacleData.online_flag !=1)
	{
		zkrt_heartv1.dev_online_s.valuebit.obstacle = DV_ONLINE;  //�������߱��
	}
	else
	{
		zkrt_heartv1.dev_online_s.valuebit.obstacle = DV_OFFLINE; 
		zkrt_heartv1.dev_online_s.valuebit.temperuture = DV_OFFLINE;
		zkrt_devinfo.status_t1 = TEMP_INVALID;
		zkrt_devinfo.status_t2 = TEMP_INVALID;
		zkrt_devinfo.temperature1 = 0;  //zkrt_notice: ֻ�м��ɰ岻����ʱ�������¶�ֵΪ0
		zkrt_devinfo.temperature2 = 0;		
	}
#endif	
}
