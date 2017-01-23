#ifndef __SBUS_H
#define	__SBUS_H

#include "sys.h"

typedef struct _zkrt_sbus
{
	uint8_t   startbyte;
	uint8_t   data[22];
	uint8_t   flags;
	uint8_t   endbyte;
} zkrt_sbus;

extern uint16_t tx_channel_in [16];
extern uint16_t tx_channel_out[16];
extern zkrt_sbus tx_sbus;

extern uint16_t rx_channel_in [16];
extern uint16_t rx_channel_out[16];
extern zkrt_sbus rx_sbus;

uint8_t sbus_send(void);
uint8_t sbus_recv(void);
#endif 

