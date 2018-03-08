/**
  ******************************************************************************
  * @file    baoshian.c 
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief   
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
#include "baoshian.h"
#include "osusart.h"
#include "osqtmr.h"
#include "dev_handle.h"
#include "guorui.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static u8 arry_xor_sequence(const u8 *data, u16 datalen);
static char baoshian_recv_handle();
static void bsa_getchdata_scmd_pack(u8 ch);
static void bsa_getchcfg_scmd_pack(u8 ch);
static void bsa_timer_task(void);
static void baoshian_sendqueue_handle(void);
static void baoshian_period_send_get_ch_data(void);
////baoshian send cmd fixed data
const u8 bsa_connec_scmd[] =     {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x00, 0x00, 0x01, 0x00, 0x55};
const u8 bsa_disconnec_scmd[] =  {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x01, 0x00, 0x00, 0x00, 0x55};
const u8 bsa_getdevinfo_scmd[] = {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x02, 0x00, 0x01, 0x02, 0x55};
u8 bsa_getchcfg_scmd[] =         {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x03, 0x00, 0xff, 0xff, 0x55}; //change by ch num, and check code must recalcultae
u8 bsa_getchdata_scmd[] =        {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x05, 0x00, 0xff, 0xff, 0x55}; //change by ch num, and check code must recalcultae
const u8 bsa_volumeopen_scmd[] = {0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x09, 0x00, 0x01, 0x09, 0x55};
const u8 bsa_volumeclose_scmd[] ={0xaa, 0x00, 0x07, 0xAD, 0x11, 0x10, 0x09, 0x00, 0x00, 0xa5, 0x55};
////send handle queue
#define BSA_SENDQUEUE_PERIOD   5 //50ms
#define BSA_SENDQUEUE_NUM      10
#define BSA_SENDQUEUE_BUF_SIZE 20
u8 baoshian_sendqueue[BSA_SENDQUEUE_NUM][BSA_SENDQUEUE_BUF_SIZE];  //10 send buffer, every buffer max size is 20
u8 baoshian_sendqueue_len[BSA_SENDQUEUE_NUM];               
u8 bsa_sendqueue_cnt;
u8 bsa_sendqueue_head;
u8 bsa_sendqueue_index;
volatile u8 bsa_sendqueue_timestartcnt;
volatile u8 bsa_senqueue_active;
////recv handle buffer and pointer
u8 baoshian_recv_buff[BAOSHIAN_MAX_LEN];
baoshian_header_ptcl *bsa_recv_header;
baoshian_tailer_ptcl *bsa_recv_tailer;
u8 *bsa_recv_data;
////baoshian dev and channel gas info of global
bsa_devinfo_st bsa_dev_info;
bsa_gas_chinfo_st bsa_chgas_info[BSA_GAS_NUM];
////baoshian process handle of global
struct bsa_prcs_handles bsa_prcs_handle;
/**
  * @brief  
  * @param  None
  * @retval None
  */
void baoshian_init(void)
{
	////para init
	//queue
	bsa_sendqueue_cnt =0;
	bsa_sendqueue_head=0;
	bsa_sendqueue_index =0;	
	bsa_senqueue_active =1;
	bsa_sendqueue_timestartcnt = BSA_SENDQUEUE_PERIOD;
	
	//handle parm
	bsa_prcs_handle.gas_online_flag = 0;
	bsa_prcs_handle.gas_update_flag = 0;
	bsa_prcs_handle.get_channel_info_period = 0;
	//info parm
	bsa_dev_info.bsa_fixed_dev.channel_num = 0;//BSA_GAS_NUM; 
	
	////
	t_systmr_insertQuickTask(bsa_timer_task, 10, OSTMR_PERIODIC);   
	baoshian_send(bsa_connec_scmd, sizeof(bsa_connec_scmd), 1);
	baoshian_send(bsa_getdevinfo_scmd, sizeof(bsa_getdevinfo_scmd), 1);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void baoshian_prcs(void)
{
	baoshian_recv_handle();       //fixed invoke
	baoshian_sendqueue_handle();  //fixed invoke
	baoshian_period_send_get_ch_data();  //app
	baoshian_reset();
}
/**
  * @brief  当系统不存在气体设备，没隔10s查询一次设备是否有连接
  * @param  None
  * @retval None
  */
void baoshian_reset(void)
{
	if((bsa_prcs_handle.reconnect_period >1000)&&(bsa_prcs_handle.gas_online_flag==0))
	{
		bsa_prcs_handle.reconnect_period = 0;
		baoshian_send(bsa_connec_scmd, sizeof(bsa_connec_scmd), 1);
		baoshian_send(bsa_getdevinfo_scmd, sizeof(bsa_getdevinfo_scmd), 1);
	}
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static char baoshian_recv_handle(void)
{
	u16 rlen;
	u8 *rbuff = baoshian_recv_buff;
	
	//recv uart data
	if(t_osscomm_ReceiveMessage(rbuff, &rlen, USART2)!= SCOMM_RET_OK)
	{
		return 0;
	}		
	//point header
	bsa_recv_header = (baoshian_header_ptcl*)baoshian_recv_buff;
	//point data
	bsa_recv_data = baoshian_recv_buff+sizeof(baoshian_header_ptcl);
	//check length
	bsa_recv_header->length = (((bsa_recv_header->length)&0xff)<<8|((bsa_recv_header->length)&0xff00)>>8);  //zkrt_notice: special handle for baoshian 
	if(bsa_recv_header->length+BAOSHIAN_LENGTH_DEFINE_INDEX != rlen)  //zkrt_notice: may be have bug, if rbuff received block
		return 0;
    //point tailer
	bsa_recv_tailer = (baoshian_tailer_ptcl*)(baoshian_recv_buff+(rlen-sizeof(baoshian_tailer_ptcl)));
	
////check format	
	//check start code
	if(bsa_recv_header->start_code !=BSA_STARTCODE)
		return 0;	
	//check end code
	if(bsa_recv_tailer->end_code !=BSA_ENDCODE)
		return 0;
	//check type id
	if(bsa_recv_header->type_id !=BSA_GAS2HOST_TYPEID)
		return 0;
	//check header check code
	if(arry_xor_sequence((u8*)rbuff, BSA_HEADERCHECK_LEN)!= bsa_recv_header->head_check)
		return 0;
	//check packet check code
    if(arry_xor_sequence((u8*)rbuff+BAOSHIAN_PACKECHECK_INDEX_INDATA, rlen-BSA_LEN_NOTNEED_PACKETCHECK)!= bsa_recv_tailer->packet_check)
		return 0;
	if(bsa_recv_header->fun_id > max_bsafid)
		return 0;	
	
	switch(bsa_recv_header->fun_id)
	{					
		case connec_bsafid:
			ZKRT_LOG(LOG_DEBUG, "connected bsa\n");
		  if(gr_handle.gas_online_flag ==0)
			{
				zkrt_devinfo.devself->dev.dev_online_s.valuebit.gas = DV_ONLINE;
				posion_recv_flag = TimingDelay;	//timer set , when timeout, dev is offline		
			}
			break;
		case disconnec_bsafid:
			break;
		case readdevinfo_bsafid:
			if((sizeof(r_get_devinfo_bsapl)+BAOSHIAN_LEN_NOT_VALID_DATA_LEN) == bsa_recv_header->length)
			{//valid
				memcpy((void*)&bsa_dev_info.bsa_fixed_dev, bsa_recv_data, sizeof(r_get_devinfo_bsapl));
				if(bsa_dev_info.bsa_fixed_dev.channel_num >BSA_GAS_NUM) 
					bsa_dev_info.bsa_fixed_dev.channel_num = BSA_GAS_NUM;	//zkrt_notice: defend array outsteam
//				printf("readdevinfo_bsafid bsa\n");
//				printf("version[%d], channel num[%d], model[%s]\n", bsa_dev_info.bsa_fixed_dev.version, bsa_dev_info.bsa_fixed_dev.channel_num, bsa_dev_info.bsa_fixed_dev.model);				
			}
			break;
		case readchgasdata_bsafid:
			if((sizeof(r_get_chgasvalue_bsa_pl)+BAOSHIAN_LEN_NOT_VALID_DATA_LEN) == bsa_recv_header->length)
			{//valid
				if(bsa_recv_data[0] < bsa_dev_info.bsa_fixed_dev.channel_num)
				{
//					float value;
					memcpy((void*)&bsa_chgas_info[bsa_recv_data[0]], bsa_recv_data, sizeof(r_get_chgasvalue_bsa_pl));
					setBit(bsa_dev_info.ch_status, bsa_recv_data[0]); //set gas channel status
					//zkrt_notice: special handle  //机体毒气模块比挂载的毒气模块优先级低
					if(gr_handle.gas_online_flag ==0)
					{
						bsa_prcs_handle.gas_update_flag = 1; //set gas update flag 
						bsa_prcs_handle.gas_online_flag = 1; //set baoshian gas online flag
						zkrt_devinfo.devself->dev.dev_online_s.valuebit.gas = DV_ONLINE;
						posion_recv_flag = TimingDelay;	//timer set , when timeout, dev is offline
					}
//					memcpy((void*)&value, (void*)&bsa_chgas_info[bsa_recv_data[0]].gas_value, 4);
//					printf("readchgasdata_bsafid bsa, ch%d, value:%f, type[%d], unit[%d], decimal[%d], ", bsa_recv_data[0], value, 
//					bsa_chgas_info[bsa_recv_data[0]].sensor_type, bsa_chgas_info[bsa_recv_data[0]].unit_type, bsa_chgas_info[bsa_recv_data[0]].decimal_accuracy);
//					memcpy((void*)&value, (void*)&bsa_chgas_info[bsa_recv_data[0]].measure_range, 4);
//					printf("range[%f]\n", value);
				}
			}
			break;
		case setmutestate_bsafid:
			break;
		default:break;
	}
	return 1;
}
/**
  * @brief  数组顺序异或
  * @param  None
  * @retval None
  */
static u8 arry_xor_sequence(const u8 *data, u16 datalen)
{
	int i;
	u8 checkcode =0;
	for(i=0; i< datalen; i++)
	{
		checkcode = data[i]^checkcode;
	}
	return checkcode;
}
/**
  * @brief  packet to sequence data
  * @param  None
  * @retval None
  */
static u16 baoshian_packet2data(u8 *data, struct baoshian_ptcl *packet)
{
//	memcpy(data, packet, BAOSHIAN_HEADER_LEN);
//	memcpy(data+BAOSHIAN_HEADER_LEN, packet->data, packet->length-BAOSHIAN_LEN_NO_VALID_DATA_LEN);
//	memcpy(data+BAOSHIAN_HEADER_LEN+packet->length-BAOSHIAN_LEN_NO_VALID_DATA_LEN, &packet->packet_check, BAOSHIAN_TAIL_LEN);
//	return (packet->length-BAOSHIAN_LEN_NO_VALID_DATA_LEN+BAOSHIAN_FIXED_LEN);
}
/**
  * @brief  packet to sequence data
  * @param  None
  * @retval None
  */
static u16 baoshian_data2packet(const u8 *data, struct baoshian_ptcl *packet)
{
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void bsa_timer_task(void)
{
	if(bsa_sendqueue_timestartcnt)
	{
		if(--bsa_sendqueue_timestartcnt == 0)
			bsa_senqueue_active = 1;
	}
	bsa_prcs_handle.get_channel_info_period++;
	bsa_prcs_handle.reconnect_period++;
}
/**
* @brief  
* @param  flag: 0-direct send, 1-queue send
* @retval 0-ok, 1-fail
*/
char baoshian_send(const u8 *data, u8 datalen, u8 flag)
{
	if(flag ==0)
		t_osscomm_sendMessage((u8*)data, datalen, USART2);
	else
	{
		if(bsa_sendqueue_cnt >=BSA_SENDQUEUE_NUM)
			return 1;
		if(datalen >BSA_SENDQUEUE_BUF_SIZE)
			return 1;
		bsa_sendqueue_cnt++;
		memcpy(baoshian_sendqueue[bsa_sendqueue_head], data, datalen);
		baoshian_sendqueue_len[bsa_sendqueue_head] = datalen;
		bsa_sendqueue_head = (bsa_sendqueue_head+1)>=BSA_SENDQUEUE_NUM?0:(bsa_sendqueue_head+1); //head point next
	}
	return 0;
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void baoshian_sendqueue_handle(void)
{
	if(bsa_sendqueue_cnt) //have message
	{
		if(bsa_senqueue_active)
		{
			bsa_sendqueue_timestartcnt = BSA_SENDQUEUE_PERIOD;
			bsa_senqueue_active = 0;
			bsa_sendqueue_cnt--;
			baoshian_send(baoshian_sendqueue[bsa_sendqueue_index], baoshian_sendqueue_len[bsa_sendqueue_index], 0);
			bsa_sendqueue_index = (bsa_sendqueue_index+1)>=BSA_SENDQUEUE_NUM?0:(bsa_sendqueue_index+1); //index point next
		}
	}
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void baoshian_period_send_get_ch_data(void)
{
	int i;
	if((bsa_prcs_handle.get_channel_info_period >BSA_GET_ALLINFO_PERIOD)&&((BSA_SENDQUEUE_NUM-bsa_sendqueue_cnt)>=bsa_dev_info.bsa_fixed_dev.channel_num))
	{
		bsa_prcs_handle.get_channel_info_period = 0;
		for(i=0; i<bsa_dev_info.bsa_fixed_dev.channel_num; i++)
		{
			bsa_getchdata_scmd_pack(i);
			baoshian_send(bsa_getchdata_scmd, sizeof(bsa_getchdata_scmd), 1);	
		}
	}
}
////////////////////////////////////////////////////////////////////////////send cmd pack
/**
* @brief  
* @param  None
* @retval None
*/
static void bsa_getchdata_scmd_pack(u8 ch)
{
	bsa_getchdata_scmd[8] = ch;
	bsa_getchdata_scmd[9] = arry_xor_sequence(bsa_getchdata_scmd+BAOSHIAN_PACKECHECK_INDEX_INDATA, sizeof(bsa_getchdata_scmd)-BAOSHIAN_LEN_NOT_VALID_DATA_LEN);
}
/**
* @brief  
* @param  None
* @retval None
*/
static void bsa_getchcfg_scmd_pack(u8 ch)
{
	bsa_getchcfg_scmd[8] = ch;
	bsa_getchcfg_scmd[9] = arry_xor_sequence(bsa_getchcfg_scmd+BAOSHIAN_PACKECHECK_INDEX_INDATA, sizeof(bsa_getchcfg_scmd)-BAOSHIAN_LEN_NOT_VALID_DATA_LEN);
}
