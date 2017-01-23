// MESSAGE SMARTBAT_TEXT PACKING
#ifndef __MAVLINK_MSG_SMARTBAT_TEXT_H__
#define __MAVLINK_MSG_SMARTBAT_TEXT_H__

#include "sys.h"
#include "mavlink.h"

#define MAVLINK_MSG_ID_SMARTBAT_TEXT 227

typedef struct __mavlink_smartbat_text_t
{
 uint32_t device_id; /*<  ID of device */
 uint16_t voltage_total; /*<  Total voltage (unit mV) */
 uint16_t current_total; /*<  Total current (unit mA)*/
 uint16_t remain_cap; /*<  Battery remaining capacity (unit mAh) */
 uint16_t charge_status; /*<  Battery percent (%) */
 uint16_t cycle_cnt; /*<  Cycle times*/
 uint16_t full_charge_cap; /*<  Full charge capacity (unit mAh) */
 uint16_t design_cap; /*<  Design capacity */
 uint16_t temperature; /*<  Temperature of device (unit K)*/
 uint16_t voltage_1; /*<  voltage 1 (unit mV)*/
 uint16_t voltage_2; /*<  voltage 2 (unit mV)*/
 uint16_t voltage_3; /*<  voltage 3 (unit mV)*/
 uint16_t voltage_4; /*<  voltage 4 (unit mV)*/
 uint16_t voltage_5; /*<  voltage 5 (unit mV)*/
 uint16_t voltage_6; /*<  voltage 6 (unit mV)*/
} mavlink_smartbat_text_t;

#define MAVLINK_MSG_ID_SMARTBAT_TEXT_LEN 32
#define MAVLINK_MSG_ID_227_LEN 32

#define MAVLINK_MSG_ID_SMARTBAT_TEXT_CRC 123
#define MAVLINK_MSG_ID_227_CRC 123



#define MAVLINK_MESSAGE_INFO_SMARTBAT_TEXT { \
	"SMARTBAT_TEXT", \
	15, \
	{  { "device_id", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_smartbat_text_t, device_id) }, \
         { "voltage_total", NULL, MAVLINK_TYPE_UINT16_T, 0, 4, offsetof(mavlink_smartbat_text_t, voltage_total) }, \
         { "current_total", NULL, MAVLINK_TYPE_UINT16_T, 0, 6, offsetof(mavlink_smartbat_text_t, current_total) }, \
         { "remain_cap", NULL, MAVLINK_TYPE_UINT16_T, 0, 8, offsetof(mavlink_smartbat_text_t, remain_cap) }, \
         { "charge_status", NULL, MAVLINK_TYPE_UINT16_T, 0, 10, offsetof(mavlink_smartbat_text_t, charge_status) }, \
         { "cycle_cnt", NULL, MAVLINK_TYPE_UINT16_T, 0, 12, offsetof(mavlink_smartbat_text_t, cycle_cnt) }, \
         { "full_charge_cap", NULL, MAVLINK_TYPE_UINT16_T, 0, 14, offsetof(mavlink_smartbat_text_t, full_charge_cap) }, \
         { "design_cap", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_smartbat_text_t, design_cap) }, \
         { "temperature", NULL, MAVLINK_TYPE_UINT16_T, 0, 18, offsetof(mavlink_smartbat_text_t, temperature) }, \
         { "voltage_1", NULL, MAVLINK_TYPE_UINT16_T, 0, 20, offsetof(mavlink_smartbat_text_t, voltage_1) }, \
         { "voltage_2", NULL, MAVLINK_TYPE_UINT16_T, 0, 22, offsetof(mavlink_smartbat_text_t, voltage_2) }, \
         { "voltage_3", NULL, MAVLINK_TYPE_UINT16_T, 0, 24, offsetof(mavlink_smartbat_text_t, voltage_3) }, \
         { "voltage_4", NULL, MAVLINK_TYPE_UINT16_T, 0, 26, offsetof(mavlink_smartbat_text_t, voltage_4) }, \
         { "voltage_5", NULL, MAVLINK_TYPE_UINT16_T, 0, 28, offsetof(mavlink_smartbat_text_t, voltage_5) }, \
         { "voltage_6", NULL, MAVLINK_TYPE_UINT16_T, 0, 30, offsetof(mavlink_smartbat_text_t, voltage_6) }, \
         } \
}



void mavlink_msg_smartbat_text_send(mavlink_channel_t chan, uint32_t device_id, uint16_t voltage_total, uint16_t current_total, uint16_t remain_cap, uint16_t charge_status, uint16_t cycle_cnt, uint16_t full_charge_cap, uint16_t design_cap, uint16_t temperature, uint16_t voltage_1, uint16_t voltage_2, uint16_t voltage_3, uint16_t voltage_4, uint16_t voltage_5, uint16_t voltage_6);



#endif

