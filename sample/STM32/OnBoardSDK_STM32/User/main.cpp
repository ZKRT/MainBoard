/**
  ******************************************************************************
  * @file    main.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    17-January-2017
  * @brief
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "main.h"
#include "MissionSample.h"
#ifdef CPLUSPLUS_HANDLE_ZK
extern "C" {
#endif

#include "stm32f4xx_adc.h"
#include "sys.h"
#include "adc.h"
#include "usart.h"
#include "zkrt.h"
#include "djiapp.h"
#include "flash.h"
#include "bat.h"
#include "mobileDataHandle.h"
#include "led.h"
#include "ostmr.h"
#include "osqtmr.h"
#ifdef USE_LWIP_FUN
#include "lwip_comm.h"
#include "lwipapp.h"
#include "usb_usr_process.h"
#endif
#ifdef HWTEST_FUN
#include "hwTestHandle.h"
#endif
#include "versionzkrt.h"
#ifdef USE_OBSTACLE_AVOID_FUN
#include "obstacleAvoid.h"
#endif
#include "undercarriageCtrl.h"
#include "heartBeatHandle.h"
#ifdef USE_SESORINTEGRATED
#include "sersorIntegratedHandle.h"
#endif
#include "iwatchdog.h"
#include "appcan.h"
#include "dev_handle.h"
#include "appgas.h"
#include "pelcod.h"

#ifdef CPLUSPLUS_HANDLE_ZK
}
#endif
#include "NuclearRadiationDetection.h"
/*-----------------------NEW DJI_LIB VARIABLE-----------------------------*/
using namespace DJI::OSDK;

bool           threadSupport = false;
bool           isFrame       = false;
RecvContainer  receivedFrame;
RecvContainer* rFrame  = &receivedFrame;
Vehicle        vehicle = Vehicle(threadSupport);
Vehicle*       v       = &vehicle;

extern TerminalCommand myTerminal; //add by yanly

Control::CtrlData flightData_zkrtctrl(Control::VERTICAL_VELOCITY | Control::HORIZONTAL_VELOCITY | Control::YAW_RATE | Control::HORIZONTAL_BODY, 0, 0, 0, 0);
/*----------ZKRT VARIABLE----------*/

/* Private function prototypes -----------------------------------------------*/
void mobile_heardbeat_packet_control(void);
void temperature_prcs(void);
void avoid_temp_alarm(void);
#ifdef USE_OBSTACLE_AVOID_FUN
void avoid_obstacle_alarm(void);
void avoid_obstacle_alarm_V2(void);
void avoid_obstacle_alarm_V3(void);
void sys_ctrl_timetask(void);
#endif
/**
  * @brief  main. 主函数
  * @param  None
  * @retval None
  */
int main() {
	BSPinit();
	msg_handle_init();
	delay_nms(1000);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n");
	printf("PRODUCT_NAME: %s\r\nPRODUCT_ID: %s\r\nPRODUCT_VERSION: %s\r\nPRODUCT_HWVERSION: %s\r\nPRODUCT_TIME: %s %s\r\n", PRODUCT_NAME, PRODUCT_ID, PRODUCT_VERSION, PRODUCT_HWVERSION, __DATE__, __TIME__);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n");
#ifdef USE_DJI_FUN
	if (dji_init() < 0)
		return -1;
#endif
#ifdef USE_LWIP_FUN
	lwip_prcs_init();
#endif
#ifdef USE_OBSTACLE_AVOID_FUN
	guidance_init();
#endif
#ifdef USE_UNDERCARRIAGE_FUN
	undercarriage_init();
#endif
	appdev_init();
	appcan_init();
	appgas_init();
	heartbeat_parm_init();   //put at last
	t_ostmr_insertTask(sys_ctrl_timetask, 1000, OSTMR_PERIODIC);  //1000
#ifdef USE_PELCOD_FUN
	pelcod_init();
#endif
	nuclear_radiation_detect_init();
	while (1) {
		nuclear_radiation_detect_prcs();
		appgas_prcs();
#ifdef USE_DJI_FUN
		dji_process();                        //大疆SDK处理
#endif
		mobile_data_process();                //将接收到的mobile透传数据进行解析处理
		appcan_prcs();                        //can com handle for the sub device message
		temperature_prcs();                   //温度超过上下限启动逃逸功能
		appdev_prcs();
#ifdef USE_OBSTACLE_AVOID_FUN
#ifdef USE_SESORINTEGRATED
		app_sersor_integrated_prcs();         //集成板数据处理
#else
		main_recv_decode_zkrt_dji_guidance(); //Guidance数据包解析处理
#endif
#ifdef USE_UART3_TEST_FUN
		myTerminal.terminalCommandHandler(v);
#endif
		avoid_obstacle_alarm_V3();
#endif
		led_process();                        //LED控制
		stmflash_process();                   //用户配置信息处理
#ifdef USE_LWIP_FUN
		lwip_prcs();													//网络流程
#endif
#ifdef USE_USB_FUN
		usb_user_prcs();                      //USB流程
#endif
#ifdef USE_UNDERCARRIAGE_FUN
		undercarriage_process();              //起落架处理
#endif
		IWDG_Feed();
	}
}
/**
  * @brief  温度处理流程：获取温度，避温控制
  * @param  None
  * @retval None
  */
void temperature_prcs(void) {
	if (read_temperature() == 1)
		avoid_temp_alarm();     //aviod temperature alarm control
}
/**
  * @brief  避温控制
  * @param  None
  * @retval None
  */
void avoid_temp_alarm(void) {
	u8 status_t1 = zkrt_devinfo.status_t1;
	u8 status_t2 = zkrt_devinfo.status_t2;

	if (djisdk_state.run_status != avtivated_ok_djirs)
		return;

	if ((status_t1 == TEMP_OVER_HIGH) || (status_t2 == TEMP_OVER_HIGH)) {
		djisdk_state.temp_alarmed = 1;
		flightData_zkrtctrl.z = 1;  //1m/s
		djisdk_state.oes_fc_controled |= 1 << fc_tempctrl_b;
		ZKRT_LOG(LOG_NOTICE, "avoid_temp_alarm open======================================\r\n");
	} else {
		djisdk_state.temp_alarmed = 0;
		if (flightData_zkrtctrl.z)
			flightData_zkrtctrl.z = 0;
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_tempctrl_b);
	}
}
#ifdef USE_OBSTACLE_AVOID_FUN
/**
*   @brief  avoid_obstacle_alarm 避障控制
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm(void) {
	u8 move_flag; //移动标记

	if (djisdk_state.run_status != avtivated_ok_djirs) //OES没激活
		return;

	if (GuidanceObstacleData.online_flag == 0) //Guidance不在线
		return;

	if (GuidanceObstacleData.ob_enabled == 0) { //避障不生效
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		return;
	}

	move_flag = obstacle_avoidance_handle();
	if (move_flag) {
		djisdk_state.oes_fc_controled |= 1 << fc_obstacle_b;

#ifdef OBSTACLE_VEL_MODE
		if (move_flag & (1 << (GE_DIR_FRONT - 1)))               //前后
			flightData_zkrtctrl.x = OBSTACLE_VEL_FORWORD_X(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.x = OBSTACLE_VEL_FORWORD_X;
		else if (move_flag & (1 << (GE_DIR_BACK - 1)))
			flightData_zkrtctrl.x = OBSTACLE_VEL_BACK_X(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.x = OBSTACLE_VEL_BACK_X;
		else
			flightData_zkrtctrl.x = 0;

		if (move_flag & (1 << (GE_DIR_RIGHT - 1)))               //左右
			flightData_zkrtctrl.y = OBSTACLE_VEL_RIGHT_Y(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.y = OBSTACLE_VEL_RIGHT_Y;
		else if (move_flag & (1 << (GE_DIR_LEFT - 1)))
			flightData_zkrtctrl.y = OBSTACLE_VEL_LEFT_Y(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.y = OBSTACLE_VEL_LEFT_Y;
		else
			flightData_zkrtctrl.y = 0;
#else
		if (move_flag & (1 << (GE_DIR_FRONT - 1)))               //前后
			flightData_zkrtctrl.y = 0; .y = OBSTACLE_ANG_FORWORD_Y;
		else if (move_flag & (1 << (GE_DIR_BACK - 1)))
			flightData_zkrtctrl.y = 0; .y = OBSTACLE_ANG_BACK_Y;
		else
			flightData_zkrtctrl.y = 0; .y = 0;

		if (move_flag & (1 << (GE_DIR_RIGHT - 1)))               //左右
			flightData_zkrtctrl.y = 0; .x = OBSTACLE_ANG_RIGHT_X;
		else if (move_flag & (1 << (GE_DIR_LEFT - 1)))
			flightData_zkrtctrl.y = 0; .x = OBSTACLE_ANG_LEFT_X;
		else
			flightData_zkrtctrl.y = 0; .x = 0;
#endif
//		printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	} else {
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		if (flightData_zkrtctrl.x)
			flightData_zkrtctrl.x = 0;
		if (flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
}
/**
*   @brief  avoid_obstacle_alarm_V2 避障控制
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm_V2(void) {
	u8 move_flag; //移动标记
	float fl_x, fl_y;
	double yaw;

	if (djisdk_state.run_status != avtivated_ok_djirs) //OES没激活
		return;

	if (GuidanceObstacleData.online_flag == 0) //Guidance不在线
		return;

	if (GuidanceObstacleData.ob_enabled == 0) { //避障不生效
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		return;
	}
	yaw = getYaw(v->broadcast->getQuaternion());
	dji_get_roll_pitch(&djif_status.roll, &djif_status.pitch);
	djif_status.xnow = v->broadcast->getVelocity().x * cos(yaw) + v->broadcast->getVelocity().y * sin(yaw);
	djif_status.ynow = -v->broadcast->getVelocity().x * sin(yaw) + v->broadcast->getVelocity().y * cos(yaw);
	move_flag = obstacle_avoidance_handle_V2(&fl_x, &fl_y, v->broadcast->getRC().pitch, v->broadcast->getRC().roll);
//	if(GuidanceObstacleData.obstacle_time_flag)
//	{
//			flightData_zkrtctrl.x = 0;
//			flightData_zkrtctrl.y = 0;
//			djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	}
//	else
//	{
	if (move_flag) {
		flightData_zkrtctrl.x = fl_x;
		flightData_zkrtctrl.y = fl_y;
		djisdk_state.oes_fc_controled |= 1 << fc_obstacle_b;
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	} else {
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		if (flightData_zkrtctrl.x)
			flightData_zkrtctrl.x = 0;
		if (flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
//	}
}
/**
*   @brief  avoid_obstacle_alarm_V5 避障控制
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm_V3(void) {
	u8 ctrl_flag = 0;
	float fl_x = 0, fl_y = 0, fl_z = 0, fl_yaw = 0;
//	float vel_limiting = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
	float vel_hover = 0;
	float vel_return = 0.5;
	float vel_z_limit = 0.5;
	float rate_yaw_limit = 20;
	int i;
	if (djisdk_state.run_status != avtivated_ok_djirs) //OES没激活
		return;

	if (GuidanceObstacleData.online_flag == 0) //Guidance不在线
		return;

	if (GuidanceObstacleData.ob_enabled == 0) { //避障不生效
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		return;
	}

	obstacle_control_run_reset();  //控制状态重置

#ifdef USE_OBSTACLE_TEST2
	/*
		cmd： FA FB AA FRONT BACK RIGHT LEFT PITCH ROLL THROTTLE YAW INIT_FLAG FE
		*/
	GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] = myTerminal.cmdIn[3] * 10;
	GuidanceObstacleData.g_distance_value[GE_DIR_BACK] = myTerminal.cmdIn[4] * 10;
	GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] = myTerminal.cmdIn[5] * 10;
	GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] = myTerminal.cmdIn[6] * 10;
	//过滤, 飞机倾斜角度过大时
	djif_status.fiter_angle_ob = get_filter_ang_ob();
	if (djif_status.roll > djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] = DISTANCE_2HIGH_BY_ANGLE;
	if (djif_status.roll < -djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] = DISTANCE_2HIGH_BY_ANGLE;
	if (djif_status.pitch > djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_BACK] = DISTANCE_2HIGH_BY_ANGLE;
	if (djif_status.pitch < -djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] = DISTANCE_2HIGH_BY_ANGLE;
#endif
#ifdef USE_OBSTACLE_TEST1
	djif_status.rc_pitch = myTerminal.cmdIn[7] * 10 * myTerminal.int_flag;
	djif_status.rc_roll = myTerminal.cmdIn[8] * 10 * myTerminal.int_flag;
	djif_status.rc_throttle = myTerminal.cmdIn[9] * 10 * myTerminal.int_flag;
	djif_status.rc_yaw = myTerminal.cmdIn[10] * 10 * myTerminal.int_flag;
	djif_status.xnow = 0;
	djif_status.ynow = 0;
#endif

#ifndef USE_OBSTACLE_TEST1
	dji_get_flight_parm((void*)&djif_status);   //获取飞机状态等信息
#endif

	//方向控制逻辑检测
	obstacleAllControl.control[GE_DIR_FRONT - 1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT], GE_DIR_FRONT);
	obstacleAllControl.control[GE_DIR_BACK - 1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_BACK], GE_DIR_BACK);
	obstacleAllControl.control[GE_DIR_RIGHT - 1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT], GE_DIR_RIGHT);
	obstacleAllControl.control[GE_DIR_LEFT - 1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT], GE_DIR_LEFT);

	//前后筛选出x轴控制状态
	//左右筛选出y轴控制状态
	for (i = 0; i < 4; i++) {
		if ((obstacleAllControl.control[i].state == OCS_LIMITING_VEL) && (obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state == OCS_VOLUNTRAY_AVOID)) {
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state = OCS_NO_CONTROL;
		}
		if ((obstacleAllControl.control[i].state == OCS_HOVER) && (obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state == OCS_VOLUNTRAY_AVOID)) {
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state = OCS_HOVER;
		}
		if ((obstacleAllControl.control[i].state == OCS_VOLUNTRAY_AVOID) && (obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state == OCS_VOLUNTRAY_AVOID)) {
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state = OCS_HOVER;
			obstacleAllControl.control[i].state = OCS_HOVER;
		}
		if ((obstacleAllControl.control[i].state == OCS_NO_CONTROL ) && (obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state == OCS_VOLUNTRAY_AVOID)) {
			if (is_rc_goto_dir(i + 1)) {
				obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state = OCS_NO_CONTROL; //special handle
			}
			if (GuidanceObstacleData.g_distance_value[i + 1] < GuidanceObstacleData.ob_distance) { //special handle
				obstacleAllControl.control[(obstacleAllControl.control[i].opposite) - 1].state = OCS_HOVER;
				obstacleAllControl.control[i].state = OCS_HOVER;
			}
		}
	}
//	obstacleAllControl.x_state = OCS_NO_CONTROL;
//	obstacleAllControl.y_state = OCS_NO_CONTROL;
//	fl_x =0;fl_y=0;fl_z=0;
//limiting
	if (obstacleAllControl.control[GE_DIR_FRONT - 1].state == OCS_LIMITING_VEL) {
//		fl_x = vel_limiting;
		fl_x = get_limit_vx(GE_DIR_FRONT);
		obstacleAllControl.x_state = OCS_LIMITING_VEL;
	}
	if (obstacleAllControl.control[GE_DIR_BACK - 1].state == OCS_LIMITING_VEL) {
//		fl_x = -vel_limiting;
		fl_x = get_limit_vx(GE_DIR_BACK);
		obstacleAllControl.x_state = OCS_LIMITING_VEL;
	}
	if (obstacleAllControl.control[GE_DIR_RIGHT - 1].state == OCS_LIMITING_VEL) {
//		fl_y = vel_limiting;
		fl_y = get_limit_vy(GE_DIR_RIGHT);
		obstacleAllControl.y_state = OCS_LIMITING_VEL;
	}
	if (obstacleAllControl.control[GE_DIR_LEFT - 1].state == OCS_LIMITING_VEL) {
//		fl_y = -vel_limiting;
		fl_y = get_limit_vy(GE_DIR_LEFT);
		obstacleAllControl.y_state = OCS_LIMITING_VEL;
	}
//hover
	if (obstacleAllControl.control[GE_DIR_FRONT - 1].state == OCS_HOVER) {
		fl_x = vel_hover;
		obstacleAllControl.x_state = OCS_HOVER;
	}
	if (obstacleAllControl.control[GE_DIR_BACK - 1].state == OCS_HOVER) {
		fl_x = -vel_hover;
		obstacleAllControl.x_state = OCS_HOVER;
	}
	if (obstacleAllControl.control[GE_DIR_RIGHT - 1].state == OCS_HOVER) {
		fl_y = vel_hover;
		obstacleAllControl.y_state = OCS_HOVER;
	}
	if (obstacleAllControl.control[GE_DIR_LEFT - 1].state == OCS_HOVER) {
		fl_y = -vel_hover;
		obstacleAllControl.y_state = OCS_HOVER;
	}
//return
	if (obstacleAllControl.control[GE_DIR_FRONT - 1].state == OCS_VOLUNTRAY_AVOID) {
		fl_x = -vel_return;
		obstacleAllControl.x_state = OCS_VOLUNTRAY_AVOID;
	}
	if (obstacleAllControl.control[GE_DIR_BACK - 1].state == OCS_VOLUNTRAY_AVOID) {
		fl_x = vel_return;
		obstacleAllControl.x_state = OCS_VOLUNTRAY_AVOID;
	}
	if (obstacleAllControl.control[GE_DIR_RIGHT - 1].state == OCS_VOLUNTRAY_AVOID) {
		fl_y = -vel_return;
		obstacleAllControl.y_state = OCS_VOLUNTRAY_AVOID;
	}
	if (obstacleAllControl.control[GE_DIR_LEFT - 1].state == OCS_VOLUNTRAY_AVOID) {
		fl_y = vel_return;
		obstacleAllControl.y_state = OCS_VOLUNTRAY_AVOID;
	}

//悬停控制情况下，该轴无rc控制时释放该轴控制权
	if ((!is_rc_goto_dir(GE_DIR_FRONT)) && (!is_rc_goto_dir(GE_DIR_BACK))
	        && (obstacleAllControl.x_state == OCS_HOVER)
	   ) {
		fl_x = 0;
		obstacleAllControl.x_state = OCS_NO_CONTROL;
	}
	if ((!is_rc_goto_dir(GE_DIR_RIGHT)) && (!is_rc_goto_dir(GE_DIR_LEFT))
	        && (obstacleAllControl.y_state == OCS_HOVER)
	   ) {
		fl_y = 0;
		obstacleAllControl.y_state = OCS_NO_CONTROL;
	}

//OES控制时，映射RC控制状态，由OES集中控制  ---y轴
	if (obstacleAllControl.x_state > OCS_NO_CONTROL) {
		if (obstacleAllControl.y_state == OCS_NO_CONTROL) {
			if (is_rc_goto_dir(GE_DIR_RIGHT)) {
//				fl_y = vel_limiting;
				fl_y = get_limit_vy(GE_DIR_RIGHT);
				obstacleAllControl.y_state = OCS_OES_REPLACE_RC;
			} else if (is_rc_goto_dir(GE_DIR_LEFT)) {
//				fl_y = -vel_limiting;
				fl_y = get_limit_vy(GE_DIR_LEFT);
				obstacleAllControl.y_state = OCS_OES_REPLACE_RC;
			}
		}
	}
//OES控制时，映射RC控制状态，由OES集中控制  ---x轴
	if (obstacleAllControl.y_state > OCS_NO_CONTROL) {
		if (obstacleAllControl.x_state == OCS_NO_CONTROL) {
			if (is_rc_goto_dir(GE_DIR_FRONT)) {
//				fl_x = vel_limiting;
				fl_x = get_limit_vx(GE_DIR_FRONT);
				obstacleAllControl.x_state = OCS_OES_REPLACE_RC;
			} else if (is_rc_goto_dir(GE_DIR_BACK)) {
//				fl_x = -vel_limiting;
				fl_x = get_limit_vx(GE_DIR_BACK);
				obstacleAllControl.x_state = OCS_OES_REPLACE_RC;
			}
		}
	}
//OES控制时，映射RC控制状态，由OES集中控制  ---z轴
	if ((obstacleAllControl.x_state > OCS_NO_CONTROL) || (obstacleAllControl.y_state > OCS_NO_CONTROL)) {
		ctrl_flag = 1;
		if (djif_status.rc_throttle > 500) {
			fl_z = vel_z_limit;
		} else if (djif_status.rc_throttle < -500) {
			fl_z = -vel_z_limit;
		} else
			fl_z = 0;
		if (djif_status.rc_yaw > 500) {
			fl_yaw = rate_yaw_limit;
		} else if (djif_status.rc_yaw < -500) {
			fl_yaw = -rate_yaw_limit;
		} else
			fl_yaw = 0;
	} else {
		ctrl_flag = 0;
		for (i = 0; i < 4; i++) {
			obstacleAllControl.control[i].state = OCS_NO_CONTROL;
		}
	}

	//记录本次控制状态
	for (i = 0; i < 4; i++) {
		obstacleAllControl.control[i].last_state = obstacleAllControl.control[i].state;
	}

	if (ctrl_flag) {
		flightData_zkrtctrl.x = fl_x;
		flightData_zkrtctrl.y = fl_y;
		flightData_zkrtctrl.z = fl_z;
		flightData_zkrtctrl.yaw = fl_yaw;
		djisdk_state.oes_fc_controled |= 1 << fc_obstacle_b;
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	} else {
		if (	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1 << fc_obstacle_b);
		if (flightData_zkrtctrl.x)
			flightData_zkrtctrl.x = 0;
		if (flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
}
#endif
/**
*   @brief  sys_ctrl_timetask 系统定时任务，秒级
  * @parm   none
  * @retval none
  */
int temp_sct_i;
void sys_ctrl_timetask(void) {
	//handle ostacle module timetask
	for (temp_sct_i = 0; temp_sct_i < 4; temp_sct_i++) {
		if (GuidanceObstacleData.constant_speed_time[temp_sct_i]--) {
			if (!GuidanceObstacleData.constant_speed_time[temp_sct_i]) {
				GuidanceObstacleData.constant_speed_time_flag[temp_sct_i] = OCS_NO_CONTROL;
			}
		}
//		if(GuidanceObstacleData.obstacle_time[temp_sct_i]--)
//		{
//			if(!GuidanceObstacleData.obstacle_time[temp_sct_i])
//			{
//				GuidanceObstacleData.obstacle_time_flag[temp_sct_i] = 0;
//			}
//		}
	}
	if(nuclear_r_d_s.peroid_upload_cnt)
	{
		nuclear_r_d_s.peroid_upload_cnt--;
		if(!nuclear_r_d_s.peroid_upload_cnt)
			nuclear_r_d_s.peroid_upload_flag = 1;
	}
}
