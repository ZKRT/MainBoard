#ifndef  _mavlink_h
#define  _mavlink_h

#include "sys.h"
#include <string.h>
#include "Mavlink_types.h"

extern uint8_t current_tx_seq;






#ifndef MAVLINK_STX
#define MAVLINK_STX 254
#endif


//CRC校验
#define X25_INIT_CRC 0xffff
#define X25_VALIDATE_CRC 0xf0b8



#ifndef MAVLINK_ENDIAN
#define MAVLINK_ENDIAN MAVLINK_LITTLE_ENDIAN
#endif

#ifndef MAVLINK_ALIGNED_FIELDS
#define MAVLINK_ALIGNED_FIELDS 1
#endif

#ifndef MAVLINK_CRC_EXTRA
#define MAVLINK_CRC_EXTRA 1
#endif

#define MAVLINK_USE_CONVENIENCE_FUNCTIONS


//这后面有相关的调试，要求CRC统一
#ifndef MAVLINK_MESSAGE_CRCS
#define MAVLINK_MESSAGE_CRCS {50, 124, 137, 0, 237, 217, 104, 119, 0, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 214, 159, 220, 168, 24, 23, 170, 144, 67, 115, 39, 246, 185, 104, 237, 244, 222, 212, 9, 254, 230, 28, 28, 132, 221, 232, 11, 153, 41, 39, 0, 0, 0, 0, 15, 3, 0, 0, 0, 0, 0, 153, 183, 51, 82, 118, 148, 21, 0, 243, 124, 0, 0, 38, 20, 158, 152, 143, 0, 0, 0, 106, 49, 22, 29, 12, 241, 233, 0, 231, 183, 63, 54, 0, 0, 0, 0, 0, 0, 0, 175, 102, 158, 208, 56, 93, 211, 108, 32, 185, 84, 0, 0, 124, 119, 4, 76, 128, 56, 116, 134, 237, 203, 250, 87, 203, 220, 25, 226, 0, 29, 223, 85, 6, 229, 203, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 49, 0, 134, 219, 208, 188, 84, 22, 19, 21, 134, 0, 78, 68, 189, 127, 154, 21, 21, 144, 1, 234, 73, 181, 22, 83, 167, 138, 234, 240, 47, 189, 52, 174, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 123, 205, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 204, 49, 170, 44, 83, 46, 0}
																																																																																																																																																																																																																																																																																																																																																																																																																																																						//163, 115, 47, 0, 0, 0, 0, 0, 0, 207, 0, 205, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,90, 0, 0, 0, 0, 0, 0, 8, 204, 49, 170, 44, 83, 46, 0}
#endif

/* 
   If you want MAVLink on a system that is native big-endian,
   you need to define NATIVE_BIG_ENDIAN
*/
#ifdef NATIVE_BIG_ENDIAN  //Little-endian is the default memory format for ARM processors.
# define MAVLINK_NEED_BYTE_SWAP (MAVLINK_ENDIAN == MAVLINK_LITTLE_ENDIAN)  //1
#else
# define MAVLINK_NEED_BYTE_SWAP (MAVLINK_ENDIAN != MAVLINK_LITTLE_ENDIAN)  //0
#endif

void mav_array_memcpy(void *dest, const void *src, size_t n);
void _mav_put_char_array(char *buf, uint8_t wire_offset, const char *b, uint8_t array_length);
void _mav_put_uint8_t_array(char *buf, uint8_t wire_offset, const uint8_t *b, uint8_t array_length);
void _mav_put_int8_t_array(char *buf, uint8_t wire_offset, const int8_t *b, uint8_t array_length);



#define _mav_put_uint8_t(buf, wire_offset, b) buf[wire_offset] = (uint8_t)b
#define _mav_put_int8_t(buf, wire_offset, b)  buf[wire_offset] = (int8_t)b
#define _mav_put_char(buf, wire_offset, b)    buf[wire_offset] = b

#if MAVLINK_NEED_BYTE_SWAP
#define _mav_put_uint16_t(buf, wire_offset, b) byte_swap_2(&buf[wire_offset], (const char *)&b)
#define _mav_put_int16_t(buf, wire_offset, b)  byte_swap_2(&buf[wire_offset], (const char *)&b)
#define _mav_put_uint32_t(buf, wire_offset, b) byte_swap_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_int32_t(buf, wire_offset, b)  byte_swap_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_uint64_t(buf, wire_offset, b) byte_swap_8(&buf[wire_offset], (const char *)&b)
#define _mav_put_int64_t(buf, wire_offset, b)  byte_swap_8(&buf[wire_offset], (const char *)&b)
#define _mav_put_float(buf, wire_offset, b)    byte_swap_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_double(buf, wire_offset, b)   byte_swap_8(&buf[wire_offset], (const char *)&b)
#elif !MAVLINK_ALIGNED_FIELDS
#define _mav_put_uint16_t(buf, wire_offset, b) byte_copy_2(&buf[wire_offset], (const char *)&b)
#define _mav_put_int16_t(buf, wire_offset, b)  byte_copy_2(&buf[wire_offset], (const char *)&b)
#define _mav_put_uint32_t(buf, wire_offset, b) byte_copy_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_int32_t(buf, wire_offset, b)  byte_copy_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_uint64_t(buf, wire_offset, b) byte_copy_8(&buf[wire_offset], (const char *)&b)
#define _mav_put_int64_t(buf, wire_offset, b)  byte_copy_8(&buf[wire_offset], (const char *)&b)
#define _mav_put_float(buf, wire_offset, b)    byte_copy_4(&buf[wire_offset], (const char *)&b)
#define _mav_put_double(buf, wire_offset, b)   byte_copy_8(&buf[wire_offset], (const char *)&b)
#else
#define _mav_put_uint16_t(buf, wire_offset, b) *(uint16_t *)&buf[wire_offset] = b
#define _mav_put_int16_t(buf, wire_offset, b)  *(int16_t *)&buf[wire_offset] = b
#define _mav_put_uint32_t(buf, wire_offset, b) *(uint32_t *)&buf[wire_offset] = b
#define _mav_put_int32_t(buf, wire_offset, b)  *(int32_t *)&buf[wire_offset] = b
#define _mav_put_uint64_t(buf, wire_offset, b) *(uint64_t *)&buf[wire_offset] = b
#define _mav_put_int64_t(buf, wire_offset, b)  *(int64_t *)&buf[wire_offset] = b
#define _mav_put_float(buf, wire_offset, b)    *(float *)&buf[wire_offset] = b
#define _mav_put_double(buf, wire_offset, b)   *(double *)&buf[wire_offset] = b
#endif


#if MAVLINK_NEED_BYTE_SWAP
#define _MAV_PUT_ARRAY(TYPE, V) \
static inline void _mav_put_ ## TYPE ##_array(char *buf, uint8_t wire_offset, const TYPE *b, uint8_t array_length) \
{ \
	if (b == NULL) { \
		memset(&buf[wire_offset], 0, array_length*sizeof(TYPE)); \
	} else { \
		uint16_t i; \
		for (i=0; i<array_length; i++) { \
			_mav_put_## TYPE (buf, wire_offset+(i*sizeof(TYPE)), b[i]); \
		} \
	} \
}
#else
#define _MAV_PUT_ARRAY(TYPE, V)					\
static inline void _mav_put_ ## TYPE ##_array(char *buf, uint8_t wire_offset, const TYPE *b, uint8_t array_length) \
{ \
	mav_array_memcpy(&buf[wire_offset], b, array_length*sizeof(TYPE)); \
}
#endif


#define _MAV_RETURN_char(msg, wire_offset)     (const char)_MAV_PAYLOAD(msg)[wire_offset]
#define _MAV_RETURN_int8_t(msg, wire_offset)   (const int8_t)_MAV_PAYLOAD(msg)[wire_offset]
#define _MAV_RETURN_uint8_t(msg, wire_offset)  _MAV_PAYLOAD(msg)[wire_offset]//以前这里有(const uint8_t)前缀，会导致无意义的修饰类型的警告，也就是uint8_t转化为uint8_t是无意义的

#if MAVLINK_NEED_BYTE_SWAP
#define _MAV_MSG_RETURN_TYPE(TYPE, SIZE) \
TYPE _MAV_RETURN_## TYPE(const mavlink_message_t *msg, uint8_t ofs) \
{ TYPE r; byte_swap_## SIZE((char*)&r, &_MAV_PAYLOAD(msg)[ofs]); return r; }


#elif !MAVLINK_ALIGNED_FIELDS
#define _MAV_MSG_RETURN_TYPE(TYPE, SIZE) \
TYPE _MAV_RETURN_## TYPE(const mavlink_message_t *msg, uint8_t ofs) \
{ TYPE r; byte_copy_## SIZE((char*)&r, &_MAV_PAYLOAD(msg)[ofs]); return r; }

#else // nicely aligned, no swap
#define _MAV_MSG_RETURN_TYPE(TYPE) \
TYPE _MAV_RETURN_## TYPE(const mavlink_message_t *msg, uint8_t ofs) \
{ return *(const TYPE *)(&_MAV_PAYLOAD(msg)[ofs]);}

#endif // MAVLINK_NEED_BYTE_SWAP


void crc_accumulate(uint8_t data, uint16_t *crcAccum);
void crc_accumulate_buffer(uint16_t *crcAccum, const char *pBuffer, uint16_t length);
void crc_init(uint16_t* crcAccum);
uint16_t crc_calculate(const uint8_t* pBuffer, uint16_t length);




uint16_t _MAV_RETURN_char_array(const mavlink_message_t *msg, char *value, uint8_t array_length, uint8_t wire_offset);
uint16_t _MAV_RETURN_uint8_t_array(const mavlink_message_t *msg, uint8_t *value, uint8_t array_length, uint8_t wire_offset);
uint16_t _MAV_RETURN_int8_t_array(const mavlink_message_t *msg, int8_t *value, uint8_t array_length, uint8_t wire_offset);


//对已经指定的消息ID、数据、数据长度、外部校验码进行mavlink封装（其中系统ID和单元ID已经自定义），并且使用串口发送
//这里分3次发送，会不会有发送重叠的危险？？

#if MAVLINK_CRC_EXTRA
void _mav_finalize_message_chan_send(mavlink_channel_t chan, uint8_t msgid, const char *packet, uint8_t length, uint8_t crc_extra);
#else
void _mav_finalize_message_chan_send(mavlink_channel_t chan, uint8_t msgid, const char *packet, uint8_t length);
#endif

//传入个结构体msg，msg里包含了数据部分；再将参数部分传入并且校验，使得msg变为一个完整的mavlink指令
#if MAVLINK_CRC_EXTRA
uint16_t mavlink_finalize_message_chan(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t chan, uint8_t length, uint8_t crc_extra);
#else
uint16_t mavlink_finalize_message_chan(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t chan, uint8_t length);
#endif

/**
 * @brief Finalize a MAVLink message with MAVLINK_COMM_0 as default channel
 //这个函数与上面的唯一区别在于：它调用了上面的函数，指定了COM0作为发射口
 */
#if MAVLINK_CRC_EXTRA
 uint16_t mavlink_finalize_message(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t length, uint8_t crc_extra);

#else
 uint16_t mavlink_finalize_message(mavlink_message_t* msg, uint8_t system_id, uint8_t component_id, uint8_t length);

#endif

//从buff里取出一个字符后，对这个字符进行校验，当最后一个字符被解析后，返回值是1
uint8_t mavlink_parse_char(uint8_t chan, uint8_t c, mavlink_message_t* r_message, mavlink_status_t* r_mavlink_status);
uint32_t _MAV_RETURN_uint32_t(const mavlink_message_t *msg, uint8_t ofs);



#endif 
