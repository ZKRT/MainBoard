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
#include "stm32f4xx.h"
#include "main.h"
#include "djiCtrl.h"
#include "QtoAngle.h"
#ifdef __cplusplus
extern "C"
{
#include "sys.h"
#include "ostmr.h"
#include "undercarriageCtrl.h"
#include "heartBeatHandle.h"
#include "mobileDataHandle.h"
}
#endif
#include <math.h>

extern Vehicle  vehicle;
extern Vehicle* v;
extern Control::CtrlData flightData_zkrtctrl;

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

extern "C" void sendToMobile(uint8_t *data, uint8_t len);

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
#define FCC_TIMEROUT         1  //20ms //zkrt_todo : 发送周期待测试
#define GETFDATA_TIMEROUT    10  //200ms
volatile u16 fc_timercnt = FCC_TIMEROUT;//飞控周期控制时钟计数
volatile u16 getfdata_timercnt = GETFDATA_TIMEROUT;//周期获取飞行数据时钟计数

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  DJI init. 大疆SDK初始化
  * @param  None
  * @retval None
  */
int dji_init(void)
{
	delay_nms(5000); 
/*(1): dji requirement	*/
	
	// Check USART communication
	if (!v->protocolLayer->getDriver()->getDeviceStatus())
	{
		printf("USART communication is not working.\r\n");
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

	// Check if the firmware version is compatible with this OSDK version
	if (v->getFwVersion() < extendedVersionBase &&
	v->getFwVersion() != Version::M100_31)
	{
		printf("Upgrade firmware using Assistant software!\n");
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
void dji_process(void)
{			
	switch(djisdk_state.run_status)
	{
		case init_none_djirs:
			djisdk_state.run_status = set_avtivate_djirs;
			djisdk_state.cmdres_timeout = TimingDelay;
//>>>>>dji oes standby work start			
      userActivate(); 																	                        //(1)
			// Verify subscription
			if (v->getFwVersion() != Version::M100_31)                                //(2)
			{
				v->subscribe->verify();
				delay_nms(500);
			}
			// Re-set Broadcast frequencies to their default values
			ACK::ErrorCode ack = v->broadcast->setBroadcastFreqDefaults(20);	    //(3)
      delay_nms(50);
			
		 // Mobile Callback set                                                     //(4)
			v->moc->setFromMSDKCallback(parseFromMobileCallback_v2, (UserData)djidataformmobile);
			delay_nms(50);
//>>>>>dji oes standby work end
			break;
		case set_avtivate_djirs:
			if(djisdk_state.cmdres_timeout - TimingDelay >= 5000) //激活不成功时，每5秒重新激活一次
			{
				djisdk_state.run_status = init_none_djirs;
//				heartbeat_ctrl(); //zkrt_debug
			}
			break;
		case avtivated_ok_djirs:
			dji_flight_ctrl();  //飞行控制
		  get_flight_data_and_handle(); //飞行数据处理
		  heartbeat_ctrl();  //心跳包定时处理
			break;
		default:
			break;
	}
}
/**
  * @brief  dji_flight_ctrl
  * @param  None
  * @retval None
  */
void dji_flight_ctrl(void)
{
  Telemetry::Status         status;
  Telemetry::RC             rc;
	Telemetry::SDKInfo        sdkinfo;
	
	status         = v->broadcast->getStatus();
	rc             = v->broadcast->getRC();
	sdkinfo        = v->broadcast->getSDKInfo();
	
	if(rc.mode <=0) //如果遥控器档位在P档以外，飞行控制跳过  //zkrt_todo: wait test
		return;
	
	if(((djisdk_state.oes_fc_controled)||(djisdk_state.last_fc_controled))
		&&((fc_timercnt == 0)))   //周期发送飞行控制命令
	//zkrt_notice: 文档建议是20ms周期控制，依文档所说来控制。https://developer.dji.com/cn/onboard-sdk/documentation/application-development-guides/programming-guide.html
	{
		fc_timercnt = FCC_TIMEROUT;
	}
	else
		return;
	
	if(djisdk_state.oes_fc_controled)
	{
		if(status.flight == 2) //飞行控制只在飞行状态==in_air_motor_on时才能使用
		{
			if(sdkinfo.deviceStatus!=2)  //OES掌握控制权限时=DEVICE_SDK //控制信息广播频率设置为50hz即20ms更新一次，这里控制信息发送频率为40ms，理论上不会有控制信息延迟更新影响此处逻辑的情况。
			{
        // Obtain Control Authority
        v->obtainCtrlAuthority();
				ZKRT_LOG(LOG_INOTICE, "oes setControl\n");
			}
//			printf("x=%f,y=%f\n,%x", flightData_zkrtctrl.x, flightData_zkrtctrl.y, flightData_zkrtctrl.flag); //zkrt_debug
			v->control->flightCtrl(flightData_zkrtctrl);
			ZKRT_LOG(LOG_NOTICE, "oes flight control=================\r\n")			
		}
	}
	else
	{
		if(sdkinfo.deviceStatus==2)  //OES掌握控制权限时=DEVICE_SDK
		{
			v->releaseCtrlAuthority();
		  ZKRT_LOG(LOG_INOTICE, "oes control closed\n");			
		}
	}
	
	if(djisdk_state.last_fc_controled != djisdk_state.oes_fc_controled)
		djisdk_state.last_fc_controled = djisdk_state.oes_fc_controled;  //置上次控制值=当前控制值
}
/**
  * @brief  get_flight_data_and_handle
  * @param  None
  * @retval None
  */
void get_flight_data_and_handle(void)
{
	if(getfdata_timercnt == 0)
	{
		getfdata_timercnt = GETFDATA_TIMEROUT;
	}
	else
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

	if(!undercarriage_data.uce_autoenabled)
		return;
	
	if(f_act_height > UDCAIE_TRIGGER_UP_HEIGHT)
	{
		undercarriage_data.state_bya3height = uped_udcaie_rs;
	}
	else if(f_act_height < UDCAIE_TRIGGER_DOWN_HEIGHT)
	{
		undercarriage_data.state_bya3height = downed_udcaie_rs;
	}
}
/**
  * @brief  heartbeat_ctrl 心跳包处理
  * @param  None
  * @retval None
  */
void heartbeat_ctrl(void)
{
	if(zkrt_heartbeat_pack())
	{
		sendToMobile((uint8_t*)&_zkrt_packet_hb, 50);
//		int k; //zkrt_debug
//		printf("heart start_code=0x %x\r\n",_zkrt_packet_hb.start_code);
//		for(k=0; k<30; k++)
//			printf("%x ",_zkrt_packet_hb.data[k]);
//		printf("\r\n");
	}
}
/**
  * @brief  djizkrt_timer_task, by ostmr.c module
  * @param  None
  * @retval None
  */
void djizkrt_timer_task(void)
{
  if(fc_timercnt)
	{
		fc_timercnt--;
	}	
  if(getfdata_timercnt)
	{
		getfdata_timercnt--;
	}
}
/**
  * @brief  djizkrt_timer_task, by ostmr.c module
  * @param  None
  * @retval None
  */
void dji_get_roll_pitch(double* roll, double* pitch)
{
	//四元数转换而来的姿态角
	*roll = getRoll(v->broadcast->getQuaternion());
	*pitch = getPitch(v->broadcast->getQuaternion());
}
/**
  * @}
  */ 
/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

