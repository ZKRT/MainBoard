#ifndef _HEARTBEAT
#define _HEARTBEAT
// MESSAGE HEARTBEAT PACKING
#include "sys.h"//这里包含了所有mavlink指令所需的内容"
#include "mavlink.h"

#define MAVLINK_MSG_ID_HEARTBEAT 0

typedef struct __mavlink_heartbeat_t
{
 uint32_t custom_mode; ///< A bitfield for use for autopilot-specific flags.
 uint8_t type; ///< Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
 uint8_t autopilot; ///< Autopilot type / class. defined in MAV_AUTOPILOT ENUM
 uint8_t base_mode; ///< System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
 uint8_t system_status; ///< System status flag, see MAV_STATE ENUM
 uint8_t mavlink_version; ///< MAVLink version, not writable by user, gets added by protocol because of magic data type: uint8_t_mavlink_version
} mavlink_heartbeat_t;

#define MAVLINK_MSG_ID_HEARTBEAT_LEN 9
#define MAVLINK_MSG_ID_0_LEN 9

#define MAVLINK_MSG_ID_HEARTBEAT_CRC 50
#define MAVLINK_MSG_ID_0_CRC 50



#define MAVLINK_MESSAGE_INFO_HEARTBEAT { \
	"HEARTBEAT", \
	6, \
	{  { "custom_mode", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_heartbeat_t, custom_mode) }, \
         { "type", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_heartbeat_t, type) }, \
         { "autopilot", NULL, MAVLINK_TYPE_UINT8_T, 0, 5, offsetof(mavlink_heartbeat_t, autopilot) }, \
         { "base_mode", NULL, MAVLINK_TYPE_UINT8_T, 0, 6, offsetof(mavlink_heartbeat_t, base_mode) }, \
         { "system_status", NULL, MAVLINK_TYPE_UINT8_T, 0, 7, offsetof(mavlink_heartbeat_t, system_status) }, \
         { "mavlink_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_heartbeat_t, mavlink_version) }, \
         } \
}


/**
 * @brief Pack a heartbeat message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param type Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
 * @param autopilot Autopilot type / class. defined in MAV_AUTOPILOT ENUM
 * @param base_mode System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
 * @param custom_mode A bitfield for use for autopilot-specific flags.
 * @param system_status System status flag, see MAV_STATE ENUM
 * @return length of the message in bytes (excluding serial stream start sign)

心跳包封包函数，根据sysid、compid、各种心跳数据信息，再结合msgid=0，调用mavlink_finalize_message，将封包结果保存到msg结构体里
 */
uint16_t mavlink_msg_heartbeat_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status);


/**
 * @brief Pack a heartbeat message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param type Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
 * @param autopilot Autopilot type / class. defined in MAV_AUTOPILOT ENUM
 * @param base_mode System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
 * @param custom_mode A bitfield for use for autopilot-specific flags.
 * @param system_status System status flag, see MAV_STATE ENUM
 * @return length of the message in bytes (excluding serial stream start sign)
 
 心跳包封包函数，根据sysid、compid、各种心跳数据信息，再结合msgid=0，调用mavlink_finalize_message_chan，将封包结果保存到msg结构体里，并指定了要发送的通道
 */
uint16_t mavlink_msg_heartbeat_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,mavlink_message_t* msg,
						           uint8_t type,uint8_t autopilot,uint8_t base_mode,uint32_t custom_mode,uint8_t system_status);


/**
 * @brief Encode a heartbeat struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat C-struct to read the message contents from
 
 心跳包封包，传入的msg里保存了外部信息，传入的heartbeat里保存了数据信息，这里将它们拆分成若干个参数而调用mavlink_msg_heartbeat_pack将指令封包在msg里
 */
uint16_t mavlink_msg_heartbeat_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_heartbeat_t* heartbeat);


/**
 * @brief Encode a heartbeat struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param heartbeat C-struct to read the message contents from
 
 心跳包封包，传入的msg里保存了外部信息，传入的heartbeat里保存了数据信息，这里将它们拆分成若干个参数而调用mavlink_msg_heartbeat_pack_chan将指令封包在msg里，并指定发送通道
 */
uint16_t mavlink_msg_heartbeat_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_heartbeat_t* heartbeat);


/**
 * @brief Send a heartbeat message
 * @param chan MAVLink channel to send the message
 *
 * @param type Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
 * @param autopilot Autopilot type / class. defined in MAV_AUTOPILOT ENUM
 * @param base_mode System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
 * @param custom_mode A bitfield for use for autopilot-specific flags.
 * @param system_status System status flag, see MAV_STATE ENUM
 
 发送心跳包数据：先封包具体的心跳数据，再添加msgid=0，len=9，extra_crc=50等信息，调用_mav_finalize_message_chan_send进行DMA发送，这个最常用！！！
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

void mavlink_msg_heartbeat_send(mavlink_channel_t chan, uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status);


#if MAVLINK_MSG_ID_HEARTBEAT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
	
这个函数与上一个完全相同，但它有个特殊功能：
传参多了一个msgbuf，函数中整理心跳包的数据时会将心跳数据赋值到msgbuf里，导致调用send函数的结构体里保存了数据信息，可以随时查看
但这对send函数是没有什么用处的，这个函数不常用
 */
void mavlink_msg_heartbeat_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status);

#endif

#endif

// MESSAGE HEARTBEAT UNPACKING


/**
 * @brief Get field type from heartbeat message
 *
 * @return Type of the MAV (quadrotor, helicopter, etc., up to 15 types, defined in MAV_TYPE ENUM)
 */
uint8_t mavlink_msg_heartbeat_get_type(const mavlink_message_t* msg);


/**
 * @brief Get field autopilot from heartbeat message
 *
 * @return Autopilot type / class. defined in MAV_AUTOPILOT ENUM
 */
uint8_t mavlink_msg_heartbeat_get_autopilot(const mavlink_message_t* msg);


/**
 * @brief Get field base_mode from heartbeat message
 *
 * @return System mode bitfield, see MAV_MODE_FLAG ENUM in mavlink/include/mavlink_types.h
 */
uint8_t mavlink_msg_heartbeat_get_base_mode(const mavlink_message_t* msg);


/**
 * @brief Get field custom_mode from heartbeat message
 *
 * @return A bitfield for use for autopilot-specific flags.
 */
uint32_t mavlink_msg_heartbeat_get_custom_mode(const mavlink_message_t* msg);


/**
 * @brief Get field system_status from heartbeat message
 *
 * @return System status flag, see MAV_STATE ENUM
 */
uint8_t mavlink_msg_heartbeat_get_system_status(const mavlink_message_t* msg);


/**
 * @brief Get field mavlink_version from heartbeat message
 *
 * @return MAVLink version, not writable by user, gets added by protocol because of magic data type: uint8_t_mavlink_version
 */
uint8_t mavlink_msg_heartbeat_get_mavlink_version(const mavlink_message_t* msg);


/**
 * @brief Decode a heartbeat message into a struct
 *
 * @param msg The message to decode
 * @param heartbeat C-struct to decode the message contents into
 
 解包心跳包，将解析出来的数据保存到heartbeat里
 */
void mavlink_msg_heartbeat_decode(const mavlink_message_t* msg, mavlink_heartbeat_t* heartbeat);
#endif

