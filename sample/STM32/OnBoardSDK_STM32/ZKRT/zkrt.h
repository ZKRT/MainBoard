#ifndef zkrt_TYPES_H_
#define zkrt_TYPES_H_

#include "sys.h"

extern uint8_t now_uav_type;
extern uint16_t now_uav_num;


//���Ƿ���ͳ����û�������cmd
#define APP_TO_UAV         0X00
#define UAV_TO_APP         0X01
#define APP_TO_UAV_SUPER   0X20
#define UAV_TO_APP_SUPER   0X21


//���Ǹ���ģ��Ĺ��ܹ��࣬����command
#define FUNC_SET_ID        0X00               //����ID
#define FUNC_HEART         0X01               //�鿴����
#define FUNC_POWER         0X02               //�̵�������
#define FUNC_ACK           0X03               //��Ӧ
#define FUNC_READ          0X04               //��ȡֵ
#define FUNC_SET_TEMPTURE  0X05               //�����¶�������
#define FUNC_THROW         0X06               //������Ͷģ��߷�ֵ
#define FUNC_THROWS        0X07               //���ö����Ͷ
#define FUNC_ANGEL         0X08               //������̨�Ƕ�
#define FUNC_FOCAL_LENGTH  0X09               //���ý���Զ��


//����仯�ġ����Ժ궨��ı���
#define _START_CODE 0XEB
#define _VERSION    0X01
#define _LENGTH     0X1E
#define _END_CODE   0XBE
#define _TOTAL_LEN  0X32   //����mavlink


//�ɻ����Ͷ��壬����UAVID[0]
#define UAV_TYPE_4 0X04
#define UAV_TYPE_6 0X06
#define UAV_TYPE_8 0X08


//�豸���Ͷ��壬����UAVID[3]
//�豸�������ڸ�CAN������ô�������Ļ���
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


//��Ŷ��壬����UAVID[2��1]��UAVID[5��4]
#define DEFAULT_NUM 0X00
#define SECOND_NUM  0X01


//�����ֽڵ�λ�ã�������
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

//��20+len=50������һ��50�����ݵ�Ҫ����ôlen=30��Ҳ����data[30]
//�ɱ�ģ�cmd��UAVID��command��data������UAVIDֻ��������ID��ʱ��Ż������仯
typedef struct _zkrt_packet_t
{
	uint8_t start_code; 		        //�ֽ�0��֡��ʼ�룬0XEB
	uint8_t ver;		   						  //�ֽ�1��Э��汾
	uint8_t session_ack;	          //�ֽ�2���ỰID��0����Ӧ��1��Ӧ��֡��ʶ��0����֡��1����֡
	uint8_t padding_enc;            //�ֽ�3������֡ʱ���ݵĳ��ȣ��Ƿ����֡��0�����ܣ�1����
	uint8_t cmd;             		  	//�ֽ�4�������룬1����Ԫ����������0������������Ԫ
	uint8_t length;           			//�ֽ�5�����ݳ���
	uint8_t seq;               			//�ֽ�6��֡���к�
	uint8_t APPID[3];      			    //�ֽ�7-9��������ID������վ
	uint8_t UAVID[6];        			  //�ֽ�10-15���ͻ���ID�����ַɻ����ĸ���š��ĸ��豸���ĸ����
	uint8_t command;        			  //�ֽ�16�������룬�����¶���صģ�0x01
	uint8_t data[30];          			//�ֽ�17-��17+len-1����Ҳ�����ֽ�17-46
	uint16_t crc;              			//�ֽ�17+len��CRCУ���룬Ҳ�����ֽ�47-48
	uint8_t end_code;          			//�ֽ�19+len��֡������Ҳ�����ֽ�49
}zkrt_packet_t;

#pragma pack(pop)


void zkrt_final_encode(zkrt_packet_t *packet);
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch);
uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch);



#endif

