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
	if ((threemodeling_recv_flag - TimingDelay) >= DV_ONLINE_TIMEOUT) //��1��ʼ�㣬��12λ����Device_Status�ĵڶ����ֽڵĵ�4λ,����0XF7���롣
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
	if ((zkrt_devinfo.status_t1 == TEMP_INVALID) && (zkrt_devinfo.status_t2 == TEMP_INVALID)) //zkrt_notice: �����쳣���¶ȴ�����������
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
		devols->valuebit.obstacle = DV_ONLINE;  //�������߱��
	}
	else
	{
		devols->valuebit.obstacle = DV_OFFLINE;
		devols->valuebit.temperuture = DV_OFFLINE;
		zkrt_devinfo.status_t1 = TEMP_INVALID;
		zkrt_devinfo.status_t2 = TEMP_INVALID;
		zkrt_devinfo.temperature1 = 0;  //zkrt_notice: ֻ�м��ɰ岻����ʱ�������¶�ֵΪ0
		zkrt_devinfo.temperature2 = 0;
	}
#endif
}
