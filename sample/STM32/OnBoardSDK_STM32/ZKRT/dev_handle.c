/**
  ******************************************************************************
  * @file    dev_handle.c 
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   设备信息处理
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
	zkrt_devinfo.temperature1 = TEMPTURE_HIGH_EXTRA;  //初始化时是无效的值
	zkrt_devinfo.temperature2 = TEMPTURE_HIGH_EXTRA;  //初始化时是无效的值
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
	/*根据can收到的模块信息，判断哪个模块在线后，将心跳包的在线标记置位*/	
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
	if ((threemodeling_recv_flag-TimingDelay) >= DV_ONLINE_TIMEOUT)  //从1开始算，第12位，即Device_Status的第二个字节的第4位,即与0XF7相与。
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
	if((zkrt_devinfo.status_t1 == TEMP_INVALID)&&(zkrt_devinfo.status_t2 == TEMP_INVALID))  //zkrt_notice: 两个异常置温度传感器不在线
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
		zkrt_heartv1.dev_online_s.valuebit.obstacle = DV_ONLINE;  //避障在线标记
	}
	else
	{
		zkrt_heartv1.dev_online_s.valuebit.obstacle = DV_OFFLINE; 
		zkrt_heartv1.dev_online_s.valuebit.temperuture = DV_OFFLINE;
		zkrt_devinfo.status_t1 = TEMP_INVALID;
		zkrt_devinfo.status_t2 = TEMP_INVALID;
		zkrt_devinfo.temperature1 = 0;  //zkrt_notice: 只有集成板不在线时，才置温度值为0
		zkrt_devinfo.temperature2 = 0;		
	}
#endif	
}
