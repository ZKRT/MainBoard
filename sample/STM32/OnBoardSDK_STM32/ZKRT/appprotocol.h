/**
  ******************************************************************************
  * @file    appprotocol.h
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
  * @brief
    *                    + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APPPROTOCOL_H
#define __APPPROTOCOL_H
/* Includes ------------------------------------------------------------------*/
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/

//version and model string //zkrt_notice
#define DEV_MODEL_MB              "OMU"
#define DEV_HW_MB                 "000400"
#define DEV_SW_MB                 "000700"

#define DEV_MODEL_BSA             "M-GDBSA"
//hw and sw is same as mb

//check msg type
#define IS_LOCALMSG(TYPE)         (((TYPE) == DEVICE_TYPE_TEMPERATURE)    || \
                                  ((TYPE) == DEVICE_TYPE_MAINBOARD))

//api_enabled
#define API_INVALID        0
#define API_VALID          1
//program_mode
#define BOOT_MODE          1
#define API_MODE           2

/* Exported constants --------------------------------------------------------*/
//////////////////////////////////////////////////////special define
//gas
#define MAX_CHANNEL_GAS              8

//////////////////////////////////////////////////////

//#define TN_HEARTBEAT                  1
//type num enum
typedef enum {
    TN_None = 0,
    TN_HEARTBEAT,
    TN_GETDEVINFO,
    TN_JUMPBOOTMODE,
    TN_GETBOOTINFO,
    TN_FWUPDATESTART,
    TN_FWUPDATEING,
    TN_FWUPDATEOK,
    TN_FWUPDATECANCEL,
    TN_MAX
} TypeNumEnum;
//common respond status
typedef enum {
    TNS_OK = 0,
    TNS_FAIL,
    TNS_FAIL_UPDATE,
    TNS_FAIL_FWSIZE, //固件超过允许的大小
	TNS_FAIL_SEQ, //序列号出错，可能丢包
	TNS_FAIL_INVALID_FW,//无效的固件包,可能没有发送准备升级命令
	TNS_FAIL_FWWRITE //写失败
} CommonRespondState;
//hb flag
#define TNHB_FLAG                     0xaabbccdd
#define THHB_FIXED_LEN                5
//device online status flag value
#define DV_ONLINE                     1
#define DV_OFFLINE                    0
#define DV_ONLINE_TIMEOUT             5000  //5000ms
//gas status flag value
#define GAS_NORMAL                    1
#define GAS_ERROR                     0
//feedback switch flag value
#define FSW_OPEN                      1
#define FSW_CLOSE                     0

//temperature status
#define TEMP_NOMAL                        0xFE
#define TEMP_INVALID                      0xFD
#define TEMP_OVER_HIGH                    0xFC
#define TEMP_OVER_LOW                     0xFB

/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
///////////////////////////////uav to app zkrt data struct
////heartbeat1
//device online status bit number  ---same as DEVICE_TYPE_XX
//device online status value bit
typedef struct {
    u8 temperuture : 1;
    u8 obstacle : 1;
    u8 control : 1;
    u8 siglecamera : 1;
    u8 gas : 1;
    u8 throwd : 1;
    u8 reserved1 : 1;
    u8 reserved2 : 1;
    u8 irradiate : 1;
    u8 megaphone : 1;
    u8 battery : 1;
    u8 threemodelling : 1;
    u8 multicamera : 1;
    u8 reserved3 : 3;
    u8 reserved4 : 8;
} dos_valuebit;
//device online status union
typedef union {
    u8 value[3];
    dos_valuebit valuebit;
} devicestatus_un;
//feedback
//feedback value bit
typedef struct {
    u8 throw1_s : 1;
    u8 throw2_s : 1;
    u8 throw3_s : 1;
    u8 irradiate_s : 1;
    u8 megaphone_s: 1;
    u8 threemodeling_photo_s: 1;
    u8 reserved1 : 2;
    u8 reserved2 : 8;
    u8 reserved3 : 8;
} feedback_valuebit;
//feedback union
typedef union {
    u8 value[3];
    feedback_valuebit valuebit;
} feedback_un;
//heartbeat1 struct
typedef struct {
    u8 t1_status;
    u16 t1;
    u8 t2_status;
    u16 t2;
    u16 t_low;
    u16 t_high;
    union_byte gas_status;
    u8 gas_num1;
    u16 gas_v1;
    u8 gas_num2;
    u16 gas_v2;
    u8 gas_num3;
    u16 gas_v3;
    u8 gas_num4;
    u16 gas_v4;
    devicestatus_un dev_online_s;
    feedback_un feedback_s;
    u8 voltage;
} zd_heartv1_st;
////heartbeat2
typedef struct {
    u16 ob_distse_v[5];     //障碍物距离
    u8 avoid_ob_enabled;    //避障使能
    u16 avoid_ob_distse;    //避障生效距离
    u16 avoid_ob_velocity;  //避障速度
    u8 uce_state;           //脚架状态
    u8 uce_autoenabled;     //脚架自动收放的使能状态
    u8 gas_num5;
    u16 gas_v5;
    u8 gas_num6;
    u16 gas_v6;
    u8 gas_num7;
    u16 gas_v7;
    u8 gas_num8;
    u16 gas_v8;
    u8 reserved[1];         //备用
} zd_heartv2_st;
//the device relevant of heartbeat protocol in version3.3
typedef struct {
    devicestatus_un dev_online_s;
    feedback_un feedback_s;
} hb_devinfo_st;
//the temperature relevant of heartbeat protocol in version3.3
typedef struct {
    u16 t_value;
    u8 t_status;
    u16 t_low;
    u16 t_high;
} hb_temper_st;
//the obstacle avoidance relevant of heartbeat protocol in version3.3
typedef struct {
    u16 ob_distse_v[5];     //障碍物距离
    u8 avoid_ob_enabled;    //避障使能
    u16 avoid_ob_distse;    //避障生效距离
    u16 avoid_ob_velocity;  //避障速度
} hb_obstacle_st;
//the gas relevant of heartbeat protocol in version3.3
typedef struct {
    u8 ch_num;
    u16 ch_status;
    u32 gas_value[MAX_CHANNEL_GAS];  //max channel num is 8
} hb_gas_st;
//the heartbeat protocol in version3.3
typedef struct {
    hb_devinfo_st dev;
    hb_temper_st temper;
    hb_obstacle_st obstacle;
    hb_gas_st gas;
} zd_heartv3_3_st;
///////////////////////////////app to uav zkrt data struct
////command=normal command, device type=temperture
typedef struct {
    short t_low;
    short t_high;
    u8 reserved[26];
} tempture_comnor_plst;
////command=normal command, device type=mainboard
typedef struct {
    u8 ctrl_num;
    u8 ctrl_data[ZK_DATA_MAX_LEN - 1];
} comnor_mainboard_plst;
//ostabcle control
typedef struct {
    u8 enable;
    u16 enbale_distance; //cm
    short vel;           //m/s, but /10, example: when vel = 101, the actul vel is 10.1m/s
    u8 reserved[24];
} ostacle_mb_plst;
//undercarriage control
typedef struct {
    u8 enable;
    short autodown_angle;    //value/10, example: when autodown_angle = 101, the actul value is 10.1 degree
    short autoup_angle; //value/10, example: when autoup_angle = 101, the actul value is 10.1 degree
    u8 adjust_action;
    u8 control;
    u8 reserved[22];
} undercarriage_mb_plst;
///////////////////////////////uav to subdev zkrt data struct
///////////////////////////////subdev to uav zkrt data struct
///////////////////////////////////////////////////////////common command
typedef struct {
    u8 type_num;
    u8 type_data[ZK_DATA_MAX_LEN - 1];
} common_data_plst;
//heartbeat data
typedef struct {
    u32 hb_flag;
    u8 hb_data[ZK_DATA_MAX_LEN - THHB_FIXED_LEN];
} common_hbd_plst;
//throw heartbeat
typedef struct {
//  hb_header_ccplst hbh;
    u8 v1;
    u8 v2;
    u8 v3;
} throw_hbccplst;
//irradiate heartbeat
typedef struct {
//  hb_header_ccplst hbh;
    u8 value;
} irradiate_hbccplst;
//3Dmodeling heartbeat
typedef irradiate_hbccplst threemodel_hbccplst;
//gas heartbeat
typedef struct {
//  hb_header_ccplst hbh;
    u8 num;
    u8 status;
    u8 gas1;
    u16 gas1v;
    u8 gas2;
    u16 gas2v;
    u8 gas3;
    u16 gas3v;
    u8 gas4;
    u16 gas4v;
    u8 gas5;
    u16 gas5v;
    u8 gas6;
    u16 gas6v;
    u8 gas7;
    u16 gas7v;
    u8 gas8;
    u16 gas8v;
} gas_hbccplst;
typedef hb_gas_st gas_v3_3_hbccplst;
//dev info data
typedef struct {
    u8 status;
    u8 model[10];
    u8 hw_version[6];
    u8 sw_version[6];
} common_get_devinfo_plst;
//jump_boot_mode_comnf
typedef struct {
    u8 mode;
} common_jumpbootmode_plst;
///////////////////////////////app to subdev zkrt data struct
////command=specify command
//respond flag
#define NEED_RETRUN                   1
#define NOTNEED_RETRUN                0

#define RES_HEADER_LEN  2  //equal to control num+status
//type num enum
typedef enum {
    NONE_GASCN = 0,
    GETCHNUM_GASCN = 1,
    GETCHINFO_GASCN,
    MAX_GASCN
} gas_ctrlnume;
//gas respond status enum
typedef enum {
    S_Success_Gas,
    S_Fail_Gas,
    S_FailNoDev_Gas,
    S_FailNoChannel_Gas
} ResStatusEnumGas;
typedef struct {
    u8 control_num;
    u8 other_data[ZK_DATA_MAX_LEN - 1];
} send_comspec_plst;
typedef struct {
    u8 control_num;
    u8 status;
    u8 other_data[ZK_DATA_MAX_LEN - 2];
} respond_comspec_plst;
//////send and respond sub
////device type=gas type
//get gas channel num  --not send, have respond
typedef struct {
    u8 ch_num;
} rgetchnum_gas_comspecplst;
//get gas channel info
typedef struct {
    u8 ch;
} getchinfo_gas_comspecplst;
typedef struct {
    u8 ch;
    u8 reseved[2];
    u32 value;
    u8 reseved2[2];
    u16 sensor_type;
    u16 unit_type;
    u32 range;
    u8 reserved3;
} rgetchinfo_gas_comspecplst;
///////////////////////////////subdev to app zkrt data struct
///////////////////////////////
#pragma pack()
/**
*   @brief  protocl handle funciton
  * @param  void* source data
  * @param  void* respond data
  * @param  u8 source data length
  * @param  u8* respond data length
  * @retval NEED_RETURN: need send respond msg, NOTNEED_RETRUN: no need send respond msg
  */
typedef char (*COMMON_HB_PTOCOL_FUN)(const void*, u8, u8);

/* Exported functions ------------------------------------------------------- */
extern const COMMON_HB_PTOCOL_FUN cmon_hb_ptcol_fun[];

#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

