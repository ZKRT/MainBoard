/**
  ******************************************************************************
  * @file    zkrt.c
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief   handle protocol with Mainboard
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

#include "zkrt.h"
//���к�ȫ�ֶ��壬Ȼ�����
uint8_t zkrt_tx_seq[DEVICE_NUMBER];               		//�ֽ�6��֡���кţ�Ϊ�����֣�ÿ��������Ҫ1��ר�ŵ����к�
/**
 * @brief Accumulate the X.25 CRC by adding one char at a time.
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new char to hash
 * @param crcAccum the already accumulated checksum
���õ����������crcУ��������ĳһʱ��
���������ã���������ĳ���ֽ�У��
����ǰ��crcУ������ĳ����ֵ
���ú�crcУ������һ���µ���ֵ
 **/
void crc_accumulate(uint8_t data, uint16_t *crcAccum) {
	/*Accumulate one byte of data into the CRC*/
	uint8_t tmp;

	tmp = data ^ (uint8_t)(*crcAccum & 0xff);
	tmp ^= (tmp << 4);
	*crcAccum = (*crcAccum >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
}
/**
 * @brief Accumulate the X.25 CRC by adding an array of bytes
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new bytes to hash
 * @param crcAccum the already accumulated checksum
���õ��������mavlink��5������У����֮��
���������ã���mavlink�е����ݲ��ֽ���crcУ��
����ǰ��crc�Ǹ��ݲ������ɵ�У����
���ú󣺵õ����ݲ���֮ǰ��У����
 **/
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length) {
	const uint8_t *p = (const uint8_t *)pBuffer;

	while (length--) {
		crc_accumulate(*p++, crcAccum);
	}
}
/**
 * @brief Calculates the X.25 checksum on a byte buffer
 *
 * @param  pBuffer buffer containing the byte array to hash
 * @param  length  length of the byte array
 * @return the checksum over the buffer bytes
���õ��������mavlinkָ�����crcУ�飨�տ�ʼ��
���������ã���mavlinkָ���buf[5��1]��5������crcУ��
����ǰ��crcУ������������
���ú󣺸��ݲ������ɵ�crcУ����
 **/
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length) {
	uint16_t crcTmp = 0XFFFF;//У��������Ϊ0xffff

	while (length--) {
		crc_accumulate(*pBuffer++, &crcTmp);//����������е�ÿ���ֽڽ���У��
	}
	return crcTmp;//����У����
}
/**
 * @brief
 * @param   �����͵Ļ�����
 * @param   ��ת���Ľṹ��
 * @return  �����͵����ݳ���
 **/
uint8_t zkrt_final_encode(uint8_t *dstdata, zkrt_packet_t *packet) {
//seq++
	packet->seq = zkrt_tx_seq[packet->UAVID[3]];
	zkrt_tx_seq[packet->UAVID[3]]++;
	zkrt_tx_seq[packet->UAVID[3]] %= 255;
//calculate crc
#if ZK_CRC_ENABLE
	packet->crc = crc_calculate(((const uint8_t*)(packet)), ZK_HEADER_LEN + packet->length);
#else
	packet->crc = 0xffff;
#endif
//copy packet to senddata
	memcpy(dstdata, packet, ZK_HEADER_LEN);
	memcpy(dstdata + ZK_HEADER_LEN, packet->data, packet->length);
	memcpy(dstdata + ZK_HEADER_LEN + packet->length, packet->data + ZK_DATA_MAX_LEN, ZK_FIXED_LEN - ZK_HEADER_LEN);
//return total length
	return (ZK_FIXED_LEN + packet->length);
}
/**
 * @brief zkrt_init_packet
 *
 * @param
 * @param
 * @return
 **/
void zkrt_init_packet(zkrt_packet_t *packet) {
	uint8_t i;
	packet->start_code = _START_CODE;
	packet->ver = _VERSION;
	packet->session_ack = ZK_SESSION_ACK_DISABLE;
	packet->padding_enc = ZK_ENCRYPT_DS;
	packet->end_code = _END_CODE;
//	packet->length = 0;
	packet->seq = 0;
//	packet->cmd = 0;
//	packet->command = 0;
	for (i = 0; i < 3; i++)
		packet->APPID[i] = 0x00;
	for (i = 0; i < 6; i++)
		packet->UAVID[i] = 0x00;
	for (i = 0; i < ZK_DATA_MAX_LEN; i++)
		packet->data[i] = 0x00;
	packet->crc = 0XFFFF;
}

/*�Ե����ֽڵ�crc����У��*/
void zkrt_update_checksum(zkrt_packet_t* packet, uint8_t ch) {
#if ZK_CRC_ENABLE
	uint16_t crc = packet->crc;

	crc_accumulate(ch, &(crc)); //���ֱ�Ӵ���crc_accumulate(ch,&(packet->crc));�����ԣ���Ϊ�ṹ���Ա�ĵ�ַ����ֱ�Ӵ��Σ���ᵼ��hardfault����

	packet->crc = crc;
#endif
}
///**
// * @brief
// * @param
// * @param
// **/
//uint8_t zkrt_curser_state = 0;
//uint8_t zkrt_recv_success = 0;
//uint8_t zkrt_app_index = 0;
//uint8_t zkrt_uav_index = 0;
//uint8_t zkrt_dat_index = 0;
//uint8_t zkrt_decode_char(zkrt_packet_t *packet, uint8_t ch)
//{
//	zkrt_recv_success = 0;
//
//	if ((zkrt_curser_state == 0)&&(ch == _START_CODE))		//�ֽ�0���õ���ʼ��
//	{
//		packet->start_code = ch;
//		packet->crc = 0XFFFF;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 1;
//	}
//	else if ((zkrt_curser_state == 1)&&(ch == _VERSION))	//�ֽ�1���õ��汾��
//	{
//		packet->ver = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 2;
//	}
//	else if (zkrt_curser_state == 2)											//�ֽ�2���õ�session
//	{
//		packet->session_ack = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 3;
//	}
//	else if (zkrt_curser_state == 3)											//�ֽ�3���õ�padding
//	{
//		packet->padding_enc = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 4;
//	}
//	else if (zkrt_curser_state == 4)											//�ֽ�4���õ�cmd
//	{
//		packet->cmd = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 5;
//	}
//	else if ((zkrt_curser_state == 5)&&(ch <= ZK_DATA_MAX_LEN))		//�ֽ�5���õ�����
//	{
//		packet->length = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 6;
//	}
//	else if (zkrt_curser_state == 6)											//�ֽ�6���������к�
//	{
//		packet->seq = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 7;
//	}
//	else if (zkrt_curser_state == 7)											//�ֽ�7-9������APPID
//	{
//		packet->APPID[zkrt_app_index] = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_app_index++;
//		if (zkrt_app_index == 3)
//		{
//			zkrt_app_index = 0;
//			zkrt_curser_state = 8;
//		}
//	}
//	else if (zkrt_curser_state == 8)											//�ֽ�10-15������UAVID
//	{
//		packet->UAVID[zkrt_uav_index] = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_uav_index++;
//		if (zkrt_uav_index == 6)
//		{
//			zkrt_uav_index = 0;
//			zkrt_curser_state = 9;
//		}
//	}
//	else if (zkrt_curser_state == 9)											//�ֽ�16���õ�command
//	{
//		packet->command = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_curser_state = 10;
//	}
//	else if (zkrt_curser_state == 10)											//�ֽ�17-46������DATA
//	{
//		packet->data[zkrt_dat_index] = ch;
//		zkrt_update_checksum(packet,ch);
//		zkrt_dat_index++;
//		if (zkrt_dat_index == packet->length)
//		{
//			zkrt_dat_index = 0;
//			zkrt_curser_state = 11;
//		}
//	}
//	else if (zkrt_curser_state == 11)	//�ֽ�47��CRC1
//	{
//#if ZK_CRC_ENABLE
//		if(ch == (uint8_t)((packet->crc)&0xff))
//			zkrt_curser_state = 12;
//		else
//			goto recv_failed;
//#else
//		  packet->crc = ch&0xff;
//			zkrt_curser_state = 12;
//#endif
//	}
//	else if (zkrt_curser_state == 12)		//�ֽ�48��CRC2
//	{
//#if ZK_CRC_ENABLE
//		if(ch == (uint8_t)((packet->crc)>>8))
//			zkrt_curser_state = 13;
//		else
//			goto recv_failed;
//#else
//		  packet->crc = (packet->crc)|(ch<<8);
//			zkrt_curser_state = 13;
//#endif
//	}
//	else if ((zkrt_curser_state == 13)&&(ch == _END_CODE))											//�ֽ�49����β
//	{
//		packet->end_code = ch;
//		zkrt_curser_state = 0;
//		zkrt_recv_success = 1;
//	}
//	else
//	{
//		goto recv_failed;
//	}
//
//	return zkrt_recv_success;
//
//recv_failed:
//		zkrt_curser_state = 0;
//		zkrt_app_index = 0;
//		zkrt_uav_index = 0;
//		zkrt_dat_index = 0;
//		return 	zkrt_recv_success;
//}
/**
 * @brief �����յ���charֵ���Ƿ�����zkrt packet��ʽ
 * @param
 * @param
 **/
uint8_t zkrt_decode_char_v2(recv_zkrt_packet_handlest *rh, uint8_t ch) {
	zkrt_packet_t *packet = &rh->packet;

	rh->recv_ok = 0;

	if ((rh->curser_state == 0) && (ch == _START_CODE)) {	//�ֽ�0���õ���ʼ��
		packet->start_code = ch;
		packet->crc = 0XFFFF;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 1;
	} else if ((rh->curser_state == 1) && (ch == _VERSION)) {	//�ֽ�1���õ��汾��
		packet->ver = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 2;
	} else if (rh->curser_state == 2) {										//�ֽ�2���õ�session
		packet->session_ack = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 3;
	} else if (rh->curser_state == 3) {										//�ֽ�3���õ�padding
		packet->padding_enc = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 4;
	} else if (rh->curser_state == 4) {										//�ֽ�4���õ�cmd
		packet->cmd = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 5;
	} else if ((rh->curser_state == 5) && (ch <= ZK_DATA_MAX_LEN)) {	//�ֽ�5���õ�����
		packet->length = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 6;
	} else if (rh->curser_state == 6) {										//�ֽ�6���������к�
		packet->seq = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 7;
	} else if (rh->curser_state == 7) {										//�ֽ�7-9������APPID
		packet->APPID[rh->app_index] = ch;
		zkrt_update_checksum(packet, ch);
		rh->app_index++;
		if (rh->app_index == 3) {
			rh->app_index = 0;
			rh->curser_state = 8;
		}
	} else if (rh->curser_state == 8) {										//�ֽ�10-15������UAVID
		packet->UAVID[rh->uav_index] = ch;
		zkrt_update_checksum(packet, ch);
		rh->uav_index++;
		if (rh->uav_index == 6) {
			rh->uav_index = 0;
			rh->curser_state = 9;
		}
	} else if (rh->curser_state == 9) {										//�ֽ�16���õ�command
		packet->command = ch;
		zkrt_update_checksum(packet, ch);
		rh->curser_state = 10;
	} else if (rh->curser_state == 10) {										//�ֽ�17-46������DATA
		packet->data[rh->dat_index] = ch;
		zkrt_update_checksum(packet, ch);
		rh->dat_index++;
		if (rh->dat_index == packet->length) {
			rh->dat_index = 0;
			rh->curser_state = 11;
		}
	} else if (rh->curser_state == 11) {	//�ֽ�47��CRC1
#if ZK_CRC_ENABLE
		if (ch == (uint8_t)((packet->crc) & 0xff))
			rh->curser_state = 12;
		else
			goto recv_failed;
#else
		packet->crc = ch & 0xff;
		rh->curser_state = 12;
#endif
	} else if (rh->curser_state == 12) {	//�ֽ�48��CRC2
#if ZK_CRC_ENABLE
		if (ch == (uint8_t)((packet->crc) >> 8))
			rh->curser_state = 13;
		else
			goto recv_failed;
#else
		packet->crc = (packet->crc) | (ch << 8);
		rh->curser_state = 13;
#endif
	} else if ((rh->curser_state == 13) && (ch == _END_CODE)) {										//�ֽ�49����β
		packet->end_code = ch;
		rh->curser_state = 0;
		rh->recv_ok = 1;
	} else {
		goto recv_failed;
	}
	return rh->recv_ok;

recv_failed:
	rh->curser_state = 0;
	rh->app_index = 0;
	rh->uav_index = 0;
	rh->dat_index = 0;
	return 	rh->recv_ok;
}
/**
 * @brief zkrt_check_packet
 * @param
 * @param
 * @return 1-check ok, 0-check fail
 **/
bool zkrt_check_packet(const zkrt_packet_t *packet) {
	u16 checkcrc;
	bool res = false;
//check start code
	if (packet->start_code != _START_CODE)
		return res;
//check ver
	if (packet->ver != _VERSION)
		return res;
//check session ack
//check padding enc
//check cmd
	if ((packet->cmd != APP_TO_UAV) &&
	        (packet->cmd != UAV_TO_APP) &&
	        (packet->cmd != UAV_TO_SUBDEV) &&
	        (packet->cmd != SUBDEV_TO_UAV) &&
	        (packet->cmd != APP_TO_UAV_SUPER) &&
	        (packet->cmd != UAV_TO_APP_SUPER))
		return res;
//check length
	if (packet->length > ZK_DATA_MAX_LEN)
		return res;
//check seq
//check appid
//check uavid
	if (((packet->UAVID[ZK_DINDEX_DEVTYPE] > DEVICE_NUMBER) &&
	        (packet->UAVID[ZK_DINDEX_DEVTYPE] < DEVICE_TYPE_LOCAL_START)) ||
	        (packet->UAVID[ZK_DINDEX_DEVTYPE] > DEVICE_TYPE_LOCAL_END))
		return res;
//check command
	if (packet->command > ZK_COMMAND_MAX)
		return res;
//check data
//check crc
#if ZK_CRC_ENABLE
	checkcrc = crc_calculate((u8*)packet, ZK_HEADER_LEN + packet->length);
	if (checkcrc != packet->crc)
		return res;
#endif
//check end code
	if (packet->end_code != _END_CODE)
		return res;
	res = true;
	return res;
}
/**
 * @brief zkrt_packet2_data
 * @param   �����͵Ļ�����
 * @param   ��ת���Ľṹ��
 * @return  �����͵����ݳ���
 **/
uint8_t zkrt_packet2_data(uint8_t *dstdata, const zkrt_packet_t *packet) {
//copy packet to senddata
	memcpy(dstdata, packet, ZK_HEADER_LEN);
	memcpy(dstdata + ZK_HEADER_LEN, packet->data, packet->length);
	memcpy(dstdata + ZK_HEADER_LEN + packet->length, packet->data + ZK_DATA_MAX_LEN, ZK_FIXED_LEN - ZK_HEADER_LEN);
//return total length
	return (ZK_FIXED_LEN + packet->length);
}
/**
 * @brief zkrt_data2_packet
 * @param
 * @param
 * @return
 **/
void zkrt_data2_packet(const uint8_t *sdata, uint8_t sdatalen, zkrt_packet_t *packet) {
//copy data to packet
	memcpy(packet, sdata, ZK_HEADER_LEN);
	memcpy(packet->data, sdata + ZK_HEADER_LEN, sdatalen - ZK_FIXED_LEN);
	memcpy(packet->data + ZK_DATA_MAX_LEN, sdata + ZK_HEADER_LEN + sdatalen - ZK_FIXED_LEN, ZK_FIXED_LEN - ZK_HEADER_LEN);
}
