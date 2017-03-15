/*! @file main.cpp
*  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  An exmaple program of DJI-onboard-SDK portable for stm32
 *
 *  Copyright 2016 DJI. All right reserved.
 *  */

#ifndef MAIN_H
#define MAIN_H

#include "string.h"
#include "stdio.h"
#include "DJI_API.h"
#include "driver.h"
#include "DJI_Flight.h"
#include "BspUsart.h"
#include "stm32f4xx_conf.h"
#include "cppforstm32.h"
#include "stm32f4xx.h"
#include "Activate.h"
#include "DJI_VirtualRC.h"
#include "timer.h"
#include <stdlib.h>
#include "VirtualRC.h"
#include "DJI_Flight.h"
#include "DJI_HotPoint.h"
#include "HotPoint.h"
#include "Receive.h"
#include "bsp.h"
#include "LocalNavigation.h"
#include "MobileCommand.h"

extern uint32_t tick; //tick is the time stamp,which record how many ms since u initialize the system.
//warnning: after 49 days of non-reset running, tick will RESET to ZERO.

//add by yanly
enum dji_run_state
{
	init_none_djirs = 0,		  //没有连接
	set_broadcastFreq_djirs,  //发送广播订阅
	set_broadcastFreq_ok,     //订阅成功
	set_avtivate_djirs,		    //激活 
	avtivated_ok_djirs		    //激活成功
}; 	

enum dji_oes_fc_controlbit
{
	fc_tempctrl_b = 0,		  //高温控制位号
	fc_obstacle_b           //避障控制位号
}; 	
typedef struct
{
	char run_status;
	char temp_alarmed; //1-高温逃逸，0-正常
	u32 cmdres_timeout;
	int oes_fc_controled; //高温控制置1<<0; 避障控制置1<<1; 全部都没有控制置0
}dji_sdk_status;
extern dji_sdk_status djisdk_state;

//void zkrt_setFrequencyCallback(DJI::onboardSDK::CoreAPI *api __UNUSED, Header *protocolHeader, DJI::UserData userData __UNUSED);
#endif //MAIN_H

