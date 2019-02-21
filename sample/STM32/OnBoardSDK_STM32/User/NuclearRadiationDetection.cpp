////////////////////////////////////////////////////////include
#include "hw_config.h"
#include "djiCtrl.h"
#include "dji_vehicle.hpp"
#include "NuclearRadiationDetection.h"

#ifdef CPLUSPLUS_HANDLE_ZK
extern "C"
{
#endif
#include "osusart.h"
#include "osqtmr.h"
#include "zkrt.h"
#include "led.h"
	//#include "NuclearRadiationDetection.h"

#ifdef CPLUSPLUS_HANDLE_ZK
}
#endif
////////////////////////////////////////////////////////variables
////dji variables
using namespace DJI::OSDK;
extern Vehicle vehicle;
extern Vehicle *v;
extern dji_sdk_status djisdk_state;
////timeout task handle variables
nrd_st nuclear_r_d_s;
////protocol data struct variables
#pragma pack(1)
typedef struct
{
	Telemetry::Quaternion q;
	Telemetry::GlobalPosition g;
	Telemetry::Battery b;
} GpsData;
#pragma pack()
////callback handle struct variables
typedef struct{
	uint8_t cmd;
}NrdCbUserData;
typedef struct{
	uint8_t buf[200];
	uint16_t len;
	uint8_t flag;
	NrdCbUserData udata;
}NrdCbHandle;
volatile NrdCbHandle nrd_cb_handle_st={{0},0,0,{0}};

////////////////////////////////////////////////////////static fun
static void nrd_recv_handle(void);
static void nrd_period_send(void);
static void nrd_callback_respond_to_pc(void);
static uint8_t cmd_len_of_pc_control_data(uint8_t cmd);
static uint8_t cmd_len_of_respond_pc_control_data(uint8_t cmd);
static uint8_t nrd_cb_common_handle(uint16_t len);
static bool is_waypoint_mission_interface_can_invoke(void);
void waypoint_event_callback(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_mission_push_callback(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
static uint8_t send_data_to_pc_zhjy(uint8_t *buf, uint16_t len);
void waypoint_mission_init_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_data_upload_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_start_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_stop_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_pause_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void waypoint_resume_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void nrd_gohome_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void getwaypointsettings_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void getwaypointindex_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData);
void wapointidlevel_callback_pc(Vehicle* vehicle, RecvContainer recvFrame,UserData userData);
////////////////////////////////////////////////////////
void nuclear_radiation_detect_init(void)
{
	nuclear_r_d_s.peroid_upload_cnt = NRD_P_U_CNT+10; //init value+10
	nuclear_r_d_s.peroid_upload_flag = 0;
	djisdk_state.oes_fc_controled |= 1 << fc_mission_b; //航点任务需要获取飞机控制权, 初始化便置标记 //zkrt_important
}
//
void nuclear_radiation_detect_prcs(void)
{
	nrd_recv_handle();
	nrd_period_send();
	nrd_callback_respond_to_pc();
}
//
PcContrlDatas pc_control_datas;
uint8_t buf_nrd[200];	 //receive
uint8_t buf_res_nrd[200]; //respond
static void nrd_recv_handle(void)
{
	uint8_t ack_state = 1;
	uint8_t *buf = buf_nrd;
	uint8_t *respond_buf = buf_res_nrd;
	uint16_t buflen;
	uint8_t cmdlen;
	uint8_t need_transmit = 0;
	NrdCbUserData *_udata = (NrdCbUserData*)&nrd_cb_handle_st.udata;
	memset(buf, 0, sizeof(buf_nrd));
	if (t_osscomm_ReceiveMessage(buf, &buflen, USART2) != SCOMM_RET_OK)
	{
		return;
	}
	alarm_led_flag = TimingDelay;
	_ALARM_LED = 0;
	pc_control_datas.start_code = buf[0];
	pc_control_datas.cmd = buf[1];
	pc_control_datas.data = &buf[2];
	if (pc_control_datas.start_code != START_CODE)
	{ //check startcode
		printf("[PC ERROR]startcode\n");
		return;
	}
	cmdlen = cmd_len_of_pc_control_data(pc_control_datas.cmd);
	if (cmdlen + FIXED_FRAME_LEN != buflen)
	{ //check len
		printf("[PC ERROR]buflen:%d,but need len:%d\n", buflen, cmdlen + FIXED_FRAME_LEN);
		return;
	}
	memcpy(&pc_control_datas.check_code, buf + cmdlen + FRAME_HEAD_LEN, 2);
	memcpy(&pc_control_datas.end_code, buf + cmdlen + FRAME_HEAD_LEN + 2, 2);
	if (pc_control_datas.end_code != END_CODE)
	{ //check endcode
		printf("[PC ERROR]endcode\n");
		return;
	}
	if (crc_calculate(buf, cmdlen + FRAME_HEAD_LEN) != pc_control_datas.check_code)
	{ //check crc
		printf("[PC ERROR]crc\n");
		return;
	}
	printf("cmd:%d\n", pc_control_datas.cmd);
	switch (pc_control_datas.cmd)
	{
	case CMD_SET_GOHOME:
//1timeout handle
//	 {
//		ACK::ErrorCode ack = v->control->goHome(NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(ack);
//		printf("ack_state:%d\n", ack_state);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		pc_control_datas.data[0] = ack_state;
//		need_transmit = 1;
//	 }

//2callback handle
	{
		v->control->goHome(nrd_gohome_callback_pc, NULL);
	}
	break;
	case CMD_WAYPOINT_INIT:
//1timeout handle
//	{
//		WayPointInitSettings *fdata = (WayPointInitSettings *)pc_control_datas.data;
//		ACK::ErrorCode ack = v->missionManager->init(WAYPOINT, NUCLEAR_MISSION_TIME, &fdata);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		ack_state = (uint8_t)ack.data;
//		pc_control_datas.data[0] = ack_state;
//		need_transmit = 1;
//	}

//2callback handle	
	{
		if(!djisdk_state.oes_fc_controled)
		{
			printf("v->obtainCtrlAuthority\n");
			v->obtainCtrlAuthority(1);
			djisdk_state.oes_fc_controled |= 1 << fc_mission_b;
		}
		WayPointInitSettings *fdata = (WayPointInitSettings *)pc_control_datas.data;
		printf("N:%d,", fdata->indexNumber);
		printf("MV:%f,", fdata->maxVelocity);
		printf("IV:%f,", fdata->idleVelocity);
		printf("FA:%d,", fdata->finishAction);
//		printf("[executiveTimes]%d,", fdata->executiveTimes);
		printf("RCLostAction:%d\n", fdata->RCLostAction);
//		printf("[latitude]%f,", fdata->latitude);
//		printf("[longitude]%f,", fdata->longitude);
//		printf("[altitude]%f\n", fdata->altitude);		
		v->missionManager->init(WAYPOINT, waypoint_mission_init_callback_pc, fdata);
		v->missionManager->wpMission->setWaypointEventCallback(waypoint_event_callback, NULL);
	}
	break;
	case CMD_SINGLE_WAYPOINT:
//1timeout handle		
//	{
//		WayPointSettings *wp = (WayPointSettings *)pc_control_datas.data;
//		ACK::WayPointIndex wpack = v->missionManager->wpMission->uploadIndexData(wp, NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(wpack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpack.ack, __func__);
//		ack_state = (uint8_t)wpack.ack.data;
//		pc_control_datas.data[0] = ack_state;
//		pc_control_datas.data[1] = wpack.data.index;
//		memcpy(pc_control_datas.data + 2, (void *)&wpack.data, sizeof(WayPointSettings));
//		need_transmit = 1;
//	}
	
//2callback handle		
	{
		WayPointSettings wp;
		memcpy((void*)&wp, (void*)pc_control_datas.data, sizeof(WayPointSettings));
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			pc_control_datas.data[1] = wp.index;
			memcpy(pc_control_datas.data + 2, (void *)&wp, sizeof(WayPointSettings));
			need_transmit = 1;
		}
		else
		{
			printf("ind:%d,", wp.index);
//			printf("[latitude]%f,", wp.latitude);
//			printf("[longitude]%f,", wp.longitude);
			printf("ALE:%f,\n", wp.altitude);
//			printf("[hasAction]%d\n", wp.hasAction);		
			v->missionManager->wpMission->uploadIndexData(&wp, waypoint_data_upload_callback_pc, NULL);
		}
	}
	break;
	case CMD_START_WAYPOINT:
//1timeout handle			
//	{
//		ACK::ErrorCode ack;
//		if (pc_control_datas.data[0] == 0)
//			ack = v->missionManager->wpMission->start(NUCLEAR_MISSION_TIME);
//		else
//			ack = v->missionManager->wpMission->stop(NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		ack_state = (uint8_t)ack.data;
//		pc_control_datas.data[0] = ack_state;
//		need_transmit = 1;
//	}
//2callback handle		
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			if (pc_control_datas.data[0] == 0)
				v->missionManager->wpMission->start(waypoint_start_callback_pc, NULL);
			else
				v->missionManager->wpMission->stop(waypoint_stop_callback_pc, NULL);			
		}
	}
	break;
	case CMD_PAUSE_WAYPOINT:
//1timeout handle				
//	{
//		ACK::ErrorCode ack;
//		if (pc_control_datas.data[0] == 0)
//			ack = v->missionManager->wpMission->pause(NUCLEAR_MISSION_TIME);
//		else
//			ack = v->missionManager->wpMission->resume(NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		ack_state = (uint8_t)ack.data;
//		pc_control_datas.data[0] = ack_state;
//		need_transmit = 1;
//	}
//2callback handle
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			if (pc_control_datas.data[0] == 0)
				v->missionManager->wpMission->pause(waypoint_pause_callback_pc, NULL);
			else
				v->missionManager->wpMission->resume(waypoint_resume_callback_pc, NULL);
		}
	}
	break;
	case CMD_READ_WAYPOINT_INIT_STATUS:
//1timeout handle		
//	{
//		ACK::WayPointInit wpinitack = v->missionManager->wpMission->getWaypointSettings(NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(wpinitack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpinitack.ack, __func__);
//		ack_state = (uint8_t)wpinitack.ack.data;
//		pc_control_datas.data[0] = ack_state;
//		memcpy(pc_control_datas.data + 1, (void *)&wpinitack.data, sizeof(WayPointInitSettings));
//		need_transmit = 1;
//	}
//2callback handle
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			_udata->cmd = CMD_READ_WAYPOINT_INIT_STATUS;
			v->missionManager->wpMission->getWaypointSettings(getwaypointsettings_callback_pc, _udata);
		}
	}
//	//test code
//	{
//		WayPointInitSettings wps;
//		wps.indexNumber = 3;
//		wps.maxVelocity = 10;
//		wps.idleVelocity = 5;
//		wps.finishAction = 1;
//		wps.executiveTimes = 1;
//		wps.yawMode = 0;
//		wps.traceMode = 0;
//		wps.RCLostAction = 1;
//		wps.gimbalPitch = 0;
//		wps.latitude = 0;
//		wps.longitude = 0;
//		wps.altitude = 0;
//		memset(wps.reserved, 0x00, sizeof(wps.reserved));
//		pc_control_datas.data[0] = 0;
//		memcpy(pc_control_datas.data + 1, (void *)&wps, sizeof(WayPointInitSettings));
//		need_transmit = 1;
//	}

	break;
	case CMD_READ_SINGLE_WAYPOINT:
//1timeout handle		
//	{
//		uint8_t index = pc_control_datas.data[0];
//		ACK::WayPointIndex wpack = v->missionManager->wpMission->getIndex(index, NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(wpack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpack.ack, __func__);
//		ack_state = (uint8_t)wpack.ack.data;
//		pc_control_datas.data[0] = ack_state;
//		pc_control_datas.data[1] = wpack.data.index;
//		memcpy(pc_control_datas.data + 2, (void *)&wpack.data, sizeof(WayPointSettings));
//		need_transmit = 1;
//	}
//2callback handle
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			uint8_t index = pc_control_datas.data[0];
			_udata->cmd = CMD_READ_SINGLE_WAYPOINT;
			v->missionManager->wpMission->getIndex(index, getwaypointindex_callback_pc, _udata);
		}
	}
	
//	//test code
//	{
//		WayPointSettings wp={0};
//		wp.index = 1;
//		wp.latitude = 23.0001;
//		wp.longitude = 112.0001;
//		wp.altitude = 100;
//		wp.damping = 0;
//		wp.yaw = 0;
//		wp.gimbalPitch = 0;
//		wp.turnMode = 0;
//		for (int i = 0; i < 8; ++i)
//		{
//			wp.reserved[i] = 0;
//		}
//		wp.hasAction = 0;
//		wp.actionTimeLimit = 100;
//		wp.actionNumber = 0;
//		wp.actionRepeat = 0;
//		for (int i = 0; i < 16; ++i)
//		{
//			wp.commandList[i] = 0;
//			wp.commandParameter[i] = 0;
//		}
//		pc_control_datas.data[0] = 0;
//		pc_control_datas.data[1] = 1;
//		memcpy(pc_control_datas.data + 2, (void *)&wp, sizeof(WayPointSettings));
//		need_transmit = 1;
//	}

	break;
	case CMD_SET_WAYPOINT_IDLE_VEL:
//1timeout handle		
//	{
//		float32_t vel;
//		memcpy(&vel, pc_control_datas.data, 4);
//		ACK::WayPointVelocity wpv = v->missionManager->wpMission->updateIdleVelocity(vel, NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(wpv.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpv.ack, __func__);
//		ack_state = (uint8_t)wpv.ack.data;
//		pc_control_datas.data[0] = ack_state;
//		memcpy(pc_control_datas.data + 1, (void *)&wpv.idleVelocity, 4);
//		need_transmit = 1;
//	}
	
//2callback handle
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			float32_t vel;
			memcpy(&vel, pc_control_datas.data, 4);
			_udata->cmd = CMD_SET_WAYPOINT_IDLE_VEL;
			v->missionManager->wpMission->updateIdleVelocity(vel, wapointidlevel_callback_pc, _udata);
		}
	}
	
	break;
	case CMD_GET_WYAPOINT_IDLE_VEL:
//1timeout handle
//	{//SDK没有返回vel，所以暂时不做 //zkrt_notice
//		ACK::ErrorCode ack = v->missionManager->wpMission->readIdleVelocity(NUCLEAR_MISSION_TIME);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		pc_control_datas.data[0] = ack_state;
//		memcpy(pc_control_datas.data + 1, (void *)&wpv.data, 4);
//		need_transmit = 1;
//	}
	
//2callback handle
	{
		if(is_waypoint_mission_interface_can_invoke()==false)
		{
			//wpMission not init
			pc_control_datas.data[0] = DJI::OSDK::ErrorCode::MissionACK::Common::NOT_INITIALIZED;
			need_transmit = 1;
		}
		else
		{
			_udata->cmd = CMD_GET_WYAPOINT_IDLE_VEL;
			v->missionManager->wpMission->readIdleVelocity(wapointidlevel_callback_pc, _udata);
		}
	}
	
	break;
	case CMD_TEST_1:	//test code 
//	{
//		ACK::ErrorCode ack;
//		float64_t increment = 0.000001;
//		float64_t extAngle = 2 * M_PI / 3;
//		printf("v->obtainCtrlAuthority\n");
//		djisdk_state.oes_fc_controled |= 1 << fc_mission_b;
//		//get control
//		ack = v->obtainCtrlAuthority(1);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//	//		v->obtainCtrlAuthority(0);
//		
//	//		//try stop last mission 
//	//		printf("v->missionManager->wpMission->stop\n");
//	//		ack = v->missionManager->wpMission->stop(2);
//	//		ack_state = ACK::getError(ack);
//	//		if (ack_state)
//	//			ACK::getErrorCodeMessage(ack, __func__);		
//		
//		//init setting
//		WayPointInitSettings wps;
//		wps.indexNumber = 3;
//		wps.maxVelocity = 10;
//		wps.idleVelocity = 5;
//		wps.finishAction = 0;
//		wps.executiveTimes = 1;
//		wps.yawMode = 0;
//		wps.traceMode = 0;
//		wps.RCLostAction = 1;
//		wps.gimbalPitch = 0;
//		wps.latitude = 0;
//		wps.longitude = 0;
//		wps.altitude = 0;
//		memset(wps.reserved, 0x00, sizeof(wps.reserved));
//		printf("v->missionManager->init\n");
//		ack = v->missionManager->init(WAYPOINT, 1, &wps);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//		v->missionManager->printInfo();
//		printf("setWaypointEventCallback\n");
//		v->missionManager->wpMission->setWaypointEventCallback(waypoint_event_callback, NULL);
//	//		v->missionManager->wpMission->setWaypointCallback(waypoint_mission_push_callback, NULL);
//		//init waypoint per point
//		WayPointSettings wp={0};
//		wp.index = 0;
//		wp.latitude = v->broadcast->getGlobalPosition().latitude;
//		wp.longitude = v->broadcast->getGlobalPosition().longitude;
//		wp.altitude = 10;
//		wp.damping = 0;
//		wp.yaw = 0;
//		wp.gimbalPitch = 0;
//		wp.turnMode = 0;
//		for (int i = 0; i < 8; ++i)
//		{
//			wp.reserved[i] = 0;
//		}
//		wp.hasAction = 0;
//		wp.actionTimeLimit = 100;
//		wp.actionNumber = 0;
//		wp.actionRepeat = 0;
//		for (int i = 0; i < 16; ++i)
//		{
//			wp.commandList[i] = 0;
//			wp.commandParameter[i] = 0;
//		}
//		printf("0v->missionManager->wpMission->uploadIndexData\n");
//		ACK::WayPointIndex wpack;
//		wpack = v->missionManager->wpMission->uploadIndexData(&wp, 1);
//	//		v->missionManager->wpMission->uploadIndexData(&wp);
//		ack_state = ACK::getError(wpack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpack.ack, __func__);
//		//next point
//		wp.index = 1;
//		wp.latitude  = (wp.latitude + (increment * cos(wp.index * extAngle)));
//		wp.longitude = (wp.longitude + (increment * sin(wp.index * extAngle)));
//		wp.altitude  = (wp.altitude + 1);
//	//		wp.latitude = 0.4014379453;
//	//		wp.longitude = 1.9547617809;
//	//		wp.altitude = 11;
//		printf("1v->missionManager->wpMission->uploadIndexData\n");
//		wpack = v->missionManager->wpMission->uploadIndexData(&wp, 1);
//	//		v->missionManager->wpMission->uploadIndexData(&wp);
//		ack_state = ACK::getError(wpack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpack.ack, __func__);
//		//next point
//		wp.index = 2;
//		wp.latitude  = (wp.latitude + (increment * cos(wp.index * extAngle)));
//		wp.longitude = (wp.longitude + (increment * sin(wp.index * extAngle)));
//		wp.altitude  = (wp.altitude + 1);
//		printf("2v->missionManager->wpMission->uploadIndexData\n");
//		wpack = v->missionManager->wpMission->uploadIndexData(&wp, 2);
//	//		v->missionManager->wpMission->uploadIndexData(&wp);
//		ack_state = ACK::getError(wpack.ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(wpack.ack, __func__);
//		//start waypoint
//		printf("v->missionManager->wpMission->start\n");
//	//		v->missionManager->wpMission->start();
//		ack = v->missionManager->wpMission->start(1);
//		ack_state = ACK::getError(ack);
//		if (ack_state)
//			ACK::getErrorCodeMessage(ack, __func__);
//	}		

	break;	
	default:
	break;
	}
	if (need_transmit)
	{
		/**pack packet**/
		//get cmd length
		cmdlen = cmd_len_of_respond_pc_control_data(pc_control_datas.cmd);
		memcpy(respond_buf, &pc_control_datas.start_code, FRAME_HEAD_LEN);
		memcpy(respond_buf + FRAME_HEAD_LEN, pc_control_datas.data, cmdlen);
		//get crc
		pc_control_datas.check_code = crc_calculate(respond_buf, FRAME_HEAD_LEN + cmdlen);
		memcpy(respond_buf + FRAME_HEAD_LEN + cmdlen, &pc_control_datas.check_code, FRAME_TAIL_LEN);
//		t_osscomm_sendMessage(respond_buf, cmdlen + FIXED_FRAME_LEN, USART2);
//		h2x_led_flag = TimingDelay;
//		_HS_LED = 0;
		send_data_to_pc_zhjy(respond_buf, cmdlen + FIXED_FRAME_LEN);
	}
}
//static void nrd_recv_handle(void)
//{
//	u16 rlen;
//	u8 rbuff[30]={0};
//
//	//recv uart data
//	if(t_osscomm_ReceiveMessage(rbuff, &rlen, USART2)!= SCOMM_RET_OK)
//	{
//		return;
//	}
//	if(rlen >30)
//		return;
//	if(rbuff[0] !=0x5a)
//		return;
//	switch(rbuff[1])
//	{
//		case 0x04:  //返航
//			if((rbuff[2] == 0xff)&&(rbuff[3] == 0xff)&&(rbuff[4] == 0x0d)&&(rbuff[5] == 0x0a))
//			{
//				v->control->goHome(); //excute gohome
//				//respond
//				rbuff[0] = 0x5a;
//				rbuff[1] = 0x14;
//				rbuff[2] = 'O';
//				rbuff[3] = 'K';
//				rbuff[4] = 0x0d;
//				rbuff[5] = 0x0a;
//				t_osscomm_sendMessage(rbuff, 6, USART2);
//			}
//			break;
//		default:break;
//	}
//}
//
// static void nrd_period_send(void)
// {
// 	if (nuclear_r_d_s.peroid_upload_flag)
// 	{
// 		unsigned int vol;
// 		u8 rbuff[8];
// 		rbuff[0] = 0x5a;
// 		rbuff[1] = 0x03;
// 		vol = v->broadcast->getBatteryInfo().voltage;
// 		memcpy(rbuff + 2, &vol, 4);
// 		rbuff[6] = 0x0d;
// 		rbuff[7] = 0x0a;
// 		t_osscomm_sendMessage(rbuff, 8, USART2);
// 		//reset flag
// 		nuclear_r_d_s.peroid_upload_flag = 0;
// 		nuclear_r_d_s.peroid_upload_cnt = NRD_P_U_CNT;
// 	}
// }
static void nrd_period_send(void)
{
	uint16_t cmdlen;
	uint8_t *hb_buf = buf_res_nrd;
	if (!nuclear_r_d_s.peroid_upload_flag)
		return;
	if (djisdk_state.run_status != avtivated_ok_djirs)
		return;
	pc_control_datas.start_code = START_CODE;
	pc_control_datas.cmd = CMD_UPLOAD_GPS_DATA;
	pc_control_datas.data = &hb_buf[FRAME_HEAD_LEN];
	pc_control_datas.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pc_control_datas.cmd);
	pc_control_datas.data[0] = 0; //ack
	GpsData *gpsdata = (GpsData *)&pc_control_datas.data[1];
	gpsdata->q = v->broadcast->getQuaternion();
	gpsdata->b = v->broadcast->getBatteryInfo();
	gpsdata->g = v->broadcast->getGlobalPosition(); 
//	printf("[GLOBAL]latitude:%f, longitude:%f, altitude:%f, height:%f\n", gpsdata->g.latitude, gpsdata->g.longitude, gpsdata->g.altitude, gpsdata->g.height);
	//get gps // Global position retrieved via subscription
	//	Telemetry::TypeMap<Telemetry::TOPIC_GPS_FUSED>::type subscribeGPosition;
	//    subscribeGPosition = v->subscribe->getValue<Telemetry::TOPIC_GPS_FUSED>();
	//	gpsdata->g.latitude = subscribeGPosition.latitude;
	//	gpsdata->g.longitude = subscribeGPosition.longitude;
	//	gpsdata->g.altitude = subscribeGPosition.altitude;
	//	printf("[GPSFUSED]latitude:%lf, longitude:%lf, altitude:%lf\n", gpsdata->g.latitude, gpsdata->g.longitude, gpsdata->g.altitude);
	//	Telemetry::TypeMap<Telemetry::TOPIC_ALTITUDE_FUSIONED>::type al_titude;
	//	al_titude = v->subscribe->getValue<Telemetry::TOPIC_ALTITUDE_FUSIONED>();
	//	printf("[ALTITUDEFUSED]altitude:%lf\n", al_titude);

	memcpy(hb_buf, &pc_control_datas.start_code, FRAME_HEAD_LEN);
	// memcpy(hb_buf + FRAME_HEAD_LEN, pc_control_datas.data, cmdlen); //not need because &data[0] pointer is &hb_buf[FRAME_HEAD_LEN]
	pc_control_datas.check_code = crc_calculate(hb_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(hb_buf + FRAME_HEAD_LEN + cmdlen, &pc_control_datas.check_code, FRAME_TAIL_LEN);
//	t_osscomm_sendMessage(hb_buf, cmdlen + FIXED_FRAME_LEN, USART2); 
//	h2x_led_flag = TimingDelay;
//	_HS_LED = 0;
	send_data_to_pc_zhjy(hb_buf, cmdlen + FIXED_FRAME_LEN);
	//reset timer flag
	nuclear_r_d_s.peroid_upload_flag = 0;
	nuclear_r_d_s.peroid_upload_cnt = NRD_P_U_CNT;
}
//nrd_callback_respond_to_pc
static void nrd_callback_respond_to_pc(void)
{
	if((!nrd_cb_handle_st.flag)||(!nrd_cb_handle_st.len))
		return;
	send_data_to_pc_zhjy((uint8_t*)nrd_cb_handle_st.buf, nrd_cb_handle_st.len);
	nrd_cb_handle_st.flag = 0;
	nrd_cb_handle_st.len = 0;
	memset((void*)nrd_cb_handle_st.buf, 0x00, sizeof(nrd_cb_handle_st.buf));
}
//get cmd length 0-invalid, >0 valid
static uint8_t cmd_len_of_pc_control_data(uint8_t cmd)
{
	uint8_t len = 2;
	switch (cmd)
	{
	case CMD_HIGH_PRESURE:
		len = 2;
		break;
	case CMD_NUCLEAR_UPLOAD_TIME:
		len = 2;
		break;
	case CMD_READ_BATTORY:
		len = 2;
		break;
	case CMD_SET_GOHOME:
		len = 2;
		break;
	case CMD_WAYPOINT_INIT:
		len = sizeof(WayPointInitSettings);
		break;
	case CMD_SINGLE_WAYPOINT:
		len = sizeof(WayPointSettings);
		break;
	case CMD_START_WAYPOINT:
		len = 1;
		break;
	case CMD_PAUSE_WAYPOINT:
		len = 1;
		break;
	case CMD_READ_WAYPOINT_INIT_STATUS:
		len = 1;
		break;
	case CMD_READ_SINGLE_WAYPOINT:
		len = 1;
		break;
	case CMD_SET_WAYPOINT_IDLE_VEL:
		len = 4;
		break;
	case CMD_GET_WYAPOINT_IDLE_VEL:
		len = 1;
		break;
	// case CMD_UPLOAD_GPS_DATA:
	// 	break;
	default:
		len = 0;
		break;
	}
	return len;
}
//get cmd length respond 0-invalid, >0 valid
static uint8_t cmd_len_of_respond_pc_control_data(uint8_t cmd)
{
	uint8_t len = 2;
	switch (cmd)
	{
	case CMD_HIGH_PRESURE:
		len = 1;
		break;
	case CMD_NUCLEAR_UPLOAD_TIME:
		len = 1;
		break;
	case CMD_READ_BATTORY:
		len = 1;
		break;
	case CMD_SET_GOHOME:
		len = 1;
		break;
	case CMD_WAYPOINT_INIT:
		len = 1;
		break;
	case CMD_SINGLE_WAYPOINT:
		len = 2 + sizeof(WayPointSettings);
		break;
	case CMD_START_WAYPOINT:
		len = 1;
		break;
	case CMD_PAUSE_WAYPOINT:
		len = 1;
		break;
	case CMD_READ_WAYPOINT_INIT_STATUS:
		len = 1 + sizeof(WayPointInitSettings);
		break;
	case CMD_READ_SINGLE_WAYPOINT:
		len = 2 + sizeof(WayPointSettings);
		;
		break;
	case CMD_SET_WAYPOINT_IDLE_VEL:
		len = 5;
		break;
	case CMD_GET_WYAPOINT_IDLE_VEL:
		len = 5;
		break;
	case CMD_UPLOAD_GPS_DATA:
		len = 1 + sizeof(GpsData);
		break;
	default:
		len = 0;
		break;
	}
	return len;
}
//check mission interfact can invoke?
static bool is_waypoint_mission_interface_can_invoke(void)
{
	bool can_call = false;
	
	if((v->missionManager->wayptCounter==0)||(v->missionManager->wpMission==NULL))
		return can_call;
	
	can_call = true;
	return can_call;
}
//send data to pc zhong he jiu yuan
static uint8_t send_data_to_pc_zhjy(uint8_t *buf, uint16_t len)
{
	t_osscomm_sendMessage(buf, len, USART2); 
	h2x_led_flag = TimingDelay;
	_HS_LED = 0;	
}
//send data to pc zhong he jiu yuan
static uint8_t nrd_cb_common_handle(uint16_t len)
{
	nrd_cb_handle_st.flag = 1;
	nrd_cb_handle_st.len = len;
}
//
//void pc_control_callback(DJI::OSDK::Vehicle *vehicle,
//						 DJI::OSDK::RecvContainer recvFrame,
//						 DJI::OSDK::UserData userData)
//{
//	// msg_handle_st *msgData = (msg_handle_st *)userData;
//	// uint8_t *data = msgData->data_recv_app;
//	// uint8_t *s_data = recvFrame.recvData.raw_ack_array;
//	// uint16_t datalen = recvFrame.recvInfo.len - OpenProtocol::PackageMin - 2; // minus cmd id and cmd set size //zkrt_test

//	// if ((datalen >= ZK_FIXED_LEN) && (datalen <= ZK_MAX_LEN))
//	// {
//	// 	memcpy(data, s_data, datalen);
//	// 	msgData->datalen_recvapp = datalen;
//	// }
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////dji callback function
//Mission Event Push Information callback
void waypoint_event_callback(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t incident_type = recvFrame.recvData.raw_ack_array[0];
	switch(incident_type) 
	{
		case NAVI_UPLOAD_FINISH:
		{
			cmd_mission_wp_upload_incident_t *event = (cmd_mission_wp_upload_incident_t *)recvFrame.recvData.raw_ack_array;
			printf("[NAVI_UPLOAD_FINISH]is_mission_valid:%d,estimated_run_time:%d\n", event->is_mission_valid, event->estimated_run_time);
		}
		break;
		case NAVI_MISSION_FINISH:
		{
			cmd_mission_wp_finish_incident_t *event = (cmd_mission_wp_finish_incident_t *)recvFrame.recvData.raw_ack_array;
			printf("[NAVI_MISSION_FINISH]repeat:%d\n", event->repeat);
		}
		break;
		case NAVI_MISSION_WP_REACH_POINT:
		{
			cmd_mission_wp_reached_incident_t *event = (cmd_mission_wp_reached_incident_t *)recvFrame.recvData.raw_ack_array;
//			printf("[NAVI_MISSION_WP_REACH_POINT]waypoint_index:%d,current_status:%d\n", event->waypoint_index, event->current_status);
		}
		break;
	}
}
//Waypoint mission push information callback 注册此callback后，生效的周期内不断收到此callback
void waypoint_mission_push_callback(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	cmd_mission_waypoint_status_push_t *event = (cmd_mission_waypoint_status_push_t *)recvFrame.recvData.raw_ack_array;
//	printf("[waypoint_mission_push_callback]mission_type:%d,target_waypoint:%d,current_status:%d,error_notification:%d\n", 
//	event->mission_type, event->target_waypoint, event->current_status, event->error_notification);
}
//missionManager init callback by pc
void waypoint_mission_init_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	//这里ack 只有一个字节
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin == 1)
	{
		ack = recvFrame.recvData.missionACK;
	}
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_WAYPOINT_INIT;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	pcd_s.data[0] = ack;
//	printf("waypoint_mission_init_callback_pc,ack:%x\n", pcd_s.data[0]);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//wappoint data upload callback by pc
void waypoint_data_upload_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d, waypointdatalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin, sizeof(WayPointSettings));
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.missionACK;
		pcd_s.data[0] = ack;
	}
	if(recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 2)
	{
		pcd_s.data[1] = recvFrame.recvData.raw_ack_array[1];
	}
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin == 2+sizeof(WayPointSettings))
	{
		memcpy(pcd_s.data + 2, recvFrame.recvData.raw_ack_array+2, sizeof(WayPointSettings));
	}
//	printf("waypoint_data_upload_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_SINGLE_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//waypoint mission start callback by pc
void waypoint_start_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.missionACK;
		pcd_s.data[0] = ack;
	}
//	printf("waypoint_start_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_START_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//waypoint mission stop callback by pc
void waypoint_stop_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.missionACK;
		pcd_s.data[0] = ack;
	}
//	printf("waypoint_stop_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_START_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//wappoint mission pause callback by pc
void waypoint_pause_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.missionACK;
		pcd_s.data[0] = ack;
	}
//	printf("waypoint_pause_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_PAUSE_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//wappoint mission resume callback by pc
void waypoint_resume_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin); 
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.missionACK;
		pcd_s.data[0] = ack;
	}
//	printf("waypoint_resume_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_PAUSE_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//NRD gohome callback
void nrd_gohome_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack = OpenProtocolCMD::ErrorCode::CommonACK::NO_RESPONSE_ERROR;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	PcContrlDatas pcd_s;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >= 1)
	{
		ack = recvFrame.recvData.commandACK;
		pcd_s.data[0] = (uint8_t)ack;
	}
//	printf("nrd_gohome_callback_pc,ack:%x\n", ack); 
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_SET_GOHOME;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);	
}
//get Waypoint Settings callback by pc
void getwaypointsettings_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack;
	PcContrlDatas pcd_s;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin);
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >=1+sizeof(WayPointInitSettings))
	{
		ack = recvFrame.recvData.wpInitACK.ack;
		pcd_s.data[0] = ack;
		memcpy((void*)(pcd_s.data + 1), (void*)&recvFrame.recvData.wpInitACK.data, sizeof(WayPointInitSettings));
//		printf("getwaypointsettings_callback_pc,ack:%x\n", ack);
	}
	else
	{
		DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
		return;
	}
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_READ_WAYPOINT_INIT_STATUS;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//get Waypoint Settings callback by pc
void getwaypointindex_callback_pc(DJI::OSDK::Vehicle *vehicle, DJI::OSDK::RecvContainer recvFrame, DJI::OSDK::UserData userData)
{
	uint8_t ack;
	PcContrlDatas pcd_s;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	
//	printf("recvFrame.recvInfo.len:%d, datalen:%d\n", recvFrame.recvInfo.len,recvFrame.recvInfo.len - OpenProtocol::PackageMin); 
	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin >=1+sizeof(WayPointSettings))
	{
		//从getIndexCallback接口能得知，接收数据没有多一个index字节，与我们自己的协议违背，这里自己补充
		ack = recvFrame.recvData.wpIndexACK.ack;
		pcd_s.data[0] = ack;
		pcd_s.data[1] = recvFrame.recvData.wpIndexACK.data.index;
		memcpy((void*)(pcd_s.data + 2), (void*)&recvFrame.recvData.wpIndexACK.data, sizeof(WayPointSettings));
//		printf("getwaypointindex_callback_pc,ack:%x\n", ack); 
	}
	else
	{
		DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
		return;
	}
	pcd_s.start_code = START_CODE;
	pcd_s.cmd = CMD_READ_SINGLE_WAYPOINT;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);
}
//wapoint get or set idle velecity callback pc
void wapointidlevel_callback_pc(Vehicle* vehicle, RecvContainer recvFrame,UserData userData)
{
	uint8_t ack;
	PcContrlDatas pcd_s;
	uint8_t *send_pc_buf = (uint8_t*)nrd_cb_handle_st.buf;
	uint16_t cmdlen;
	pcd_s.data = &send_pc_buf[FRAME_HEAD_LEN];
	NrdCbUserData* udata = (NrdCbUserData*)userData;

	if (recvFrame.recvInfo.len - OpenProtocol::PackageMin <=sizeof(ACK::WayPointVelocityInternal))
	{
		ack = recvFrame.recvData.wpVelocityACK.ack;
		pcd_s.data[0] = ack;
		memcpy((void*)(pcd_s.data + 1), (void*)&recvFrame.recvData.wpVelocityACK.idleVelocity, 4);
//		printf("wapointidlevel_callback_pc,ack:%x\n", ack); 	
	}
	else
	{
		DERROR("ACK is exception, sequence %d\n", recvFrame.recvInfo.seqNumber);
		return;
	}
	pcd_s.start_code = START_CODE;
	if((udata->cmd==CMD_SET_WAYPOINT_IDLE_VEL)||(udata->cmd==CMD_GET_WYAPOINT_IDLE_VEL))
		pcd_s.cmd = udata->cmd;
	else
		pcd_s.cmd = CMD_GET_WYAPOINT_IDLE_VEL;
	pcd_s.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pcd_s.cmd);
	memcpy(send_pc_buf, &pcd_s.start_code, FRAME_HEAD_LEN);
	pcd_s.check_code = crc_calculate(send_pc_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(send_pc_buf + FRAME_HEAD_LEN + cmdlen, &pcd_s.check_code, FRAME_TAIL_LEN);
	nrd_cb_common_handle(cmdlen + FIXED_FRAME_LEN);	
}
