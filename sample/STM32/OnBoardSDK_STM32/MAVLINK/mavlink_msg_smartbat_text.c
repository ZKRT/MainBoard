#include "mavlink_msg_smartbat_text.h"


/**
 * @brief Pack a smartbat_text message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param device_id  ID of device 
 * @param voltage_total  Total voltage (unit mV) 
 * @param current_total  Total current (unit mA)
 * @param remain_cap  Battery remaining capacity (unit mAh) 
 * @param charge_status  Battery percent (%) 
 * @param cycle_cnt  Cycle times
 * @param full_charge_cap  Full charge capacity (unit mAh) 
 * @param design_cap  Design capacity 
 * @param temperature  Temperature of device (unit K)
 * @param voltage_1  voltage 1 (unit mV)
 * @param voltage_2  voltage 2 (unit mV)
 * @param voltage_3  voltage 3 (unit mV)
 * @param voltage_4  voltage 4 (unit mV)
 * @param voltage_5  voltage 5 (unit mV)
 * @param voltage_6  voltage 6 (unit mV)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
uint16_t mavlink_msg_smartbat_text_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t device_id, uint16_t voltage_total, uint16_t current_total, uint16_t remain_cap, uint16_t charge_status, uint16_t cycle_cnt, uint16_t full_charge_cap, uint16_t design_cap, uint16_t temperature, uint16_t voltage_1, uint16_t voltage_2, uint16_t voltage_3, uint16_t voltage_4, uint16_t voltage_5, uint16_t voltage_6)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN];
	_mav_put_uint32_t(buf, 0, device_id);
	_mav_put_uint16_t(buf, 4, voltage_total);
	_mav_put_uint16_t(buf, 6, current_total);
	_mav_put_uint16_t(buf, 8, remain_cap);
	_mav_put_uint16_t(buf, 10, charge_status);
	_mav_put_uint16_t(buf, 12, cycle_cnt);
	_mav_put_uint16_t(buf, 14, full_charge_cap);
	_mav_put_uint16_t(buf, 16, design_cap);
	_mav_put_uint16_t(buf, 18, temperature);
	_mav_put_uint16_t(buf, 20, voltage_1);
	_mav_put_uint16_t(buf, 22, voltage_2);
	_mav_put_uint16_t(buf, 24, voltage_3);
	_mav_put_uint16_t(buf, 26, voltage_4);
	_mav_put_uint16_t(buf, 28, voltage_5);
	_mav_put_uint16_t(buf, 30, voltage_6);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#else
	mavlink_smartbat_text_t packet;
	packet.device_id = device_id;
	packet.voltage_total = voltage_total;
	packet.current_total = current_total;
	packet.remain_cap = remain_cap;
	packet.charge_status = charge_status;
	packet.cycle_cnt = cycle_cnt;
	packet.full_charge_cap = full_charge_cap;
	packet.design_cap = design_cap;
	packet.temperature = temperature;
	packet.voltage_1 = voltage_1;
	packet.voltage_2 = voltage_2;
	packet.voltage_3 = voltage_3;
	packet.voltage_4 = voltage_4;
	packet.voltage_5 = voltage_5;
	packet.voltage_6 = voltage_6;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SMARTBAT_TEXT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
}

/**
 * @brief Pack a smartbat_text message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param device_id  ID of device 
 * @param voltage_total  Total voltage (unit mV) 
 * @param current_total  Total current (unit mA)
 * @param remain_cap  Battery remaining capacity (unit mAh) 
 * @param charge_status  Battery percent (%) 
 * @param cycle_cnt  Cycle times
 * @param full_charge_cap  Full charge capacity (unit mAh) 
 * @param design_cap  Design capacity 
 * @param temperature  Temperature of device (unit K)
 * @param voltage_1  voltage 1 (unit mV)
 * @param voltage_2  voltage 2 (unit mV)
 * @param voltage_3  voltage 3 (unit mV)
 * @param voltage_4  voltage 4 (unit mV)
 * @param voltage_5  voltage 5 (unit mV)
 * @param voltage_6  voltage 6 (unit mV)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
uint16_t mavlink_msg_smartbat_text_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t device_id,uint16_t voltage_total,uint16_t current_total,uint16_t remain_cap,uint16_t charge_status,uint16_t cycle_cnt,uint16_t full_charge_cap,uint16_t design_cap,uint16_t temperature,uint16_t voltage_1,uint16_t voltage_2,uint16_t voltage_3,uint16_t voltage_4,uint16_t voltage_5,uint16_t voltage_6)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN];
	_mav_put_uint32_t(buf, 0, device_id);
	_mav_put_uint16_t(buf, 4, voltage_total);
	_mav_put_uint16_t(buf, 6, current_total);
	_mav_put_uint16_t(buf, 8, remain_cap);
	_mav_put_uint16_t(buf, 10, charge_status);
	_mav_put_uint16_t(buf, 12, cycle_cnt);
	_mav_put_uint16_t(buf, 14, full_charge_cap);
	_mav_put_uint16_t(buf, 16, design_cap);
	_mav_put_uint16_t(buf, 18, temperature);
	_mav_put_uint16_t(buf, 20, voltage_1);
	_mav_put_uint16_t(buf, 22, voltage_2);
	_mav_put_uint16_t(buf, 24, voltage_3);
	_mav_put_uint16_t(buf, 26, voltage_4);
	_mav_put_uint16_t(buf, 28, voltage_5);
	_mav_put_uint16_t(buf, 30, voltage_6);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#else
	mavlink_smartbat_text_t packet;
	packet.device_id = device_id;
	packet.voltage_total = voltage_total;
	packet.current_total = current_total;
	packet.remain_cap = remain_cap;
	packet.charge_status = charge_status;
	packet.cycle_cnt = cycle_cnt;
	packet.full_charge_cap = full_charge_cap;
	packet.design_cap = design_cap;
	packet.temperature = temperature;
	packet.voltage_1 = voltage_1;
	packet.voltage_2 = voltage_2;
	packet.voltage_3 = voltage_3;
	packet.voltage_4 = voltage_4;
	packet.voltage_5 = voltage_5;
	packet.voltage_6 = voltage_6;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_SMARTBAT_TEXT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
}

/**
 * @brief Encode a smartbat_text struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param smartbat_text C-struct to read the message contents from
 */
uint16_t mavlink_msg_smartbat_text_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_smartbat_text_t* smartbat_text)
{
	return mavlink_msg_smartbat_text_pack(system_id, component_id, msg, smartbat_text->device_id, smartbat_text->voltage_total, smartbat_text->current_total, smartbat_text->remain_cap, smartbat_text->charge_status, smartbat_text->cycle_cnt, smartbat_text->full_charge_cap, smartbat_text->design_cap, smartbat_text->temperature, smartbat_text->voltage_1, smartbat_text->voltage_2, smartbat_text->voltage_3, smartbat_text->voltage_4, smartbat_text->voltage_5, smartbat_text->voltage_6);
}

/**
 * @brief Encode a smartbat_text struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param smartbat_text C-struct to read the message contents from
 */
uint16_t mavlink_msg_smartbat_text_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_smartbat_text_t* smartbat_text)
{
	return mavlink_msg_smartbat_text_pack_chan(system_id, component_id, chan, msg, smartbat_text->device_id, smartbat_text->voltage_total, smartbat_text->current_total, smartbat_text->remain_cap, smartbat_text->charge_status, smartbat_text->cycle_cnt, smartbat_text->full_charge_cap, smartbat_text->design_cap, smartbat_text->temperature, smartbat_text->voltage_1, smartbat_text->voltage_2, smartbat_text->voltage_3, smartbat_text->voltage_4, smartbat_text->voltage_5, smartbat_text->voltage_6);
}

/**
 * @brief Send a smartbat_text message
 * @param chan MAVLink channel to send the message
 *
 * @param device_id  ID of device 
 * @param voltage_total  Total voltage (unit mV) 
 * @param current_total  Total current (unit mA)
 * @param remain_cap  Battery remaining capacity (unit mAh) 
 * @param charge_status  Battery percent (%) 
 * @param cycle_cnt  Cycle times
 * @param full_charge_cap  Full charge capacity (unit mAh) 
 * @param design_cap  Design capacity 
 * @param temperature  Temperature of device (unit K)
 * @param voltage_1  voltage 1 (unit mV)
 * @param voltage_2  voltage 2 (unit mV)
 * @param voltage_3  voltage 3 (unit mV)
 * @param voltage_4  voltage 4 (unit mV)
 * @param voltage_5  voltage 5 (unit mV)
 * @param voltage_6  voltage 6 (unit mV)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

void mavlink_msg_smartbat_text_send(mavlink_channel_t chan, uint32_t device_id, uint16_t voltage_total, uint16_t current_total, uint16_t remain_cap, uint16_t charge_status, uint16_t cycle_cnt, uint16_t full_charge_cap, uint16_t design_cap, uint16_t temperature, uint16_t voltage_1, uint16_t voltage_2, uint16_t voltage_3, uint16_t voltage_4, uint16_t voltage_5, uint16_t voltage_6)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN];
	_mav_put_uint32_t(buf, 0, device_id);
	_mav_put_uint16_t(buf, 4, voltage_total);
	_mav_put_uint16_t(buf, 6, current_total);
	_mav_put_uint16_t(buf, 8, remain_cap);
	_mav_put_uint16_t(buf, 10, charge_status);
	_mav_put_uint16_t(buf, 12, cycle_cnt);
	_mav_put_uint16_t(buf, 14, full_charge_cap);
	_mav_put_uint16_t(buf, 16, design_cap);
	_mav_put_uint16_t(buf, 18, temperature);
	_mav_put_uint16_t(buf, 20, voltage_1);
	_mav_put_uint16_t(buf, 22, voltage_2);
	_mav_put_uint16_t(buf, 24, voltage_3);
	_mav_put_uint16_t(buf, 26, voltage_4);
	_mav_put_uint16_t(buf, 28, voltage_5);
	_mav_put_uint16_t(buf, 30, voltage_6);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
#else
	mavlink_smartbat_text_t packet;
	packet.device_id = device_id;
	packet.voltage_total = voltage_total;
	packet.current_total = current_total;
	packet.remain_cap = remain_cap;
	packet.charge_status = charge_status;
	packet.cycle_cnt = cycle_cnt;
	packet.full_charge_cap = full_charge_cap;
	packet.design_cap = design_cap;
	packet.temperature = temperature;
	packet.voltage_1 = voltage_1;
	packet.voltage_2 = voltage_2;
	packet.voltage_3 = voltage_3;
	packet.voltage_4 = voltage_4;
	packet.voltage_5 = voltage_5;
	packet.voltage_6 = voltage_6;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, (const char *)&packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, (const char *)&packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
void mavlink_msg_smartbat_text_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t device_id, uint16_t voltage_total, uint16_t current_total, uint16_t remain_cap, uint16_t charge_status, uint16_t cycle_cnt, uint16_t full_charge_cap, uint16_t design_cap, uint16_t temperature, uint16_t voltage_1, uint16_t voltage_2, uint16_t voltage_3, uint16_t voltage_4, uint16_t voltage_5, uint16_t voltage_6)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, device_id);
	_mav_put_uint16_t(buf, 4, voltage_total);
	_mav_put_uint16_t(buf, 6, current_total);
	_mav_put_uint16_t(buf, 8, remain_cap);
	_mav_put_uint16_t(buf, 10, charge_status);
	_mav_put_uint16_t(buf, 12, cycle_cnt);
	_mav_put_uint16_t(buf, 14, full_charge_cap);
	_mav_put_uint16_t(buf, 16, design_cap);
	_mav_put_uint16_t(buf, 18, temperature);
	_mav_put_uint16_t(buf, 20, voltage_1);
	_mav_put_uint16_t(buf, 22, voltage_2);
	_mav_put_uint16_t(buf, 24, voltage_3);
	_mav_put_uint16_t(buf, 26, voltage_4);
	_mav_put_uint16_t(buf, 28, voltage_5);
	_mav_put_uint16_t(buf, 30, voltage_6);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, buf, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
#else
	mavlink_smartbat_text_t *packet = (mavlink_smartbat_text_t *)msgbuf;
	packet->device_id = device_id;
	packet->voltage_total = voltage_total;
	packet->current_total = current_total;
	packet->remain_cap = remain_cap;
	packet->charge_status = charge_status;
	packet->cycle_cnt = cycle_cnt;
	packet->full_charge_cap = full_charge_cap;
	packet->design_cap = design_cap;
	packet->temperature = temperature;
	packet->voltage_1 = voltage_1;
	packet->voltage_2 = voltage_2;
	packet->voltage_3 = voltage_3;
	packet->voltage_4 = voltage_4;
	packet->voltage_5 = voltage_5;
	packet->voltage_6 = voltage_6;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, (const char *)packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN, MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SMARTBAT_TEXT, (const char *)packet, MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN);
#endif
#endif
}
#endif

#endif

