// MESSAGE STATUSTEXT PACKING
#include "Mavlink_types.h"
#include "mavlink.h"
#include "stm32f4xx.h" 
#include<string.h>


#define MAVLINK_MSG_ID_MANGEMENT_MODULE 246

typedef struct __mavlink_zkrt_packet_t
{
	uint8_t start_code;
	uint8_t ver;
	uint8_t sission_identifier;//	uint8_t identifier;
	uint8_t padding_ENC;
	uint8_t cmd;
	uint16_t lenght;
	uint8_t seq;
	uint8_t type;
	uint8_t SenderID[8];
	uint8_t ReceiverID[8];
	uint8_t Command[3];
	uint8_t Data[24];
	uint16_t crc;
	uint8_t end_code;
} mavlink_zkrt_packet_t;








typedef struct __mavlink_management_module_t
{
   mavlink_zkrt_packet_t zkrt_packet; ///< Status text message, without null termination character
} mavlink_management_module_t;

#define MAVLINK_MSG_ID_MANGEMENT_MODULE_LEN 50
#define MAVLINK_MSG_ID_246_LEN 50

#define MAVLINK_MSG_ID_MANGEMENT_MODULE_CRC 88
#define MAVLINK_MSG_ID_246_CRC 88

#define MAVLINK_MSG_MANGEMENT_MODULE_TEXT_LEN 50

#define MAVLINK_MESSAGE_INFO_MANGEMENT_MODULE { \
	"MANGEMENTMODULE", \
	 1, \
	{  \
         { "zkrt_packet", NULL, MAVLINK_TYPE_CHAR, 50, 1, offsetof(mavlink_management_module_t, zkrt_packet) }, \
  } \
}


/**
 * @brief Pack a statustext message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param severity Severity of status. Relies on the definitions within RFC-5424. See enum MAV_SEVERITY.
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
uint16_t mavlink_msg_management_module_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const char *text);

/**
 * @brief Pack a statustext message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param severity Severity of status. Relies on the definitions within RFC-5424. See enum MAV_SEVERITY.
 * @param text Status text message, without null termination character
 * @return length of the message in bytes (excluding serial stream start sign)
 */
 uint16_t mavlink_msg_management_module_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,mavlink_message_t* msg,const char *in_packet);

/**
 * @brief Encode a statustext struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param statustext C-struct to read the message contents from
 */
uint16_t mavlink_msg_management_module_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_management_module_t* in_packet);


/**
 * @brief Encode a statustext struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param statustext C-struct to read the message contents from
 */
uint16_t mavlink_msg_management_module_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_management_module_t* in_packet);


/**
 * @brief Send a statustext message
 * @param chan MAVLink channel to send the message
 *
 * @param severity Severity of status. Relies on the definitions within RFC-5424. See enum MAV_SEVERITY.
 * @param text Status text message, without null termination character
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

void mavlink_msg_management_module_send(mavlink_channel_t chan, const char *in_packet);


#if MAVLINK_MSG_ID_STATUSTEXT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
void mavlink_msg_management_module_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan, const char *in_packet);

#endif

#endif

// MESSAGE STATUSTEXT UNPACKING


/**
 * @brief Get field severity from statustext message
 *
 * @return Severity of status. Relies on the definitions within RFC-5424. See enum MAV_SEVERITY.
 */
uint8_t mavlink_msg_management_module_get_severity(const mavlink_message_t* msg);


/**
 * @brief Get field text from statustext message
 *
 * @return Status text message, without null termination character
 */
uint16_t mavlink_msg_management_module_get_text(const mavlink_message_t* msg, char *text);


/**
 * @brief Decode a statustext message into a struct
 *
 * @param msg The message to decode
 * @param statustext C-struct to decode the message contents into
 */
void mavlink_msg_management_module_decode(const mavlink_message_t* msg, mavlink_management_module_t* in_packet);

