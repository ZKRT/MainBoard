#ifndef zkrt_TYPES_H_
#define zkrt_TYPES_H_

#include "sys.h"

extern uint8_t now_uav_type;
extern uint16_t now_uav_num;


//这是方向和超级用户，用于cmd
#define APP_TO_UAV         0X00
#define UAV_TO_APP         0X01
#define APP_TO_UAV_SUPER   0X20
#define UAV_TO_APP_SUPER   0X21


//这是各个模块的功能归类，属于command
#define FUNC_SET_ID        0X00               //设置ID
#define FUNC_HEART         0X01               //查看心跳
#define FUNC_POWER         0X02               //继电器开关
#define FUNC_ACK           0X03               //响应
#define FUNC_READ          0X04               //读取值
#define FUNC_SET_TEMPTURE  0X05               //设置温度上下限
#define FUNC_THROW         0X06               //设置抛投模块高峰值
#define FUNC_THROWS        0X07               //设置多点抛投
#define FUNC_ANGEL         0X08               //设置云台角度
#define FUNC_FOCAL_LENGTH  0X09               //设置焦距远近


//不会变化的、可以宏定义的变量
#define _START_CODE 0XEB
#define _VERSION    0X01
#define _LENGTH     0X1E
#define _END_CODE   0XBE
#define _TOTAL_LEN  0X32   //用于mavlink


//飞机类型定义，用于UAVID[0]
#define UAV_TYPE_4 0X04
#define UAV_TYPE_6 0X06
#define UAV_TYPE_8 0X08


//设备类型定义，用于UAVID[3]
//设备数量用于给CAN创建这么多数量的缓存
#define DEVICE_NUMBER           0X0B
#define DEVICE_TYPE_UDP         0X00
#define DEVICE_TYPE_TEMPERATURE 0X01
#define DEVICE_TYPE_OBSTACLE    0X02
#define DEVICE_TYPE_CONTROL     0X03
#define DEVICE_TYPE_CAMERA      0X04
#define DEVICE_TYPE_GAS         0X05
#define DEVICE_TYPE_THROW       0X06
#define DEVICE_TYPE_STANDBY     0X07
#define DEVICE_TYPE_PARACHUTE   0X08
#define DEVICE_TYPE_IRRADIATE   0X09
#define DEVICE_TYPE_MEGAPHONE   0X0A
#define DEVICE_TYPE_BATTERY     0X0B
#define DEVICE_TYPE_BAOSHAN			0X0C
#define DEVICE_TYPE_HEART				101


//编号定义，用于UAVID[2：1]和UAVID[5：4]
#define DEFAULT_NUM 0X00
#define SECOND_NUM  0X01


//各个字节的位置，不常用
#define ZKRT_INDEX_START       0
#define ZKRT_INDEX_VER         1
#define ZKRT_INDEX_SESSION_ACK 2
#define ZKRT_INDEX_PADDING_ENC 3
#define ZKRT_INDEX_CMD         4
#define ZKRT_INDEX_LENGTH      5
#define ZKRT_INDEX_SEQ         6
#define ZKRT_INDEX_APPID       7
#define ZKRT_INDEX_UAVID      10
#define ZKRT_INDEX_COMMAND    16
#define ZKRT_INDEX_DATA       17
#define ZKRT_INDEX_CRC	      47
#define ZKRT_INDEX_END        49


#pragma pack(push, 1)

//让20+len=50以满足一共50个数据的要求，那么len=30，也就是data[30]
//可变的：cmd、UAVID、command、data，其中UAVID只有在设置ID的时候才会有所变化
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
	uint8_t command;        			  //字节16，功能码，比如温度相关的，0x01
	uint8_t data[30];          			//字节17-（17+len-1），也就是字节17-46
	uint16_t crc;              			//字节17+len，CRC校验码，也就是字节47-48
	uint8_t end_code;          			//字节19+len，帧结束，也就是字节49
}zkrt_packet_t;

#pragma pack(pop)


void zkrt_final_encode(zkrt_packet_t *packet);
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch);
uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch);



#endif

