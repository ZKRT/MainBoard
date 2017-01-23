#ifndef _HEART_H
#define _HEART_H

#include "sys.h"

//这个用于存储30个字长的心跳包数据，发送函数调用msg_msartbat_text，ID号227
extern uint8_t msg_smartbat_buffer[30];

//这个将毒气模块传上来的ZKRT包拆分后，数据保存在此缓冲区里
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

