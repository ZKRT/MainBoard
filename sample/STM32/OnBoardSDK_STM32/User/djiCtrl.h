/**
  ******************************************************************************
  * @file    djiCtrl.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    08-March-2017
  * @brief   Header for djiCtrl.cpp module
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DJICTRL_H
#define __DJICTRL_H

/* Includes ------------------------------------------------------------------*/
#include "DJI_API.h"
#include "DJI_Camera.h"
#include "DJI_Type.h"
#include "DJI_VirtualRC.h"
#include "main.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//dji 运行状态
enum dji_run_state
{
	init_none_djirs = 0,		  //没有连接
	set_broadcastFreq_djirs,  //发送广播订阅
	set_broadcastFreq_ok,     //订阅成功
	set_avtivate_djirs,		    //激活 
	avtivated_ok_djirs		    //激活成功
}; 	

//dji 飞控控制状态标记，高温和避障控制时置位
enum dji_oes_fc_controlbit
{
	fc_tempctrl_b = 0,		  //高温控制位号
	fc_obstacle_b           //避障控制位号
}; 	

		
/* Exported typedef ------------------------------------------------------------*/

//dji 运行状态结构体
typedef struct
{
	char run_status;
	char temp_alarmed; //1-高温逃逸，0-正常
	unsigned int cmdres_timeout;
	char oes_fc_controled; //高温控制置1<<0; 避障控制置1<<1; 全部都没有控制置0
	char last_fc_controled; //上一次的oes_fc_controled值
}dji_sdk_status;

/* Exported functions ------------------------------------------------------- */
void dji_init(void);
void dji_process(void);
void dji_get_roll_pitch(double* roll, double* pitch);
extern dji_sdk_status djisdk_state;
#endif /* __DJICTRL_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
