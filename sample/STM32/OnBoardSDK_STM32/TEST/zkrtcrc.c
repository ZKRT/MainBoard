#include <stdio.h>
#include <string.h>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

void crc_accumulate(unsigned char data, uint16_t *crcAccum)
{
	/*Accumulate one byte of data into the CRC*/
	uint8_t tmp;

	tmp = data ^ (uint8_t)(*crcAccum &0xff);
	tmp ^= (tmp<<4);
	*crcAccum = (*crcAccum>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);
}
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length)
{
	uint16_t crcTmp = 0XFFFF;//D¡ê?¨¦??¨¦¨¨???a0xffff
        
	while (length--) 
	{
		crc_accumulate(*pBuffer++, &crcTmp);//?e????¨ºy¡Á¨¦?D¦Ì?????¡Á??¨²??DDD¡ê?¨¦
	}
	return crcTmp;//¡¤¦Ì??D¡ê?¨¦??
}

//frame length
#define ZK_HEADER_LEN               17    //[start code, Data)
#define ZK_FIXED_LEN                20
#define ZK_EXCEP_LEN                (ZK_FIXED_LEN-ZK_HEADER_LEN)
#define ZK_MAX_LEN                  100
#define ZK_DATA_MAX_LEN             (ZK_MAX_LEN-ZK_FIXED_LEN)
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

#pragma pack(push, 1)
typedef struct _zkrt_packet_t
{
	uint8_t start_code; 		       
	uint8_t ver;		   						  
	uint8_t session_ack;	         
	uint8_t padding_enc;            
	uint8_t cmd;             		  	
	uint8_t length;           		
	uint8_t seq;               			
	uint8_t APPID[3];      			  
	uint8_t UAVID[6];        			  
	uint8_t command;        			  
	uint8_t data[ZK_DATA_MAX_LEN];  
	uint16_t crc;              		
	uint8_t end_code;          		
}zkrt_packet_t;
#pragma pack(pop)

zkrt_packet_t mypacket;

int main(void) {
	int i;
	// your code goes here
	unsigned char sdata[ZK_DATA_MAX_LEN];	
	zkrt_packet_t *packet = &mypacket;
	packet->start_code = 0xeb;                            
	packet->ver = 01;                                      
	packet->session_ack = 0;                                 
	packet->padding_enc = 0;    
    packet->end_code = 0xbe;		                                  
	packet->seq = 0;  
	packet->cmd = 0;
	packet->command = 0;
	packet->crc = 0XFFFF;
	for(i=0;i<3;i++)
		packet->APPID[i]= 0x00;        
	for(i=0;i<6;i++)
		packet->UAVID[i]= 0x00;	
	for(i=0;i<ZK_DATA_MAX_LEN;i++)
		packet->data[i]= 0x00;	

//////////////////		
	packet->length = 30; 
	packet->UAVID[3]= DEVICE_TYPE_THROW;	
	packet->data[0] = 1;
	packet->data[1] = 0;
	packet->data[2] = 0;
	packet->crc = crc_calculate(((const unsigned char*)(packet)), ZK_HEADER_LEN+packet->length);
	memcpy(sdata, packet, ZK_HEADER_LEN);
	memcpy(sdata+ZK_HEADER_LEN, packet->data, packet->length);
	memcpy(sdata+ZK_HEADER_LEN+packet->length, packet->data+ZK_DATA_MAX_LEN, ZK_FIXED_LEN-ZK_HEADER_LEN);
	printf("start===================\n");
	for(i=0; i< ZK_FIXED_LEN+packet->length; i++)
		printf("%x ", sdata[i]);
	printf("end===================\n");
	return 0;
}
