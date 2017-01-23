#include "zkrt.h"
#include "mavlink.h"			

uint8_t zkrt_tx_seq[DEVICE_NUMBER];               
uint8_t now_uav_type = 0;
uint16_t now_uav_num = 0;


void zkrt_final_encode(zkrt_packet_t *packet)
{
	uint8_t i;
	
	packet->start_code = _START_CODE;                             //�ֽ�0��֡��ʼ�룬0XEB
	packet->ver = _VERSION;                                       //�ֽ�1��Э��汾
	packet->session_ack = 0X00;                                   //�ֽ�2������Ӧ������֡
	packet->padding_enc = 0X00;                                   //�ֽ�3��������
	packet->length = _LENGTH;                                     //�ֽ�5��һ��30λ
	packet->seq = zkrt_tx_seq[packet->UAVID[3]];                  //�ֽ�6��֡���к�
	zkrt_tx_seq[packet->UAVID[3]]++;
	zkrt_tx_seq[packet->UAVID[3]]%=255;
	for(i=0;i<3;i++)
	packet->APPID[i]= 0x00;                                       //�ֽ�7-9��������ID������վ
	packet->crc = crc_calculate(((const uint8_t*)(packet)),47);     //��47-48��У��λ
	packet->end_code = _END_CODE;                                 //��49��������
}


/*�Ե����ֽڵ�crc����У��*/
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch)
{
	uint16_t crc = packet->crc;
	
	crc_accumulate(ch,&(crc));
	
	packet->crc = crc;
}



uint8_t zkrt_curser_state = 0;
uint8_t zkrt_recv_success = 0;
uint8_t zkrt_app_index = 0;
uint8_t zkrt_uav_index = 0;
uint8_t zkrt_dat_index = 0;
uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch)
{
	zkrt_recv_success = 0;
	
	if ((zkrt_curser_state == 0)&&(ch == _START_CODE))		//�ֽ�0���õ���ʼ��
	{
//	 printf("zkrt_decode_char_START_CODE\r\n");
		packet->start_code = ch;
		packet->crc = 0XFFFF;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 1;
	}
	else if ((zkrt_curser_state == 1)&&(ch == _VERSION))	//�ֽ�1���õ��汾��
	{
//	printf("zkrt_decode_char_VERSION\r\n");
		packet->ver = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 2;
	}
	else if (zkrt_curser_state == 2)											//�ֽ�2���õ�session
	{
//	printf("zkrt_decode_char_2\r\n");
		packet->session_ack = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 3;
	}
	else if (zkrt_curser_state == 3)											//�ֽ�3���õ�padding
	{
	//printf("zkrt_decode_char_3\r\n");
		packet->padding_enc = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 4;
	}
	else if (zkrt_curser_state == 4)											//�ֽ�4���õ�cmd
	{
//	printf("zkrt_decode_char_4\r\n");
		packet->cmd = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 5;
	}
	else if ((zkrt_curser_state == 5)&&(ch == _LENGTH))		//�ֽ�5���õ�����
	{
//	printf("zkrt_decode_char_5\r\n");
		packet->length = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 6;
	}
	else if (zkrt_curser_state == 6)											//�ֽ�6���������к�
	{
//	printf("zkrt_decode_char_6\r\n");
		packet->seq = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 7;
	}
	else if (zkrt_curser_state == 7)											//�ֽ�7-9������APPID
	{
//	printf("zkrt_decode_char_8\r\n");
		packet->APPID[zkrt_app_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_app_index++;
		if (zkrt_app_index == 3)
		{
			zkrt_app_index = 0;
			zkrt_curser_state = 8;
		}
	}
	else if (zkrt_curser_state == 8)											//�ֽ�10-15������UAVID
	{
//	printf("zkrt_decode_char_9\r\n");
		packet->UAVID[zkrt_uav_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_uav_index++;
		if (zkrt_uav_index == 6)
		{
			zkrt_uav_index = 0;
			zkrt_curser_state = 9;
		}
	}
	else if (zkrt_curser_state == 9)											//�ֽ�16���õ�command
	{
//	printf("zkrt_decode_char_9\r\n");
		packet->command = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_curser_state = 10;
	}
	else if (zkrt_curser_state == 10)											//�ֽ�17-46������DATA
	{
//	printf("zkrt_decode_char_data=0x %x\r\n",ch);
		packet->data[zkrt_dat_index] = ch;
		zkrt_update_checksum(packet,ch);
		zkrt_dat_index++;
		if (zkrt_dat_index == 30)
		{
			zkrt_dat_index = 0;
			zkrt_curser_state = 11;
		}
	}
	else if ((zkrt_curser_state == 11)&&(ch == (uint8_t)((packet->crc)&0xff)))	//�ֽ�47��CRC1
	{
		zkrt_curser_state = 12;
	}
	else if ((zkrt_curser_state == 12)&&(ch == (uint8_t)((packet->crc)>>8)))		//�ֽ�48��CRC2
	{
		zkrt_curser_state = 13;
	}
	else if ((zkrt_curser_state == 13)&&(ch == _END_CODE))											//�ֽ�49����β
	{
		packet->end_code = ch;
		zkrt_curser_state = 0;
		zkrt_recv_success = 1;
	}
	else
	{
		zkrt_curser_state = 0;
		zkrt_app_index = 0;
		zkrt_uav_index = 0;
		zkrt_dat_index = 0;
	}
	
	return zkrt_recv_success;
}	



