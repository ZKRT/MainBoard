#ifndef _HEART_H
#define _HEART_H

#include "sys.h"

//������ڴ洢30���ֳ������������ݣ����ͺ�������msg_msartbat_text��ID��227
extern uint8_t msg_smartbat_buffer[30];

//���������ģ�鴫������ZKRT����ֺ����ݱ����ڴ˻�������
extern uint8_t posion_buffer[16];

extern uint8_t mavlink_type_flag;

void turn_off_5V_power(void);
void turn_on_5V_power(void);
void zkrt_read_heart_vol_check(void);
void zkrt_read_heart_vol(void);
void zkrt_read_heart_posion_check(void);
void zkrt_read_heart_posion(void);
void zkrt_read_heart_tempture_check_send(uint8_t num);
void zkrt_read_heart_tempture_check(void);
void zkrt_read_heart_tempture(void);
void zkrt_read_heart_ack(void);

#endif

