#ifndef zkrt_TYPES_H_
#define zkrt_TYPES_H_

#include "sys.h"

////////////////////////////////////////////////////////////macro switch
#define ZK_CRC_ENABLE               1     //CRC校验开关

//start code
#define _START_CODE                 0XEB
//version
#define _VERSION                    0x01   
//end code
#define _END_CODE                   0XBE

//session ack
#define ZK_SESSION_ACK_ENABLE       1
#define ZK_SESSION_ACK_DISABLE      0

//padding encrypt
#define  ZK_ENCRYPT_EN              1
#define  ZK_ENCRYPT_DS              0

//cmd 指示方向和用户
#define APP_TO_UAV                  0X00
#define UAV_TO_APP                  0X01
#define UAV_TO_SUBDEV               0X02  //管理板到子设备
#define SUBDEV_TO_UAV               0X03
#define APP_TO_UAV_SUPER            0X20
#define UAV_TO_APP_SUPER            0X21

//length 此帧字节定义的是Data的数据长度
//seq 序列号，自增
//APPID, 保留，值0
//#define _LENGTH     0X1E
//#define _TOTAL_LEN  0X32

//UAVID，设备类型定义，用于UAVID[3]
#define DEVICE_TYPE_UDP             0X00
#define DEVICE_TYPE_TEMPERATURE     0X01
#define DEVICE_TYPE_OBSTACLE        0X02
#define DEVICE_TYPE_CONTROL         0X03
#define DEVICE_TYPE_CAMERA          0X04
#define DEVICE_TYPE_GAS             0X05
#define DEVICE_TYPE_THROW           0X06
#define DEVICE_TYPE_STANDBY         0X07
#define DEVICE_TYPE_PARACHUTE       0X08
#define DEVICE_TYPE_IRRADIATE       0X09
#define DEVICE_TYPE_MEGAPHONE       0X0A
#define DEVICE_TYPE_BATTERY         0X0B
#define DEVICE_TYPE_3DMODELING      0X0C
#define DEVICE_TYPE_MULTICAMERA     0X0D
#define DEVICE_NUMBER               DEVICE_TYPE_MULTICAMERA   //zkrt_notice: 每次新增设备需更新此值
//#define DEVICE_TYPE_SELF            DEVICE_TYPE_MEGAPHONE     //自身设备码

#define DEVICE_TYPE_LOCAL_START     100
#define DEVICE_TYPE_MAINBOARD       100
#define DEVICE_TYPE_HEART				    101
#define DEVICE_TYPE_HEARTV2			    102
#define DEVICE_TYPE_LOCAL_END       DEVICE_TYPE_HEARTV2

//command
#define ZK_COMMAND_NORMAL           0
#define ZK_COMMAND_SPECIFIED        1
#define ZK_COMMAND_COMMON           2
#define ZK_COMMAND_MAX              ZK_COMMAND_COMMON

//命令index
#define ZKRT_INDEX_START            0
#define ZKRT_INDEX_VER              1
#define ZKRT_INDEX_SESSION_ACK      2
#define ZKRT_INDEX_PADDING_ENC      3
#define ZKRT_INDEX_CMD              4
#define ZKRT_INDEX_LENGTH           5
#define ZKRT_INDEX_SEQ              6
#define ZKRT_INDEX_APPID            7
#define ZKRT_INDEX_UAVID            10
#define ZK_INDEX_UAVDEVNUM          12
#define ZKRT_INDEX_COMMAND          16
#define ZKRT_INDEX_DATA             17
#define ZKRT_INDEX_CRC(datalen)	    (ZK_HEADER_LEN+datalen-1)
#define ZKRT_INDEX_END(datalen)     (ZK_HEADER_LEN+datalen+1)
//data index
#define ZK_DINDEX_DEVTYPE           3

//编号定义，用于UAVID[2：1]和UAVID[5：4]
#define DEFAULT_NUM 0X00
#define SECOND_NUM  0X01

//frame length
#define ZK_HEADER_LEN               17    //[start code, Data)
#define ZK_FIXED_LEN                20
#define ZK_EXCEP_LEN                (ZK_FIXED_LEN-ZK_HEADER_LEN)
#define ZK_MAX_LEN                  100
#define ZK_DATA_MAX_LEN             (ZK_MAX_LEN-ZK_FIXED_LEN)

//packet max num
#pragma pack(push, 1)
typedef struct
{
	uint8_t start_code;
	uint8_t ver;
	uint8_t session_ack;
	uint8_t padding_enc;
	uint8_t cmd;
	uint8_t length;    //length -- [data length]
	uint8_t seq;
	uint8_t APPID[3];
	uint8_t UAVID[6];
	uint8_t command;
} zkrtpacket_header;
typedef struct
{
	uint16_t crc;      //[Start Code~Data[n]]
	uint8_t end_code;
} zkrtpacket_tailer;
typedef struct _zkrt_packet_t
{
	uint8_t start_code; 		        //字节0，帧起始码，0XEB
	uint8_t ver;		   						  //字节1，协议版本
	uint8_t session_ack;	          //字节2，会话ID，0无需应答，1有应答；帧标识：0数据帧，1命令帧
	uint8_t padding_enc;            //字节3，加密帧时数据的长度；是否加密帧，0不加密，1加密
	uint8_t cmd;             		  	//字节4，命令码，1管理单元给服务器，0服务器给管理单元
	uint8_t length;           			//字节5，数据长度
	uint8_t seq;               			//字节6，帧序列号
	uint8_t APPID[3];      			    //字节7-9，服务器ID，地面站
	uint8_t UAVID[6];        			  //字节10-15，客户端ID：哪种飞机、哪个编号、哪个设备、哪个编号
	uint8_t command;        			  //字节16
	uint8_t data[ZK_DATA_MAX_LEN];  //字节
	uint16_t crc;              			//字节17+len，CRC校验码
	uint8_t end_code;          			//字节19+len，帧结束
} zkrt_packet_t;

#pragma pack(pop)

//get zkrt packet in can buffer relevant handle struct
typedef struct
{
	u8 recv_ok;       //1-receive complete, 0-receive not complete
	u8 curser_state;  //handle index state
	u8 app_index;
	u8 uav_index;
	u8 dat_index;
	zkrt_packet_t packet;  //receive packet
} recv_zkrt_packet_handlest;

void crc_accumulate(uint8_t data, uint16_t *crcAccum);
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length);
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length);
uint8_t zkrt_final_encode(uint8_t *dstdata, zkrt_packet_t *packet);
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch);
uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch);
uint8_t zkrt_decode_char_v2(recv_zkrt_packet_handlest *rh, uint8_t ch);
void zkrt_init_packet(zkrt_packet_t *packet);
bool zkrt_check_packet(const zkrt_packet_t *packet);
uint8_t zkrt_packet2_data(uint8_t *dstdata, const zkrt_packet_t *packet);
void zkrt_data2_packet(const uint8_t *sdata, uint8_t sdatalen, zkrt_packet_t *packet);
#endif

