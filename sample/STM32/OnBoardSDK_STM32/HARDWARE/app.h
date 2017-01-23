#ifndef _APP_H
#define _APP_H



#include "sys.h"											
#include "led.h"											
#include "timer_zkrt.h"										
#include "sbus.h"											
#include "ds18b20.h"									
#include "adc.h"											
#include "iic.h"												
#include "iwatchdog.h"
#include "flash.h"	
#include "usart.h"											
#include "mavlink.h"									
#include "Mavlink_msg_statustext.h" 		
#include "zkrt.h"											
#include "mavlink_msg_smartbat_text.h"	
#include "mavlink_msg_heartbeat.h"		
#include "tempture.h"										
#include "can.h"												
#include "heart.h"											
#include "bat.h"											


uint8_t heart_ack_check(uint8_t device_id, uint8_t value);

void main_zkrt_recv(void);

void main_mavlink_recv(void);


#endif

