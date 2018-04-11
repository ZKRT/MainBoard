#ifndef __MOBILE_DATA_HANDLE_H
#define __MOBILE_DATA_HANDLE_H

#include "zkrt.h"

//message protocol handle flag and buffer struct
typedef struct
{
    zkrt_packet_t recvpacket_app;  //with app
//	zkrt_packet_t sendpacket_app;  //with app
    u8 data_recv_app[ZK_MAX_LEN];
    u8 data_send_app[ZK_MAX_LEN];
    u8 datalen_recvapp;
    u8 datalen_sendapp;
//u8 data[ZK_MAX_LEN];
} msg_handle_st;

extern msg_handle_st msg_handlest;

void msg_handle_init(void);
void mobile_data_process(void);
#endif

