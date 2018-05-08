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
#include "baoshian.h"
#include "guorui.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
zk_dev_info_st zkrt_devinfo;
devicestatus_un* devols;
feedback_un* devfbv;
/* Private functions ---------------------------------------------------------*/
static void app_dev_state_handle(void);
/**
  * @brief
  * @param  None
  * @retval None
  */
void appdev_init(void)
{
	zkrt_devinfo.devself = &zkrt_heartbeat;
	devols = &zkrt_devinfo.devself->dev.dev_online_s;
	devfbv = &zkrt_devinfo.devself->dev.feedback_s;
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
	if ((posion_recv_flag - TimingDelay)  >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.gas = DV_OFFLINE;
		//info reset
		zkrt_devinfo.devself->gas.ch_num = 0;
		zkrt_devinfo.devself->gas.ch_status = 0;
		memset(zkrt_devinfo.devself->gas.gas_value, 0x00, sizeof(zkrt_devinfo.devself->gas.gas_value));
		//manu handle
		gr_handle.gas_online_flag = 0;
		bsa_prcs_handle.gas_online_flag = 0;
	}
	if ((throw_recv_flag - TimingDelay)  >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.throwd = DV_OFFLINE;
		//info reset
		devfbv->valuebit.throw1_s = 0;
		devfbv->valuebit.throw2_s = 0;
		devfbv->valuebit.throw3_s = 0;
	}
	if ((camera_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.siglecamera = DV_OFFLINE;
		//info reset
	}
	if ((irradiate_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.irradiate = DV_OFFLINE;
		//info reset
		devfbv->valuebit.irradiate_s = 0;
	}
	if ((phone_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.megaphone = DV_OFFLINE;
		//info reset
	}
	if ((threemodeling_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT) //从1开始算，第12位，即Device_Status的第二个字节的第4位,即与0XF7相与。
	{
		devols->valuebit.threemodelling = DV_OFFLINE;
		//info reset
		devfbv->valuebit.threemodeling_photo_s = 0;
	}
	if ((multicamera_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT)
	{
		devols->valuebit.multicamera = DV_OFFLINE;
		//info reset
	}
	//temperature online check
	if ((zkrt_devinfo.status_t1 == TEMP_INVALID) && (zkrt_devinfo.status_t2 == TEMP_INVALID)) //zkrt_notice: 两个异常置温度传感器不在线
	{
		devols->valuebit.temperuture = DV_OFFLINE;
	}
	else
	{
		devols->valuebit.temperuture = DV_ONLINE;
	}
	//obstacle online check and temperature check again
#ifdef USE_SESORINTEGRATED
	if (GuidanceObstacleData.online_flag == DV_ONLINE)
	{
		devols->valuebit.obstacle = DV_ONLINE;  //避障在线标记
	}
	else
	{
		devols->valuebit.obstacle = DV_OFFLINE;
		devols->valuebit.temperuture = DV_OFFLINE;
		zkrt_devinfo.status_t1 = TEMP_INVALID;
		zkrt_devinfo.status_t2 = TEMP_INVALID;
		zkrt_devinfo.temperature1 = 0;  //zkrt_notice: 只有集成板不在线时，才置温度值为0
		zkrt_devinfo.temperature2 = 0;
	}
#endif
}
