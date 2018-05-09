/*
 *  File name   : mobile_data_handle.c
 *  Created on  :
 *  Author      :
 *  Description :
 *  Version     :
 *  History     : <author>    <time>    <version>   <desc>
 */
#include "sys.h"
#include "zkrt.h"
#include "can.h"
#include "mobileDataHandle.h"
#include "ProtocolZkrtHandle.h"
#include "dev_handle.h"
#include "appprotocol.h"
#include "zkrt_gas.h"
#include "appgas.h"
/////////////////////////////////////////////////////////////////////valible define
msg_handle_st msg_handlest; //消息处理
zkrt_packet_t *main_dji_rev; //point in msg_handle_init

extern u8 which_gas_manu;   //使用哪个厂家的气体探测器
extern void sendToMobile(uint8_t *data, uint8_t len);
////////////////////////////////////////////////////////////////////static function define
static void mobile_data_handle(void);
static void copydataformmobile(const u8* sdata, u8 sdatalen);

/**
 * @brief msg_handle_init
 * @param
 * @param
 * @return
 **/
void msg_handle_init(void) {
	main_dji_rev = &msg_handlest.recvpacket_app;
	memset(&msg_handlest, 0x00, sizeof(msg_handlest));
}
/*
* @brief 将接收到的mobile透传数据进行解析处理
  mobile数据来源是DJI串口透传数据，经过UART1中断接收解析处理过，拷贝到数组
*/
void mobile_data_process(void) {
	//获取从地面站软件的数据
	if (msg_handlest.datalen_recvapp != 0) {
		copydataformmobile(msg_handlest.data_recv_app, msg_handlest.datalen_recvapp);
		mobile_data_handle();
		msg_handlest.datalen_recvapp = 0;
	}
#ifdef CanSend2SubModule_TEST
	if (can_send_debug - TimingDelay > 3000) {
		int index;
		printf("CanSend2SubModule_TEST\n");
		can_send_debug        = TimingDelay;
		msg_handlest.sendpacket_app.start_code  = _START_CODE;
		msg_handlest.sendpacket_app.ver         = _VERSION;
		msg_handlest.sendpacket_app.session_ack = 0;
		msg_handlest.sendpacket_app.padding_enc = 0;
		msg_handlest.sendpacket_app.cmd         = APP_TO_UAV;
		msg_handlest.sendpacket_app.command         = ZK_COMMAND_COMMON;
		msg_handlest.sendpacket_app.length      = 1;
		msg_handlest.sendpacket_app.seq         = 0;
		msg_handlest.sendpacket_app.UAVID[3]    = DEVICE_TYPE_GAS;  //device uavid
		msg_handlest.sendpacket_app.data[0]     = 2;
		msg_handlest.sendpacket_app.end_code    = _END_CODE;
		msg_handlest.datalen_sendapp = zkrt_final_encode(msg_handlest.data_send_app, &msg_handlest.sendpacket_app);
		CAN1_send_message_fun((uint8_t *)msg_handlest.data_send_app,  msg_handlest.datalen_sendapp,
		                      (msg_handlest.sendpacket_app.UAVID[3]));/*通过CAN总线发送数据*/

#ifdef GetDevVersionBSA_TEST
		memcpy(main_dji_rev, &msg_handlest.sendpacket_app, sizeof(zkrt_packet_t));
		msg_handlest.datalen_recvapp = main_dji_rev->length + ZK_FIXED_LEN;
		mobile_data_handle();
		for (index = 0; index < msg_handlest.datalen_sendapp; index++) {
			printf("%x ", msg_handlest.data_send_app[index]);
		}
		printf("\n");
		msg_handlest.datalen_recvapp = 0;
#endif
	}
#endif
}
/***********************************************************************
              中科瑞泰消防无人机函数定义

    函数 名称：接收处理函数
         函数 功能：
    函数 入参：
    函数返回值：
    说     明：

************************************************************************/
void mobile_data_handle(void) {
	int k;
	u8 localmsg_type = 0;
	u8 respondflag   = 0;
	u8 datalen       = main_dji_rev->length;
//check length
	if (msg_handlest.datalen_recvapp != main_dji_rev->length + ZK_FIXED_LEN)
		return;
//check packet format
	if (zkrt_check_packet(main_dji_rev) == false)
		return;
	for (k = 0; k < msg_handlest.datalen_recvapp; k++) {
#ifdef LOG_DEBUG_DATA
		printf("%x ", msg_handlest.data_recv_app[k]);
#endif
	}
#ifdef LOG_DEBUG_DATA
	printf("\n");
#endif
	for (k = 0; k < datalen; k++) {
		ZKRT_LOG(LOG_NOTICE, "main_dji_rev->data[%d]=0x %x\r\n", k, main_dji_rev->data[k]);
	}
	ZKRT_LOG(LOG_NOTICE, "main_dji_rev->crc=0x %x\r\n", main_dji_rev->crc);
	ZKRT_LOG(LOG_NOTICE, "main_dji_rev->end_code=0x %x\r\n", main_dji_rev->end_code);

	//chekc msg affiliation
	if (IS_LOCALMSG(main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE]))
		localmsg_type = 1;

	if ((main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_GAS) && (which_gas_manu == GASTYPE_MAP_BSA_INDEX))
		localmsg_type = 1;

	if (localmsg_type) {
		switch (main_dji_rev->command) {
		case ZK_COMMAND_NORMAL:
			if (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_TEMPERATURE) {
				zkrt_devinfo.temperature_low  =  ((main_dji_rev->data[1]) << 8) + (main_dji_rev->data[0]);
				zkrt_devinfo.temperature_high = ((main_dji_rev->data[3]) << 8) + (main_dji_rev->data[2]);
			} else if (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_MAINBOARD) {
				if (main_dji_rev->data[0] <= MAX_MBDH_NUM)
					mb_self_handle_fun[main_dji_rev->data[0] - 1](main_dji_rev->data + 1);
			}
			break;
		case ZK_COMMAND_SPECIFIED:
			if (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_GAS) {
				if (appgas_message_handle(main_dji_rev, msg_handlest.data_send_app, &msg_handlest.datalen_sendapp) == NEED_RETRUN)
					sendToMobile(msg_handlest.data_send_app, msg_handlest.datalen_sendapp);
			} else if (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE] == DEVICE_TYPE_MAINBOARD)
			{}
			break;
		case ZK_COMMAND_COMMON:
			respondflag = common_data_handle(main_dji_rev, &msg_handlest.sendpacket_app);
			break;
		default: break;
		}

	} else {
		ZKRT_LOG(LOG_NOTICE, "CAN1_send_message_fun\r\n");
		CAN1_send_message_fun(msg_handlest.data_recv_app, msg_handlest.datalen_recvapp,
		                      (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE]));
	}
	if (respondflag) {
		msg_handlest.datalen_sendapp = zkrt_final_encode(msg_handlest.data_send_app, &msg_handlest.sendpacket_app);
		sendToMobile(msg_handlest.data_send_app, msg_handlest.datalen_sendapp);
	}
}
/*
* @brief: mobile数组数据 转存到 zkrt_packet_t格式的全局结构体里
*/
void copydataformmobile(const u8* sdata, u8 sdatalen) {
//copy data to packet
	zkrt_data2_packet(sdata, sdatalen, main_dji_rev);
}
