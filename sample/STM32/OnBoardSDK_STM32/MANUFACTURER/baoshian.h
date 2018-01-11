/**
  ******************************************************************************
  * @file    baoshian.h 
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief   
	*					 + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BAOSHIAN_H
#define __BAOSHIAN_H 
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "hw_usart.h"
/* Exported macro ------------------------------------------------------------*/

//length
//#define BAOSHIAN_HEADER_LEN                 8
//#define BAOSHIAN_TAIL_LEN                   2
#define BAOSHIAN_FIXED_LEN                  10
#define BAOSHIAN_LEN_NOT_VALID_DATA_LEN     6
#define BSA_LEN_NOTNEED_PACKETCHECK         6
#define BAOSHIAN_MAX_DATA_LEN               (USART_DATA_SIZE-BAOSHIAN_FIXED_LEN)
#define BAOSHIAN_MAX_LEN                    (BAOSHIAN_MAX_DATA_LEN+BAOSHIAN_FIXED_LEN+1)
#define BSA_HEADERCHECK_LEN                 3

//index
#define BAOSHIAN_PACKECHECK_INDEX_INDATA	4
#define BAOSHIAN_LENGTH_DEFINE_INDEX	    4

////protocol 
//flag
#define BSA_STARTCODE                       0xaa
#define BSA_ENDCODE                         0x55
//typeid
#define BSA_HOST2GAS_TYPEID                 0x1011
#define BSA_GAS2HOST_TYPEID                 0x1001
//cmd order flag
#define BSA_DEFAULT_CMD_ORDER               0
//funid
typedef enum{
	connec_bsafid=0,
	disconnec_bsafid,
	readdevinfo_bsafid,
	readchcfg_bsafid,
	setchcfg_bsafid,
	readchgasdata_bsafid,
	readrecordnum_bsafid,
	readrecorddata_bsafid,
	clearrecorddata_bsafid,
	setmutestate_bsafid,
	max_bsafid
}bsa_funid;

//sensor type
typedef enum
{
	co2_bsast=0,
	h2s_bsast,
	ex_bsast,
	o2_basst,
	so2_basst,
	ch4_basst,
	no_basst,
	no2_basst,
	cl2_basst,
	nh3_basst,
	h2_basst,
	hcn_basst,
	hcl_basst,
	ph3_basst,
	o3_basst,
	clo2_basst,
	c2h40_basst	
}bsa_sensortype;
//unit type
typedef enum
{
	umoldiviedmol_bsaunit=0,
	VOLpercent_bsaunit,
	LELpercent_bsaunit,
	PPM_bsaunit,
	mgdivied10cube_bsaunit,
	percent_bsaunit
}bsa_unit;
//decimal accuracy
typedef enum
{
	integer_bsadlay=0,
	onepoint_bsadlay,
	twopoint_bsadlay
}bsa_decimalaccuracy;

//gas state
typedef enum
{
	invalid_bsagste=0,
	normal_bsagste,
	lowalarm_bsagste,
	highalarm_bsagste,	
	exceedlimit_bsagste	
}bsa_gas_state;

//mute flag
#define BSA_MUTE                        0
#define BSA_NOTMUTE                     1

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
////////////////////////////////////////////////////////// protocol relevant struct
//baoshian ptotocol
#pragma pack(push, 1)
typedef struct
{
	u8 start_code;
	u16 length;      //length range is [type_id, end_code], so original message length is equal to [length]+4
	u8 head_check;  //zkrt_notice: head check range is [start_code, length]
	u16 type_id;
	u8 fun_id;
	u8 cmd_order;
}baoshian_header_ptcl;
typedef struct
{
	u8 packet_check;  //packet_check range is [type_id, data[valid]]
	u8 end_code;
}baoshian_tailer_ptcl;
struct baoshian_ptcl
{
	baoshian_header_ptcl header;
	u8 data[BAOSHIAN_MAX_DATA_LEN];
    baoshian_tailer_ptcl tailer;
};

////respond data define 
//get device info data at respond
typedef struct
{
	u8 version;
	u8 channel_num;
	char model[10];
}r_get_devinfo_bsapl;
//get channel parameter data at respond 
typedef struct
{
	u8 channel;
	u8 enabled;  //0-disable, 1-enable
	u8 sensor_type;
	u8 unit_type;
	u8 decimal_accuracy;
    u32 low_alarm;
	u32 high_alarm;
	u32 measure_range;
	u32 whatmean; //not define in the baoshian protocol
}r_get_chpara_bsapl;
//get channel gas value data at respond 
typedef struct
{
	u8 channel;
	u8 enabled;  //0-disable, 1-enable
	u8 update_flag;  //not use
	u32 gas_value;  //format is 32 bit float, need to change
	u16 decimal_accuracy;
    u16 state;
	u16 sensor_type;
	u16 unit_type;
	u32 measure_range; //the byte num is differnt with protocol document, because document error //zkrt_notice: it is like gas_value, format is 32 bit float
	u8 mute_flag;
}r_get_chgasvalue_bsa_pl;
#pragma pack(pop)
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////  dev info global use struct
//dev info
typedef r_get_devinfo_bsapl bsa_devinfo_st;
//gas info
typedef r_get_chgasvalue_bsa_pl bsa_gas_chinfo_st;

////////////////////////////////////////////////////////// communication handle struct 
//baoshian process handle struct
#define BSA_GET_ALLINFO_PERIOD      50  //500ms
struct bsa_prcs_handles
{
	u8 gas_online_flag;         //
	u8 gas_update_flag;         //when got the gas info, flag set 1, and clear to 0 when local application update the gas info
	volatile u8 get_channel_info_period; //when period > 200ms
};
////baoshian recv handle struct
//struct baoshian_recv_handle
//{
//	u8 recv_ok;       //1-receive complete, 0-receive not complete
//	u8 curser_state;  //handle index state
//	u8 data_index;
//};
////baoshian buffer
//struct baoshian_buffer
//{
//	struct baoshian_ptcl packet;
//	u8 data_buff[sizeof(struct baoshian_ptcl)];
//	u16 data_buff_len;
//};
/* Exported functions ------------------------------------------------------- */
void baoshian_init(void);
void baoshian_prcs(void);
char baoshian_send(const u8 *data, u8 datalen, u8 flag);
#endif /* __BAOSHIAN_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

