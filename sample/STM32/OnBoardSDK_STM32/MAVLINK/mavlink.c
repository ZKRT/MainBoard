#include "mavlink.h"
#include "usart.h"								//用串口发送








uint8_t current_tx_seq = 0;

_MAV_MSG_RETURN_TYPE(uint16_t)
_MAV_MSG_RETURN_TYPE(int16_t)
_MAV_MSG_RETURN_TYPE(uint32_t)
_MAV_MSG_RETURN_TYPE(int32_t)
_MAV_MSG_RETURN_TYPE(uint64_t)
_MAV_MSG_RETURN_TYPE(int64_t)
_MAV_MSG_RETURN_TYPE(float)
_MAV_MSG_RETURN_TYPE(double)


//将msg结构体里的payload[0]偏移offset单位后的地址之后的数据，复制到value这个数组里，长度保持array_length
uint16_t _MAV_RETURN_char_array(const mavlink_message_t *msg, char *value, uint8_t array_length, uint8_t wire_offset)
{
	memcpy(value, &_MAV_PAYLOAD(msg)[wire_offset], array_length);
	return array_length;
}

uint16_t _MAV_RETURN_uint8_t_array(const mavlink_message_t *msg, uint8_t *value, uint8_t array_length, uint8_t wire_offset)
{
	memcpy(value, &_MAV_PAYLOAD(msg)[wire_offset], array_length);
	return array_length;
}

uint16_t _MAV_RETURN_int8_t_array(const mavlink_message_t *msg, int8_t *value, uint8_t array_length, uint8_t wire_offset)
{
	memcpy(value, &_MAV_PAYLOAD(msg)[wire_offset], array_length);
	return array_length;
}


/*
  like memcpy(), but if src is NULL, do a memset to zero
  数组复制，如果数组没有指向，那么清空数组
*/
void mav_array_memcpy(void *dest, const void *src, size_t n)
{
	if (src == NULL) {
		memset(dest, 0, n);
	} else {
		memcpy(dest, src, n);
	}
}

/*
 * Place a char array into a buffer
 * 将char型数组复制到buffer里
 */
void _mav_put_char_array(char *buf, uint8_t wire_offset, const char *b, uint8_t array_length)
{
	mav_array_memcpy(&buf[wire_offset], b, array_length);

}

/*
 * Place a uint8_t array into a buffer
 * 将无符号char型数组复制到buffer里
 */
void _mav_put_uint8_t_array(char *buf, uint8_t wire_offset, const uint8_t *b, uint8_t array_length)
{
	mav_array_memcpy(&buf[wire_offset], b, array_length);

}

/*
 * Place a int8_t array into a buffer
 * 这个也是char型数组复制，重复定义了
 */
void _mav_put_int8_t_array(char *buf, uint8_t wire_offset, const int8_t *b, uint8_t array_length)
{
	mav_array_memcpy(&buf[wire_offset], b, array_length);

}

/**
 * @brief Accumulate the X.25 CRC by adding one char at a time.
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new char to hash
 * @param crcAccum the already accumulated checksum
 **/
void crc_accumulate(uint8_t data, uint16_t *crcAccum)
{
        /*Accumulate one byte of data into the CRC*/
        uint8_t tmp;

        tmp = data ^ (uint8_t)(*crcAccum &0xff);
        tmp ^= (tmp<<4);
        *crcAccum = (*crcAccum>>8) ^ (tmp<<8) ^ (tmp <<3) ^ (tmp>>4);
}


/**
 * @brief Accumulate the X.25 CRC by adding an array of bytes
 *
 * The checksum function adds the hash of one char at a time to the
 * 16 bit checksum (uint16_t).
 *
 * @param data new bytes to hash
 * @param crcAccum the already accumulated checksum
 **/
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length)
{
	const uint8_t *p = (const uint8_t *)pBuffer;
	while (length--) {
                crc_accumulate(*p++, crcAccum);
        }
}

/**
 * @brief Initiliaze the buffer for the X.25 CRC
 *
 * @param crcAccum the 16 bit X.25 CRC
 */
void crc_init(uint16_t* crcAccum)
{
        *crcAccum = X25_INIT_CRC;
}

/**
 * @brief Calculates the X.25 checksum on a byte buffer
 *
 * @param  pBuffer buffer containing the byte array to hash
 * @param  length  length of the byte array
 * @return the checksum over the buffer bytes
 **/
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length)
{
        uint16_t crcTmp;
        crc_init(&crcTmp);
				while (length--) 
				{
					crc_accumulate(*pBuffer++, &crcTmp);
				}
        return crcTmp;
}


void _mavlink_send_uart(mavlink_channel_t chan, const char *buf, uint16_t len)
{
	usart1_tx_copyed(buf, len);  
}


/**
 * @brief Finalize a MAVLink message with channel assignment and send
调用的情况：已知所有的要发送的信息，要将其发送出去
函数的功能：对已经指定的消息ID、数据、数据长度、外部校验码进行mavlink封装，并且使用串口发送
操作前环境：buf是新建的6个长度的数组（值不定），seq是某个值，ck是新建的2个长度的数组
操作后环境：buf是mavlink指令的前6位，seq++，ck是包含两个crc校验码的数组
调用这个函数之前记得把seq定义个全局变量保存下来
*/
#if MAVLINK_CRC_EXTRA
void _mav_finalize_message_chan_send(mavlink_channel_t chan, uint8_t msgid, const char *packet, uint8_t length, uint8_t crc_extra)
#else
void _mav_finalize_message_chan_send(mavlink_channel_t chan, uint8_t msgid, const char *packet, uint8_t length)
#endif
{
	uint16_t checksum;
	uint8_t buf[MAVLINK_NUM_HEADER_BYTES];
	uint8_t ck[2];
	//mavlink_status_t *status = mavlink_get_channel_status(chan);
	buf[0] = MAVLINK_STX;   
	buf[1] = length;        
	buf[2] = current_tx_seq;
	buf[3] = 1;            
	buf[4] = 255;           
	buf[5] = msgid;        
	current_tx_seq++;       
	current_tx_seq%=0xff;
	checksum = crc_calculate((const uint8_t*)&buf[1], MAVLINK_CORE_HEADER_LEN);
	crc_accumulate_buffer(&checksum, packet, length);
#if MAVLINK_CRC_EXTRA
	crc_accumulate(crc_extra, &checksum);
#endif
	ck[0] = (uint8_t)(checksum & 0xFF);
	ck[1] = (uint8_t)(checksum >> 8);

	
	_mavlink_send_uart(chan, (const char *)buf, MAVLINK_NUM_HEADER_BYTES); //ldl 2016 09 12
	_mavlink_send_uart(chan, packet, length);                                                       //ldl 2016 09 12            
	_mavlink_send_uart(chan, (const char *)ck, 2);                                               //ldl 2016 09 12

//	sendToMobile((uint8_t *)packet,length);

//	usart1_tx_DMA();	  //ldl 2016 09 12
}


/**
 * @brief Finalize a MAVLink message with channel assignment
 *
 * This function calculates the checksum and sets length and aircraft id correctly.
 * It assumes that the message id and the payload are already correctly set. This function
 * can also be used if the message header has already been written before (as in mavlink_msg_xxx_pack
 * instead of mavlink_msg_xxx_pack_headerless), it just introduces little extra overhead.
 *
 * @param msg Message to finalize
 * @param system_id Id of the sending (this) system, 1-127
 * @param length Message length
 */
#if MAVLINK_CRC_EXTRA
uint16_t mavlink_finalize_message_chan(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t chan, uint8_t length, uint8_t crc_extra)
#else
uint16_t mavlink_finalize_message_chan(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t chan, uint8_t length)
#endif
{
	// This code part is the same for all messages;
	msg->magic = MAVLINK_STX;   
	msg->len = length;         
	msg->sysid = system_id;    
	msg->compid = component_id; 
	// One sequence number per component
	msg->seq = current_tx_seq;  
	current_tx_seq++;          
	current_tx_seq%=0xff;
	msg->checksum = crc_calculate(((const uint8_t*)(msg)) + 3, MAVLINK_CORE_HEADER_LEN);
	crc_accumulate_buffer(&msg->checksum, _MAV_PAYLOAD(msg), msg->len);
#if MAVLINK_CRC_EXTRA
	crc_accumulate(crc_extra, &msg->checksum);
#endif
	mavlink_ck_a(msg) = (uint8_t)(msg->checksum & 0xFF);
	mavlink_ck_b(msg) = (uint8_t)(msg->checksum >> 8);  

	return length + MAVLINK_NUM_NON_PAYLOAD_BYTES;
}


/**
 * @brief Finalize a MAVLink message with MAVLINK_COMM_0 as default channel

 */
#if MAVLINK_CRC_EXTRA
 uint16_t mavlink_finalize_message(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t length, uint8_t crc_extra)
{
	return mavlink_finalize_message_chan(msg, system_id, component_id, MAVLINK_COMM_0, length, crc_extra);
}
#else
 uint16_t mavlink_finalize_message(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t length)
{
	return mavlink_finalize_message_chan(msg, system_id, component_id, MAVLINK_COMM_0, length);
}
#endif







//接收处理部分
//*********************************************************************************************************************************************************

void mavlink_start_checksum(mavlink_message_t* msg)
{
	crc_init(&msg->checksum);
}

void mavlink_update_checksum(mavlink_message_t* msg, uint8_t c)
{
	crc_accumulate(c, &msg->checksum);
}

/**
 * This is a convenience function which handles the complete MAVLink parsing.
 * the function will parse one byte at a time and return the complete packet once
 * it could be successfully decoded. Checksum and other failures will be silently
 * ignored.
 *
 * Messages are parsed into an internal buffer (one for each channel). When a complete
 * message is received it is copies into *returnMsg and the channel's status is
 * copied into *returnStats.
 *
 * @param chan     ID of the current channel. This allows to parse different channels with this function.
 *                 a channel is not a physical message channel like a serial port, but a logic partition of
 *                 the communication streams in this case. COMM_NB is the limit for the number of channels
 *                 on MCU (e.g. ARM7), while COMM_NB_HIGH is the limit for the number of channels in Linux/Windows
 * @param c        The char to parse
 *
 * @param returnMsg NULL if no message could be decoded, the message data else
 * @param returnStats if a message was decoded, this is filled with the channel's stats
 * @return 0 if no message could be decoded, 1 else
 *
 * A typical use scenario of this function call is:
 *
 * @code
 * #include <inttypes.h> // For fixed-width uint8_t type
 *
 * mavlink_message_t msg;
 * int chan = 0;
 *
 *
 * while(serial.bytesAvailable > 0)
 * {
 *   uint8_t byte = serial.getNextByte();
 *   if (mavlink_parse_char(chan, byte, &msg))
 *     {
 *     printf("Received message with ID %d, sequence: %d from component %d of system %d", msg.msgid, msg.seq, msg.compid, msg.sysid);
 *     }
 * }
 *
 *
 * @endcode
 */
 
 
 mavlink_status_t m_mavlink_status;
 mavlink_message_t m_mavlink_buffer;
 
uint8_t mavlink_parse_char(uint8_t chan, uint8_t c, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status)
{
        /*
	  default message crc function. You can override this per-system to
	  put this data in a different memory segment
	*/
#if MAVLINK_CRC_EXTRA
#ifndef MAVLINK_MESSAGE_CRC
	static const uint8_t mavlink_message_crcs[256] = MAVLINK_MESSAGE_CRCS;
#define MAVLINK_MESSAGE_CRC(msgid) mavlink_message_crcs[msgid]
#endif
#endif

/* Enable this option to check the length of each message.
 This allows invalid messages to be caught much sooner. Use if the transmission
 medium is prone to missing (or extra) characters (e.g. a radio that fades in
 and out). Only use if the channel will only contain messages types listed in
 the headers.
*/
#if MAVLINK_CHECK_MESSAGE_LENGTH
#ifndef MAVLINK_MESSAGE_LENGTH
	static const uint8_t mavlink_message_lengths[256] = MAVLINK_MESSAGE_LENGTHS;
#define MAVLINK_MESSAGE_LENGTH(msgid) mavlink_message_lengths[msgid]
#endif
#endif

	mavlink_message_t* rxmsg = &m_mavlink_buffer;
	mavlink_status_t* status = &m_mavlink_status; 
	int bufferIndex = 0;

	status->msg_received = 0;

	switch (status->parse_state)
	{
	case MAVLINK_PARSE_STATE_UNINIT:
	case MAVLINK_PARSE_STATE_IDLE:
		if (c == MAVLINK_STX)
		{
			status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
			rxmsg->len = 0;
			rxmsg->magic = c;
			mavlink_start_checksum(rxmsg);
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_STX:
			if (status->msg_received
/* Support shorter buffers than the
   default maximum packet size */
#if (MAVLINK_MAX_PAYLOAD_LEN < 255)
				|| c > MAVLINK_MAX_PAYLOAD_LEN
#endif
				)
		{
			status->buffer_overrun++;
			status->parse_error++;
			status->msg_received = 0;
			status->parse_state = MAVLINK_PARSE_STATE_IDLE;
		}
		else//正确的话执行这一步。
		{
			// NOT counting STX, LENGTH, SEQ, SYSID, COMPID, MSGID, CRC1 and CRC2
			rxmsg->len = c;
			status->packet_idx = 0;
			mavlink_update_checksum(rxmsg, c);
			status->parse_state = MAVLINK_PARSE_STATE_GOT_LENGTH;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_LENGTH:
		rxmsg->seq = c;
		mavlink_update_checksum(rxmsg, c);
		status->parse_state = MAVLINK_PARSE_STATE_GOT_SEQ;
		break;

	case MAVLINK_PARSE_STATE_GOT_SEQ:
		rxmsg->sysid = c;
		mavlink_update_checksum(rxmsg, c);
		status->parse_state = MAVLINK_PARSE_STATE_GOT_SYSID;
		break;

	case MAVLINK_PARSE_STATE_GOT_SYSID:
		rxmsg->compid = c;
		mavlink_update_checksum(rxmsg, c);
		status->parse_state = MAVLINK_PARSE_STATE_GOT_COMPID;
		break;

	case MAVLINK_PARSE_STATE_GOT_COMPID:
#if MAVLINK_CHECK_MESSAGE_LENGTH
	        if (rxmsg->len != MAVLINK_MESSAGE_LENGTH(c))
		{
			status->parse_error++;
			status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			break;
			if (c == MAVLINK_STX)
			{
				status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
				mavlink_start_checksum(rxmsg);
			}
	        }
#endif
		rxmsg->msgid = c;
		mavlink_update_checksum(rxmsg, c);
		if (rxmsg->len == 0)//如果长度是0
		{
			status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
		}
		else//如果长度非0
		{
			status->parse_state = MAVLINK_PARSE_STATE_GOT_MSGID;//状态是已经得到msgid
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_MSGID:
		_MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx++] = (char)c;
		mavlink_update_checksum(rxmsg, c);
		if (status->packet_idx == rxmsg->len)
		{
			status->parse_state = MAVLINK_PARSE_STATE_GOT_PAYLOAD;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_PAYLOAD:
#if MAVLINK_CRC_EXTRA
		mavlink_update_checksum(rxmsg, MAVLINK_MESSAGE_CRC(rxmsg->msgid));
#endif
		if (c != (rxmsg->checksum & 0xFF)) {
			// Check first checksum byte
			status->parse_error++;
			status->msg_received = 0;
			status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			if (c == MAVLINK_STX)
			{                    
				status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
				rxmsg->len = 0;
				mavlink_start_checksum(rxmsg);
			}
		}
		else//如果接收校验位低位是正确的
		{
			status->parse_state = MAVLINK_PARSE_STATE_GOT_CRC1;
			_MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx] = (char)c;
		}
		break;

	case MAVLINK_PARSE_STATE_GOT_CRC1:
		if (c != (rxmsg->checksum >> 8)) {
			// Check second checksum byte
			status->parse_error++;//
			status->msg_received = 0;
			status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			if (c == MAVLINK_STX)
			{
				status->parse_state = MAVLINK_PARSE_STATE_GOT_STX;
				rxmsg->len = 0;
				mavlink_start_checksum(rxmsg);
			}
		}
		else
		{
			// Successfully got message
			status->msg_received = 1;
			status->parse_state = MAVLINK_PARSE_STATE_IDLE;
			_MAV_PAYLOAD_NON_CONST(rxmsg)[status->packet_idx+1] = (char)c;
			memcpy(r_message, rxmsg, sizeof(mavlink_message_t));
		}
		break;
	}

	bufferIndex++;//这有什么用？？？？？？？？？？？
	// If a message has been sucessfully decoded, check index
	if (status->msg_received == 1)
	{
		//while(status->current_seq != rxmsg->seq)
		//{
		//	status->packet_rx_drop_count++;
		//               status->current_seq++;
		//}
		status->current_rx_seq = rxmsg->seq;
		// Initial condition: If no packet has been received so far, drop count is undefined
		if (status->packet_rx_success_count == 0) status->packet_rx_drop_count = 0;
		// Count this packet as received
		status->packet_rx_success_count++;
	}

	r_mavlink_status->current_rx_seq = status->current_rx_seq+1;
																															
	r_mavlink_status->packet_rx_success_count = status->packet_rx_success_count;
	r_mavlink_status->packet_rx_drop_count = status->parse_error;
	status->parse_error = 0;
	return status->msg_received;
}



