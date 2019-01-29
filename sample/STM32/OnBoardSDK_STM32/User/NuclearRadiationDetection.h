#ifndef NUCLEARRADIATIONDETECTION_H
#define NUCLEARRADIATIONDETECTION_H
#include "sys.h"

typedef struct
{
#define NRD_P_U_CNT 1	//1s
	unsigned char peroid_upload_flag;
	unsigned char peroid_upload_cnt;
//msg handle
	unsigned char pc_cmd; //pc send cmd, this flag for action callback distingusih
}nrd_st;
//mission control timeout in dji sdk control
#define NUCLEAR_MISSION_TIME 1
#pragma pack(1)
/////////////////////////////////////////////////
//控制类型数据结构
typedef struct{
	uint8_t start_code;
	uint8_t cmd;
	uint8_t *data;
	uint16_t check_code;
	uint16_t end_code;
}PcContrlDatas;
//typedef struct
//{
//    Quaternion q;
//    GlobalPosition g;
//    Battery b;
//}GpsData;
/////////////////////////////////////////////////
#pragma pack()

//start code
#define START_CODE 0x5a
#define END_CODE 0x0a0d
//cmd code
#define CMD_HIGH_PRESURE                     0x01
#define CMD_NUCLEAR_UPLOAD_TIME              0x02
#define CMD_READ_BATTORY                     0x03
#define CMD_SET_GOHOME                       0x04
#define CMD_WAYPOINT_INIT                    0x05
#define CMD_SINGLE_WAYPOINT                  0x06
#define CMD_START_WAYPOINT                   0x07
#define CMD_PAUSE_WAYPOINT                   0x08
#define CMD_READ_WAYPOINT_INIT_STATUS        0x09
#define CMD_READ_SINGLE_WAYPOINT             0x0a
#define CMD_SET_WAYPOINT_IDLE_VEL            0x0b
#define CMD_GET_WYAPOINT_IDLE_VEL            0x0c
#define CMD_UPLOAD_GPS_DATA                  0xFE

//len
#define FIXED_FRAME_LEN 6
#define FRAME_HEAD_LEN  2
#define FRAME_TAIL_LEN 4

//ack
#define NRD_ACK_OK	0


void nuclear_radiation_detect_init(void);
void nuclear_radiation_detect_prcs(void);

extern nrd_st nuclear_r_d_s;
#endif // NUCLEARRADIATIONDETECTION_H
