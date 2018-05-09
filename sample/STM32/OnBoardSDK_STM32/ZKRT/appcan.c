/**
	******************************************************************************
	* @file    appcan.c
	* @author  ZKRT
	* @version V1.0
	* @date    9-May-2017
	* @brief   can应用数据通信
	*					 + (1) init
	*
	******************************************************************************
	* @attention
	*
	* ...
	*
	******************************************************************************
	*/

/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "appcan.h"
#include "appprotocol.h"
#include "heartBeatHandle.h"
#include "mobileDataHandle.h"
#include "guorui.h"
#include "dev_handle.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
can_msg_handle_st canmsg_hst = {0};
recv_zkrt_packet_handlest canrecv_hdlest[DEVICE_NUMBER] = {0}; //DEVICE BUFFER IS canrecv_hdlest[DEVICE_TYPE-1]
/* Private functions ---------------------------------------------------------*/
static void app_can_recv_handle(void);
extern void sendToMobile(uint8_t *data, uint8_t len);

/**
	* @brief  appcan_init
	* @param  None
	* @retval None
	*/
void appcan_init(void) {
	canmsg_hst.dev_support_en[DEVICE_TYPE_CAMERA - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_GAS - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_THROW - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_IRRADIATE - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_MEGAPHONE - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_3DMODELING - 1] = 1;
	canmsg_hst.dev_support_en[DEVICE_TYPE_MULTICAMERA - 1] = 1;
}
/**
	* @brief
	* @param  None
	* @retval None
	*/
void appcan_prcs(void) {
	//recv handle
	app_can_recv_handle();
}
/**
	* @brief
	* @param
	* @retval
	*/
static u8 zkrt_decode_type(u8 dev_type) {
	uint8_t value;
	recv_zkrt_packet_handlest *recv_handle = &canrecv_hdlest[dev_type];

	while (CAN1_rx_check(dev_type) == 1) {
		value = CAN1_rx_byte(dev_type);
		if (zkrt_decode_char_v2(recv_handle, value) == 1) {
			return 1;
		}
	}
	return 0;
}
/**
	* @brief
	* @param
	* @retval
	*/
static void zkrt_common_data_parse(const u8* data, u8 datalen, u8 dev_type) {
	common_data_plst *cda = (common_data_plst*)data;
	common_hbd_plst *chbda = (common_hbd_plst*)cda->type_data;

	if (dev_type > DEVICE_NUMBER - 1)
		return;

	switch (cda->type_num) {
	case TN_HEARTBEAT:

		if (chbda->hb_flag != TNHB_FLAG)
			break;
		cmon_hb_ptcol_fun[dev_type](chbda->hb_data, datalen - THHB_FIXED_LEN, dev_type);
		ZKRT_LOG(LOG_DEBUG, "dev:%d online\n", dev_type + 1);
		break;
	default:
		break;
	}
}
/**
	* @brief
	* @param  None
	* @retval None
	*/
static void app_can_recv_handle(void) {
	int i;
	zkrt_packet_t *packet = NULL;
	for (i = 0; i < DEVICE_NUMBER; i++) { //循环检测子模块通信状态
		if (canmsg_hst.dev_support_en[i]) {
			if (zkrt_decode_type(i)) {
				packet = &canrecv_hdlest[i].packet;
				//check cmd
				if ((packet->cmd != UAV_TO_APP) && (packet->cmd != SUBDEV_TO_UAV) && (packet->cmd != UAV_TO_APP_SUPER))
					continue;
				//check command
				if (packet->command > ZK_COMMAND_MAX)
					continue;
				//check uavid
				if (packet->UAVID[ZK_DINDEX_DEVTYPE] > DEVICE_NUMBER)
					continue;
				//check message direction and handle
				if ((packet->cmd == UAV_TO_APP) || (packet->cmd == UAV_TO_APP_SUPER)) {
					//send packet to app
					msg_handlest.datalen_sendapp = zkrt_packet2_data(msg_handlest.data_send_app, packet);
					sendToMobile(msg_handlest.data_send_app, msg_handlest.datalen_sendapp);
					ZKRT_LOG(LOG_DEBUG, "len=%d\n", msg_handlest.datalen_sendapp);
#ifdef CanSend2SubModule_TEST
#ifdef GetDevVersion_TEST
					if ((packet->command == ZK_COMMAND_COMMON) && (packet->data[0] == 2)) {
						common_get_devinfo_plst *tempdevinfo = (common_get_devinfo_plst *)&packet->data[1];
						printf("=======\n");
						printf("=======\n");
						printf("=======\n");
						printf("length: %d, ", packet->length);
						printf("status: %d, ", tempdevinfo->status);
						printf("model: %s, ", tempdevinfo->model);
						printf("sw: %s, ", tempdevinfo->sw_version);
						printf("hw: %s \n", tempdevinfo->hw_version);
						printf("=======\n");
						printf("=======\n");
						printf("=======\n");
					}
#endif
#endif
				} else if (packet->cmd == SUBDEV_TO_UAV) {
					//check command
					if (packet->command == ZK_COMMAND_COMMON) {
						zkrt_common_data_parse(packet->data, packet->length, i);
					} else
						continue;
				} else
					continue;
			}
		}
	}
}
/*-------------------------------------------------------------------------------------*/  //protocol common hb data fun
static char none_chbptf(const void *data, u8 len, u8 type);
static char none_chbptf(const void *data, u8 len, u8 type);
static char none_chbptf(const void *data, u8 len, u8 type);
static char none_chbptf(const void *data, u8 len, u8 type);
static char signlecamera_chbptf(const void *data, u8 len, u8 type);
static char gas_chbptf(const void *data, u8 len, u8 type);
static char throw_chbptf(const void *data, u8 len, u8 type);
static char irradiate_chbptf(const void *data, u8 len, u8 type);
static char megaphone_chbptf(const void *data, u8 len, u8 type);
static char threedmodeling_chbptf(const void *data, u8 len, u8 type);
static char multicamera_chbptf(const void *data, u8 len, u8 type);
const COMMON_HB_PTOCOL_FUN cmon_hb_ptcol_fun[] = { //INDEX IS DEVICE TYPE -1
	none_chbptf,
	none_chbptf,
	none_chbptf,
	signlecamera_chbptf,
	gas_chbptf,
	throw_chbptf,
	none_chbptf,
	none_chbptf,
	irradiate_chbptf,
	megaphone_chbptf,
	none_chbptf,
	threedmodeling_chbptf,
	multicamera_chbptf,
};

/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char none_chbptf(const void *data, u8 len, u8 type) {
	return 0;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char signlecamera_chbptf(const void *data, u8 len, u8 type) {
	if (len != 0)
		return 0;

	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.siglecamera = DV_ONLINE;
	//timer set , when timeout, dev is offline
	camera_recv_flag = TimingDelay;

	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char gas_chbptf(const void *data, u8 len, u8 type) {
	gas_v3_3_hbccplst* hbd = (gas_v3_3_hbccplst*)data;

	if (len != sizeof(gas_v3_3_hbccplst))
		return 0;
	//value set
	guorui_gas_info = *hbd;
	gr_handle.gas_update_flag = 1;
	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.gas = DV_ONLINE;
	gr_handle.gas_online_flag = 1;
	//timer set , when timeout, dev is offline
	posion_recv_flag = TimingDelay;
	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char throw_chbptf(const void *data, u8 len, u8 type) {
	throw_hbccplst* hbd = (throw_hbccplst*)data;

	if (len != sizeof(throw_hbccplst))
		return 0;

	//value set
	zkrt_devinfo.devself->dev.feedback_s.valuebit.throw1_s = (hbd->v1) & 0x01;
	zkrt_devinfo.devself->dev.feedback_s.valuebit.throw2_s = (hbd->v2) & 0x01;
	zkrt_devinfo.devself->dev.feedback_s.valuebit.throw3_s = (hbd->v3) & 0x01;
	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.throwd = DV_ONLINE;
	//timer set , when timeout, dev is offline
	throw_recv_flag = TimingDelay;

	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char irradiate_chbptf(const void *data, u8 len, u8 type) {
	irradiate_hbccplst* hbd = (irradiate_hbccplst*)data;

	if (len != sizeof(irradiate_hbccplst))
		return 0;

	//value set
	zkrt_devinfo.devself->dev.feedback_s.valuebit.irradiate_s = (hbd->value) & 0x01;
	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.irradiate = DV_ONLINE;
	//timer set , when timeout, dev is offline
	irradiate_recv_flag = TimingDelay;

	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char megaphone_chbptf(const void *data, u8 len, u8 type) {
	if (len != 0)
		return 0;
	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.megaphone = DV_ONLINE;
	//timer set , when timeout, dev is offline
	phone_recv_flag = TimingDelay;

	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char threedmodeling_chbptf(const void *data, u8 len, u8 type) {
	threemodel_hbccplst* hbd = (threemodel_hbccplst*)data;
	if (len != sizeof(threemodel_hbccplst))
		return 0;

	//value set
	zkrt_devinfo.devself->dev.feedback_s.valuebit.threemodeling_photo_s = (hbd->value) & 0x01;
	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.threemodelling = DV_ONLINE;
	//timer set , when timeout, dev is offline
	threemodeling_recv_flag = TimingDelay;

	return 1;
}
/**
	* @brief
	* @param
	* @note
	* @retval
	*/
static char multicamera_chbptf(const void *data, u8 len, u8 type) {
	if (len != 0)
		return 0;

	//online value
	zkrt_devinfo.devself->dev.dev_online_s.valuebit.multicamera = DV_ONLINE;
	//timer set , when timeout, dev is offline
	multicamera_recv_flag = TimingDelay;

	return 1;
}
