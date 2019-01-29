////////////////////////////////////////////////////////include
#include "hw_config.h"
#include "dji_vehicle.hpp"
#include "NuclearRadiationDetection.h"

#ifdef CPLUSPLUS_HANDLE_ZK
extern "C"
{
#endif
#include "osusart.h"
#include "osqtmr.h"
#include "zkrt.h"
//#include "NuclearRadiationDetection.h"

#ifdef CPLUSPLUS_HANDLE_ZK
}
#endif
////////////////////////////////////////////////////////variables
using namespace DJI::OSDK;
extern Vehicle vehicle;
extern Vehicle *v;

nrd_st nuclear_r_d_s;

#pragma pack(1)
typedef struct
{
	Telemetry::Quaternion q;
	Telemetry::GlobalPosition g;
	Telemetry::Battery b;
} GpsData;
#pragma pack()
////////////////////////////////////////////////////////static fun
static void nrd_recv_handle(void);
static void nrd_period_send(void);
static uint8_t cmd_len_of_pc_control_data(uint8_t cmd);
static uint8_t cmd_len_of_respond_pc_control_data(uint8_t cmd);
//void pc_control_callback(DJI::OSDK::Vehicle *vehicle,
//						 DJI::OSDK::RecvContainer recvFrame,
//						 DJI::OSDK::UserData userData);
//
void nuclear_radiation_detect_init(void)
{
	nuclear_r_d_s.peroid_upload_cnt = NRD_P_U_CNT * 3; //init value, *3
	nuclear_r_d_s.peroid_upload_flag = 0;
}
//
void nuclear_radiation_detect_prcs(void)
{
	nrd_recv_handle();
	nrd_period_send();
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
	memset(buf, 0, sizeof(buf_nrd));
	if (t_osscomm_ReceiveMessage(buf, &buflen, USART2) != SCOMM_RET_OK)
	{
		return;
	}
	pc_control_datas.start_code = buf[0];
	pc_control_datas.cmd = buf[1];
	pc_control_datas.data = &buf[2];
	if (pc_control_datas.start_code != START_CODE)
	{ //check startcode
		return;
	}
	cmdlen = cmd_len_of_pc_control_data(pc_control_datas.cmd);
	if (cmdlen + FIXED_FRAME_LEN != buflen)
	{ //check len
		return;
	}
	memcpy(&pc_control_datas.check_code, buf + cmdlen + FRAME_HEAD_LEN, 2);
	memcpy(&pc_control_datas.end_code, buf + cmdlen + FRAME_HEAD_LEN + 2, 2);
	if (pc_control_datas.end_code != END_CODE)
	{ //check endcode
		return;
	}
	if (crc_calculate(buf, cmdlen + FRAME_HEAD_LEN) != pc_control_datas.check_code)
	{ //check crc
		return;
	}
	switch (pc_control_datas.cmd)
	{
	case CMD_SET_GOHOME:
		// nuclear_r_d_s.pc_cmd = pc_control_datas.cmd;
		// v->control->goHome(pc_control_callback, &nuclear_r_d_s.pc_cmd);
		{
			ACK::ErrorCode ack = v->control->goHome(NUCLEAR_MISSION_TIME);
			if (ack_state)
				ACK::getErrorCodeMessage(ack, __func__); //zkrt_debug
			pc_control_datas.data[0] = ack_state;
			need_transmit = 1;
		}
		break;
	case CMD_WAYPOINT_INIT:
	{
		WayPointInitSettings *fdata = (WayPointInitSettings *)pc_control_datas.data;
		ACK::ErrorCode ack = v->missionManager->init(WAYPOINT, NUCLEAR_MISSION_TIME, &fdata);
		ack_state = ACK::getError(ack);
		if (ack_state)
			ACK::getErrorCodeMessage(ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		need_transmit = 1;
	}
	break;
	case CMD_SINGLE_WAYPOINT:
	{
		WayPointSettings *wp = (WayPointSettings *)pc_control_datas.data;
		ACK::WayPointIndex wpack = v->missionManager->wpMission->uploadIndexData(wp, NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(wpack.ack);
		if (ack_state)
			ACK::getErrorCodeMessage(wpack.ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		pc_control_datas.data[1] = wpack.data.index;
		memcpy(pc_control_datas.data + 2, (void *)&wpack.data, sizeof(WayPointSettings));
		need_transmit = 1;
	}
	break;
	case CMD_START_WAYPOINT:
	{
		ACK::ErrorCode ack;
		if (pc_control_datas.data[0] == 0)
			ack = v->missionManager->wpMission->start(NUCLEAR_MISSION_TIME);
		else
			ack = v->missionManager->wpMission->stop(NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(ack);
		if (ack_state)
			ACK::getErrorCodeMessage(ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		need_transmit = 1;
	}
	break;
	case CMD_PAUSE_WAYPOINT:
	{
		ACK::ErrorCode ack;
		if (pc_control_datas.data[0] == 0)
			ack = v->missionManager->wpMission->pause(NUCLEAR_MISSION_TIME);
		else
			ack = v->missionManager->wpMission->resume(NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(ack);
		if (ack_state)
			ACK::getErrorCodeMessage(ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		need_transmit = 1;
	}
	break;
	case CMD_READ_WAYPOINT_INIT_STATUS:
	{
		ACK::WayPointInit wpinitack = v->missionManager->wpMission->getWaypointSettings(NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(wpinitack.ack);
		if (ack_state)
			ACK::getErrorCodeMessage(wpinitack.ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		memcpy(pc_control_datas.data + 1, (void *)&wpinitack.data, sizeof(WayPointInitSettings));
		need_transmit = 1;
	}
	break;
	case CMD_READ_SINGLE_WAYPOINT:
	{
		uint8_t index = pc_control_datas.data[0];
		ACK::WayPointIndex wpack = v->missionManager->wpMission->getIndex(index, NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(wpack.ack);
		if (ack_state)
			ACK::getErrorCodeMessage(wpack.ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		pc_control_datas.data[1] = wpack.data.index;
		;
		memcpy(pc_control_datas.data + 2, (void *)&wpack.data, sizeof(WayPointSettings));
		need_transmit = 1;
	}
	break;
	case CMD_SET_WAYPOINT_IDLE_VEL:
	{
		float32_t vel;
		memcpy(&vel, pc_control_datas.data, 4);
		ACK::WayPointVelocity wpv = v->missionManager->wpMission->updateIdleVelocity(vel, NUCLEAR_MISSION_TIME);
		ack_state = ACK::getError(wpv.ack);
		if (ack_state)
			ACK::getErrorCodeMessage(wpv.ack, __func__); //zkrt_debug
		pc_control_datas.data[0] = ack_state;
		memcpy(pc_control_datas.data + 1, (void *)&wpv.idleVelocity, 4);
		need_transmit = 1;
	}

	break;
	case CMD_GET_WYAPOINT_IDLE_VEL:
		//	{//SDK没有返回vel，所以暂时不做 //zkrt_notice
		//		ACK::ErrorCode ack = v->missionManager->wpMission->readIdleVelocity(NUCLEAR_MISSION_TIME);
		//		ack_state = ACK::getError(ack);
		//		if (ack_state)
		//			ACK::getErrorCodeMessage(ack, __func__); //zkrt_debug
		//		pc_control_datas.data[0] = ack_state;
		//		memcpy(pc_control_datas.data + 1, (void *)&wpv.data, 4);
		//		need_transmit = 1;
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
		memcpy(respond_buf + FRAME_HEAD_LEN, &pc_control_datas.data, cmdlen);
		//get crc
		pc_control_datas.check_code = crc_calculate(respond_buf, FRAME_HEAD_LEN + cmdlen);
		memcpy(respond_buf + FRAME_HEAD_LEN + cmdlen, &pc_control_datas.check_code, FRAME_TAIL_LEN);
		t_osscomm_sendMessage(respond_buf, cmdlen + FIXED_FRAME_LEN, USART2);
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

	pc_control_datas.start_code = START_CODE;
	pc_control_datas.cmd = CMD_UPLOAD_GPS_DATA;
	pc_control_datas.data = &hb_buf[FRAME_HEAD_LEN];
	pc_control_datas.end_code = END_CODE;
	cmdlen = cmd_len_of_respond_pc_control_data(pc_control_datas.cmd);
	pc_control_datas.data[0] = 0; //ack
	GpsData *gpsdata = (GpsData *)&pc_control_datas.data[1];
	gpsdata->q = v->broadcast->getQuaternion();
	gpsdata->g = v->broadcast->getGlobalPosition();
	gpsdata->b = v->broadcast->getBatteryInfo();
	memcpy(hb_buf, &pc_control_datas.start_code, FRAME_HEAD_LEN);
	// memcpy(hb_buf + FRAME_HEAD_LEN, &pc_control_datas.data, cmdlen); //not need because &data[0] pointer is &hb_buf[FRAME_HEAD_LEN]
	pc_control_datas.check_code = crc_calculate(hb_buf, FRAME_HEAD_LEN + cmdlen);
	memcpy(hb_buf + FRAME_HEAD_LEN + cmdlen, &pc_control_datas.check_code, FRAME_TAIL_LEN);
	t_osscomm_sendMessage(hb_buf, cmdlen + FIXED_FRAME_LEN, USART2);

	//reset timer flag
	nuclear_r_d_s.peroid_upload_flag = 0;
	nuclear_r_d_s.peroid_upload_cnt = NRD_P_U_CNT;
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
