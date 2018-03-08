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

#ifdef CanSend2SubModule_TEST
zkrt_packet_t cansdebug;
#endif
/**
 * @brief msg_handle_init
 * @param
 * @param
 * @return
 **/
void msg_handle_init(void)
{
  main_dji_rev = &msg_handlest.recvpacket_app;
  memset(&msg_handlest, 0x00, sizeof(msg_handlest));
}
/*
* @brief 将接收到的mobile透传数据进行解析处理
  mobile数据来源是DJI串口透传数据，经过UART1中断接收解析处理过，拷贝到数组
*/
void mobile_data_process(void)
{
  //获取从地面站软件的数据
  if(msg_handlest.datalen_recvapp!=0)
  {
    copydataformmobile(msg_handlest.data_recv_app, msg_handlest.datalen_recvapp);
    mobile_data_handle();
    msg_handlest.datalen_recvapp =0;
  }
#ifdef CanSend2SubModule_TEST
  if(can_send_debug - TimingDelay > 10000)
  {
    can_send_debug = TimingDelay;
    cansdebug.start_code = 0xEB;
    cansdebug.ver = 1;
    cansdebug.session_ack = 0;
    cansdebug.padding_enc = 0;
    cansdebug.cmd =0x20;
    cansdebug.length = 30;
    cansdebug.seq = 0;
    cansdebug.UAVID[3] = 0x0d;  //device uavid
    cansdebug.data[0] = (u8)(TimingDelay&0xff);
    cansdebug.end_code = 0xbe;
    cansdebug.crc = crc_calculate(((const uint8_t*)(&cansdebug)),47);
    CAN1_send_message_fun((uint8_t *)(&cansdebug), _TOTAL_LEN,
                          (cansdebug.UAVID[3]));/*通过CAN总线发送数据*/
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
//int dataindex;
//short fuyang, hangxiang;
void mobile_data_handle(void)
{
  int k;
  u8 datalen = main_dji_rev->length;
//check length
  if(msg_handlest.datalen_recvapp != main_dji_rev->length+ZK_FIXED_LEN)
    return;
//check packet format
  if(zkrt_check_packet(main_dji_rev)==false)
    return;
//  ZKRT_LOG(LOG_NOTICE, "***************main_dji_revdata_form_mobile************************\r\n");
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->start_code=0x %x\r\n",main_dji_rev->start_code);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->ver=0x %x\r\n",main_dji_rev->ver);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->session_ack=0x %x \r\n",main_dji_rev->session_ack);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->padding_enc=0x %x\r\n",main_dji_rev->cmd);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->cmd=0x %x\r\n",main_dji_rev->ver);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->length=0x %x\r\n",main_dji_rev->length);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->seq=0x %x\r\n",main_dji_rev->seq);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->APPID[0]=0x %x \r\n",main_dji_rev->APPID[0]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->APPID[1]=0x %x \r\n",main_dji_rev->APPID[1]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->APPID[2]=0x %x \r\n",main_dji_rev->APPID[2]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[0]=0x %x \r\n",main_dji_rev->UAVID[0]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[1]=0x %x\r\n",main_dji_rev->UAVID[1]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[2]=0x %x\r\n",main_dji_rev->UAVID[2]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[3]=0x %x\r\n",main_dji_rev->UAVID[3]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[4]=0x %x\r\n",main_dji_rev->UAVID[4]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->UAVID[5]=0x %x\r\n",main_dji_rev->UAVID[5]);
//  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->command=0x %x \r\n",main_dji_rev->command);
  for(k=0; k<msg_handlest.datalen_recvapp; k++)  
  {
    ZKRT_LOG(LOG_DEBUG, "%x ", msg_handlest.data_recv_app[k]);
  }
  ZKRT_LOG(LOG_DEBUG, "\n");
  for(k=0; k<datalen; k++)
  {
    ZKRT_LOG(LOG_NOTICE,"main_dji_rev->data[%d]=0x %x\r\n",k,main_dji_rev->data[k]);
  }
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->crc=0x %x\r\n",main_dji_rev->crc);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev->end_code=0x %x\r\n",main_dji_rev->end_code);

  switch(main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE])
  {
  case DEVICE_TYPE_TEMPERATURE:
    zkrt_devinfo.temperature_low =  ((main_dji_rev->data[1])<<8)+(main_dji_rev->data[0]);
    zkrt_devinfo.temperature_high = ((main_dji_rev->data[3])<<8)+(main_dji_rev->data[2]);
    break;
  case DEVICE_TYPE_MAINBOARD:
    if(main_dji_rev->data[0] <= MAX_MBDH_NUM)
      mb_self_handle_fun[main_dji_rev->data[0]-1](main_dji_rev->data+1);
    break;
  //zkrt_debug
//    case DEVICE_TYPE_MULTICAMERA:
//      printf("[start]   ");
//      for(dataindex=0; dataindex<30; dataindex++)
//      {
//        printf("%x ", main_dji_rev->data[dataindex]);
//      }
//      printf("\r\n");
//      memcpy(&hangxiang, &main_dji_rev->data[0], 2);
//      memcpy(&fuyang, &main_dji_rev->data[2], 2);
//      printf("hangxiang=%d ", hangxiang);
//      printf("fuyang=%d ", fuyang);
//      printf("   [end]\r\n");
//      CAN1_send_message_fun((uint8_t *)(&main_dji_rev), _TOTAL_LEN, (main_dji_rev->UAVID[3]));/*通过CAN总线发送数据*/
//      break;
	case DEVICE_TYPE_GAS:
		if(which_gas_manu ==GASTYPE_MAP_GR_INDEX)
			CAN1_send_message_fun(msg_handlest.data_recv_app, msg_handlest.datalen_recvapp,
											(main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE]));
		else
		{
			if(appgas_message_handle(main_dji_rev, msg_handlest.data_send_app, &msg_handlest.datalen_sendapp)==NEED_RETRUN)
				sendToMobile(msg_handlest.data_send_app, msg_handlest.datalen_sendapp);
		}
		break;
  default:
    ZKRT_LOG(LOG_NOTICE,"CAN1_send_message_fun\r\n");
    CAN1_send_message_fun(msg_handlest.data_recv_app, msg_handlest.datalen_recvapp,
                          (main_dji_rev->UAVID[ZK_DINDEX_DEVTYPE]));/*通过CAN总线发送数据*/
    break;
  }
}
/*
* @brief: mobile数组数据 转存到 zkrt_packet_t格式的全局结构体里
*/
void copydataformmobile(const u8* sdata, u8 sdatalen)
{
//copy data to packet
  zkrt_data2_packet(sdata, sdatalen, main_dji_rev);
}
