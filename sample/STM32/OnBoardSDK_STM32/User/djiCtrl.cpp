/**
  ******************************************************************************
  * @file    djiCtrl.cpp
  * @author  ZKRT
  * @version V0.0.1
  * @date    15-March-2017
  * @brief   dji_ctrl communication with the a3 flight controller
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
#include "djiCtrl.h"
#include "QtoAngle.h"

#ifdef CPLUSPLUS_HANDLE_ZK
extern "C" {
#endif

#include "sys.h"
#include "ostmr.h"
#include "undercarriageCtrl.h"
#include "heartBeatHandle.h"
#include "mobileDataHandle.h"
#include "obstacleAvoid.h"
#include "led.h"

#ifdef CPLUSPLUS_HANDLE_ZK
}
#endif

#include <math.h>

extern Vehicle  vehicle;
extern Vehicle* v;
extern Control::CtrlData flightData_zkrtctrl;

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

#ifdef CPLUSPLUS_HANDLE_ZK
extern "C" void sendToMobile(uint8_t *data, uint8_t len);
#else
extern void sendToMobile(uint8_t *data, uint8_t len);
#endif

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void dji_flight_ctrl(void);
void djizkrt_timer_task(void);
void get_flight_data_and_handle(void);
void heartbeat_ctrl(void);

/* Private variables ---------------------------------------------------------*/

/*----------ZKRT VARIABLE----------*/
dji_sdk_status djisdk_state = {init_none_djirs, 0, 0xffffffff, 0, 0};  //dji sdk 运行状态

//#define FCC_TIMEROUT         10  //200ms
#define FCC_TIMEROUT         1  //20ms //
#define GETFDATA_TIMEROUT    10  //200ms
volatile u16 fc_timercnt = FCC_TIMEROUT;//飞控周期控制时钟计数
volatile u16 getfdata_timercnt = GETFDATA_TIMEROUT;//周期获取飞行数据时钟计数
int init_dji_cnt=0; //>10 reset
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  DJI init. 大疆SDK初始化
  * @param  None
  * @retval None
  */
int dji_init(void) {
	_RUN_LED = 0;  //add by yanly
	delay_nms(5000);
	/*(1): dji requirement	*/

	// Check USART communication
	if (!v->protocolLayer->getDriver()->getDeviceStatus()) {
		ZKRT_LOG(LOG_ERROR, "USART communication is not working.\r\n");
		__set_FAULTMASK(1);
		NVIC_SystemReset();
		delete (v);
		return -1;
	}
	printf("\nPrerequisites:\r\n");
	printf("1. Vehicle connected to the Assistant and simulation is ON\r\n");
	printf("2. Battery fully chanrged\r\n");
	printf("3. DJIGO App connected (for the first time run)\r\n");
	printf("4. Gimbal mounted if needed\r\n");

	delay_nms(30);

	//! Initialize functional Vehicle components like
	//! Subscription, Broabcast, Control, Camera, etc
	v->functionalSetUp();
	delay_nms(500);

	if (v->getFwVersion() == 0) {
		ZKRT_LOG(LOG_ERROR, "version unmath, Upgrade firmware using Assistant software!\n");
		__set_FAULTMASK(1);
		NVIC_SystemReset();
		delete (v);
		return -1;
	}

	// Check if the firmware version is compatible with this OSDK version
	if (v->getFwVersion() > 0 &&
	        v->getFwVersion() < extendedVersionBase &&
	        v->getFwVersion() != Version::M100_31) {
		ZKRT_LOG(LOG_ERROR, "version unmath, Upgrade firmware using Assistant software!\n");
		__set_FAULTMASK(1);
		NVIC_SystemReset();
		delete (v);
		return -1;
	}

//	userActivate();   //not need by yanly modify
//	delay_nms(500);
//	/*ACK::ErrorCode ack = waitForACK();
//	if(ACK::getError(ack))
//	{
//		ACK::getErrorCodeMessage(ack, func);
//	}*/

//	// Verify subscription
//	if (v->getFwVersion() != Version::M100_31)
//	{
//		v->subscribe->verify();
//		delay_nms(500);
//	}
//
//	// Obtain Control Authority
//	v->obtainCtrlAuthority();	      //not need by yanly modify

	delay_nms(1000);

	/*(2)zkrt funciton*/
	t_ostmr_insertTask(djizkrt_timer_task, 20, OSTMR_PERIODIC);   //20ms task

	return 1;
}
/**
  * @brief  dji_process.  DJI 流程
  * @param  None
  * @retval None
  */
//uint8_t subcribed_nuclear = 0; //zkrt_nuclear
void dji_process(void) {
	switch (djisdk_state.run_status) {
	case init_none_djirs:
		init_dji_cnt++;
		djisdk_state.run_status = set_avtivate_djirs;
		djisdk_state.cmdres_timeout = TimingDelay;
//>>>>>dji oes standby work start
		userActivate(); 																	                        //(1)
		// Verify subscription
		if (v->getFwVersion() != Version::M100_31) {                              //(2)
			v->subscribe->verify();
			delay_nms(500);
		}
		// Re-set Broadcast frequencies to their default values
		ACK::ErrorCode ack = v->broadcast->setBroadcastFreqDefaults(2);	    //(3)
		delay_nms(50);
		//Broadcast Callback set
//			v->broadcast->setUserBroadcastCallback(djiBroadcastCallback, &dji_broaddata);  //zkrt_test
//			delay_nms(10);
		// Mobile Callback set                                                     //(4)
		v->moc->setFromMSDKCallback(parseFromMobileCallback_v2, (UserData)&msg_handlest);
		delay_nms(50);
//>>>>>dji oes standby work end
		break;
	case set_avtivate_djirs:
		if (djisdk_state.cmdres_timeout - TimingDelay >= 5000) { //激活不成功时，每5秒重新激活一次
			djisdk_state.run_status = init_none_djirs;
			v->protocolLayer->sendPoll();
			if(init_dji_cnt>5)
			{
				ZKRT_LOG(LOG_ERROR, "avtive dji faile exceed 10 times,reset program!\n");
				__set_FAULTMASK(1);
				NVIC_SystemReset();
				delete (v);
				return;				
			}
		}
		break;
	case avtivated_ok_djirs:
		dji_flight_ctrl();  //飞行控制
		get_flight_data_and_handle(); //飞行数据处理
		heartbeat_ctrl();  //心跳包定时处理 
		v->protocolLayer->sendPoll(); //add by yanly190220
//		if(!subcribed_nuclear){v
//			subcribed_nuclear = 1;
//			setUpSubscription(v); //zkrt_nuclear 采用订阅方式获取GPS数据
//		}
		break;
	default:
		break;
	}
}
///**
//  * @brief  dji_flight_ctrl
//  * @param  None
//  * @retval None
//  */
//void dji_flight_ctrl(void)
//{
//  Telemetry::Status         status;
//  Telemetry::RC             rc;
//	Telemetry::SDKInfo        sdkinfo;
//
//	status         = v->broadcast->getStatus();
//	rc             = v->broadcast->getRC();
//	sdkinfo        = v->broadcast->getSDKInfo();
//
//	if(rc.mode <=0) //如果遥控器档位在P档以外，飞行控制跳过
//		return;
//
//	if(((djisdk_state.oes_fc_controled)||(djisdk_state.last_fc_controled))
//		&&((fc_timercnt == 0)))   //周期发送飞行控制命令
//	//zkrt_notice: 文档建议是20ms周期控制，依文档所说来控制。https://developer.dji.com/cn/onboard-sdk/documentation/application-development-guides/programming-guide.html
//	{
//		fc_timercnt = FCC_TIMEROUT;
//	}
//	else
//		return;
//
//	if(djisdk_state.oes_fc_controled)
//	{
//		if(status.flight == 2) //飞行控制只在飞行状态==in_air_motor_on时才能使用
//		{
//			if(sdkinfo.deviceStatus!=2)  //OES掌握控制权限时=DEVICE_SDK //控制信息广播频率设置为50hz即20ms更新一次，这里控制信息发送频率为40ms，理论上不会有控制信息延迟更新影响此处逻辑的情况。
//			{
//        // Obtain Control Authority
//        v->obtainCtrlAuthority();
//				ZKRT_LOG(LOG_INOTICE, "oes setControl\n");
//			}
//			v->control->flightCtrl(flightData_zkrtctrl);
//			ZKRT_LOG(LOG_NOTICE, "oes flight control=================\r\n")
//		}
//	}
//	else
//	{
//		if(sdkinfo.deviceStatus==2)  //OES掌握控制权限时=DEVICE_SDK
//		{
//			v->releaseCtrlAuthority();
//		  ZKRT_LOG(LOG_INOTICE, "oes control closed\n");
//		}
//	}
//
//	if(djisdk_state.last_fc_controled != djisdk_state.oes_fc_controled)
//		djisdk_state.last_fc_controled = djisdk_state.oes_fc_controled;  //置上次控制值=当前控制值
//}
/**
  * @brief  dji_flight_ctrl
  * @param  None
  * @retval None
  */
char fc_mission_b_flag=0;
void dji_flight_ctrl(void) {
	Telemetry::Status         status;
	Telemetry::RC             rc;
	Telemetry::SDKInfo        sdkinfo;

	status         = v->broadcast->getStatus();
	rc             = v->broadcast->getRC();
	sdkinfo        = v->broadcast->getSDKInfo();

	//检查遥控器模式
	if (rc.mode <= 0)
		return;

	//检查是否航向任务
	if(djisdk_state.oes_fc_controled&(1 << fc_mission_b)) 
	{
		if(!fc_mission_b_flag)
		{
			v->obtainCtrlAuthority();
			fc_mission_b_flag = 1;
		}
	}
	
	//检查飞行状态
	if (status.flight != 2)
		return;

	//检测OES是否需要飞控控制权
	if (!djisdk_state.oes_fc_controled) {
		if (sdkinfo.deviceStatus == 2) {
			v->releaseCtrlAuthority();
			ZKRT_LOG(LOG_INOTICE, "oes control closed\n");
		}
		return;
	}

	//检查是否需要有基础飞行操作
	if((djisdk_state.oes_fc_controled&(1 << fc_tempctrl_b))
		||(djisdk_state.oes_fc_controled&(1 << fc_obstacle_b)))
	{
	//检查飞行周期
		if ((djisdk_state.oes_fc_controled) && ((fc_timercnt == 0))) //周期发送飞行控制命令
			//zkrt_notice: 文档建议是20ms周期控制，依文档所说来控制。https://developer.dji.com/cn/onboard-sdk/documentation/application-development-guides/programming-guide.html
		{
			fc_timercnt = FCC_TIMEROUT;
		} else
			return;	
	//	ZKRT_LOG(LOG_DEBUG, "x=%f,y=%f\n,%x", flightData_zkrtctrl.x, flightData_zkrtctrl.y, flightData_zkrtctrl.flag);
		
		if (sdkinfo.deviceStatus != 2) { //检查当前OES的控制权
			v->obtainCtrlAuthority();
			ZKRT_LOG(LOG_INOTICE, "sdkinfo.deviceStatus=%d, oes setControl\n", sdkinfo.deviceStatus);
		}
		else
		{
			v->control->flightCtrl(flightData_zkrtctrl);
			ZKRT_LOG(LOG_NOTICE, "oes flight control=================\r\n");
		}
	}
}
/**
  * @brief  get_flight_data_and_handle
  * @param  None
  * @retval None
  */
void get_flight_data_and_handle(void) {
	if (getfdata_timercnt == 0) {
		getfdata_timercnt = GETFDATA_TIMEROUT;
	} else
		return;

//	Telemetry::Status         status;
	Telemetry::GlobalPosition globalPosition;   //do
//  Telemetry::RC             rc;
//  Telemetry::Vector3f       velocity;
//  Telemetry::Quaternion     quaternion;
//	Telemetry::SDKInfo        sdkinfo;

//	status         = v->broadcast->getStatus();
	globalPosition = v->broadcast->getGlobalPosition();  //do
//	rc             = v->broadcast->getRC();
//	velocity       = v->broadcast->getVelocity();
//	quaternion     = v->broadcast->getQuaternion();
//	sdkinfo        = v->broadcast->getSDKInfo();

//	double yaw;
//	double roll;
//	double pitch;
//  float x_body, y_body;
	float32_t f_act_height; //do
	f_act_height = globalPosition.height;   //do
//	yaw = getYaw(quaternion);
//	roll = getRoll(quaternion);
//	pitch = getPitch(quaternion);
//	x_body = velocity.x*cos(yaw)+velocity.y*sin(yaw);
//	y_body = -velocity.x*sin(yaw)+velocity.y*cos(yaw);

//	printf("height:%f\n", f_act_height);
//	//机体高度，非海拔高度
//	printf("controlDevice:%d\n", sdkinfo.deviceStatus);
//	//飞控控制权限 0->rc  1->app  2->serial
//	printf("flightstatus:%d\n", status.flight);
//	//飞行运行状态 0-stop,1-解锁,2-起飞
//	printf("rc yaw=%d, roll=%d, pitch=%d, throttle=%d, gear=%d, mode=%d\n", rc.yaw, rc.roll, rc.pitch, rc.throttle, rc.gear, rc.mode);
//	//遥控器摇杆值 mode：P~10000, A~0, F~-10000 yaw-右正左负，roll-右正左负，pitch-前正后负
//	printf("ground vel x=%f, y=%f, z=%f\n", velocity.x, velocity.y, velocity.z);
//	//这个是大地坐标系速度   //实测飞机P档控制最大速度13m/s左右 x-前正后负，y-右正左负，z-上正下负
//	printf("qyaw=%f, qroll=%f, qpitch=%f\n", yaw, roll, pitch);
//	//四元数转换而来的姿态角 yaw-右正左负，roll-右正左负，pitch-前负后正
//  printf("body vel x=%f, y=%f\n", x_body, y_body);
//	//这个是机体坐标系速度 x-前正后负，y-右正左负

	if (!undercarriage_data.uce_autoenabled)
		return;

	if (f_act_height > UDCAIE_TRIGGER_UP_HEIGHT) {
		undercarriage_data.state_bya3height = uped_udcaie_rs;
	} else if (f_act_height < UDCAIE_TRIGGER_DOWN_HEIGHT) {
		undercarriage_data.state_bya3height = downed_udcaie_rs;
	}
}
/**
  * @brief  heartbeat_ctrl 心跳包处理
  * @param  None
  * @retval None
  */
void heartbeat_ctrl(void) {
	zd_heartv3_3_st *hb;
	float gas_v[8];
	int i;
	if (zkrt_heartbeat_pack(msg_handlest.data_send_app, &msg_handlest.datalen_sendapp)) {
		sendToMobile(msg_handlest.data_send_app, msg_handlest.datalen_sendapp);

		hb = (zd_heartv3_3_st*)(msg_handlest.data_send_app + ZK_HEADER_LEN);
		for (i = 0; i < 8; i++)
			memcpy(&gas_v[i], &hb->gas.gas_value[i], 4);
		ZKRT_LOG(LOG_DEBUG, "online[%x], feedback[%x], temper[v %x,s %x,h %x,l %x], distance[f %d, r %d, b %d, l %d, enbale %d, effec %d, vel %d], gas[num %d, , ch0:%f, ch1:%f, ch2:%f, ch3:%f, ch4:%f, ch5:%f, ch6:%f, ch7:%f]\n",
		         hb->dev.dev_online_s, hb->dev.feedback_s, hb->temper.t_value, hb->temper.t_status, hb->temper.t_high, hb->temper.t_low,
		         hb->obstacle.ob_distse_v[1], hb->obstacle.ob_distse_v[2], hb->obstacle.ob_distse_v[3], hb->obstacle.ob_distse_v[4], hb->obstacle.avoid_ob_enabled, hb->obstacle.avoid_ob_distse, hb->obstacle.avoid_ob_velocity,
		         hb->gas.ch_num, gas_v[0], gas_v[1], gas_v[2], gas_v[3], gas_v[4], gas_v[5], gas_v[6], gas_v[7]);
	}
}
/**
  * @brief  djizkrt_timer_task, by ostmr.c module
  * @param  None
  * @retval None
  */
void djizkrt_timer_task(void) {
	if (fc_timercnt) {
		fc_timercnt--;
	}
	if (getfdata_timercnt) {
		getfdata_timercnt--;
	}
}
/**
  * @brief  dji_get_roll_pitch
  * @param  None
  * @retval None
  */
void dji_get_roll_pitch(double* roll, double* pitch) {
	//四元数转换而来的姿态角
	*roll = getRoll(v->broadcast->getQuaternion());
	*pitch = getPitch(v->broadcast->getQuaternion());
}
/**
  * @brief  dji_get_flight_parm
  * @param  None
  * @retval None
  */
void dji_get_flight_parm(void *vdfs) {
	Telemetry::GlobalPosition globalPosition;
	Telemetry::RC             rc;
	Telemetry::Vector3f       velocity;
	Telemetry::Quaternion     quaternion;

	globalPosition = v->broadcast->getGlobalPosition();
	rc             = v->broadcast->getRC();
	velocity       = v->broadcast->getVelocity();
	quaternion     = v->broadcast->getQuaternion();

	dji_flight_status *dfs = (dji_flight_status *)vdfs;
	dfs->roll = getRoll(v->broadcast->getQuaternion());
	dfs->pitch = getPitch(v->broadcast->getQuaternion());
	dfs->rc_pitch = rc.pitch;
	dfs->rc_roll = rc.roll;
	dfs->rc_throttle = rc.throttle;
	dfs->rc_yaw = rc.yaw;
	dfs->xnow = velocity.x * cos(getYaw(quaternion)) + velocity.y * sin(getYaw(quaternion));
	dfs->ynow = -velocity.x * sin(getYaw(quaternion)) + velocity.y * cos(getYaw(quaternion));
	dfs->height = globalPosition.height;
}
/**
  * @brief  get_limit_vx
  * @param  None
  * @retval None
  */
float get_limit_vx(uint8_t dir) {
	float vel_limiting = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
	float lv = djif_status.xnow;

	if (lv > vel_limiting)
		lv = vel_limiting;
	else if (lv < -vel_limiting)
		lv = -vel_limiting;
	else {
		if (lv >= 0)
			lv = (lv + vel_limiting + lv) / 3;
		else
			lv = (lv + lv - vel_limiting) / 3;
	}

	if ((dir == GE_DIR_FRONT) && (lv < 0))
		lv = vel_limiting;
	if ((dir == GE_DIR_BACK) && (lv > 0))
		lv = -vel_limiting;

	return lv;
}
/**
  * @brief  get_limit_vy
  * @param  None
  * @retval None
  */
float get_limit_vy(uint8_t dir) {
	float vel_limiting = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
	float lv = djif_status.ynow;

	if (lv > vel_limiting)
		lv = vel_limiting;
	else if (lv < -vel_limiting)
		lv = -vel_limiting;
	else {
		if (lv >= 0)
			lv = (lv + lv + vel_limiting) / 3;
		else
			lv = (lv + lv - vel_limiting) / 3;
	}

	if ((dir == GE_DIR_RIGHT) && (lv < 0))
		lv = vel_limiting;
	if ((dir == GE_DIR_LEFT) && (lv > 0))
		lv = -vel_limiting;

	return lv;
}
/**
  * @}
  */
/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

