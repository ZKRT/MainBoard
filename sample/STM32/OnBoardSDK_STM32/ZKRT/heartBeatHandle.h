/**
  ******************************************************************************
  * @file    heartBeatHandle.h
  * @author  ZKRT
  * @version V1.0.0
  * @date    25-March-2017
  * @brief   Header for heartBeatHandle.c module
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HEARTBEATHANDLE_H
#define __HEARTBEATHANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appprotocol.h"
/* Exported macro ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/


/** @def OBSTACLE_ALARM_DISTANCE definition
  */

/**
  * @}
  */

/* Exported typedef ------------------------------------------------------------*/

//心跳包V2数据包结构体
#pragma pack(push, 1)
//typedef struct
//{
////    u16 down_ob_distse;     //下方向
////    u16 front_ob_distse;    //前方向障碍物距离
////    u16 right_ob_distse;    //右方向障碍物距离
////    u16 back_ob_distse;     //后方向障碍物距离
////    u16 left_ob_distse;     //左方向障碍物距离
//  u16 ob_distse_v[5];     //障碍物距离
//  u8 avoid_ob_enabled;    //避障使能
//  u16 avoid_ob_distse;    //避障生效距离
//  u16 avoid_ob_velocity;  //避障速度
//  u8 uce_state;           //脚架状态
//  u8 uce_autoenabled;     //脚架自动收放的使能状态
//  u16 uce_angle;          //脚架当前角度值
//  u16 uce_autodown_ae;    //脚架自动降落的角度值
//  u16 uce_autoup_ae;      //脚架自动升起的角度值
//  u8 reserved[7];         //备用
//}heartv2_st;
//typedef struct
//{
//  u16 ob_distse_v[5];     //障碍物距离
//  u8 avoid_ob_enabled;    //避障使能
//  u16 avoid_ob_distse;    //避障生效距离
//  u16 avoid_ob_velocity;  //避障速度
//  u8 uce_state;           //脚架状态
//  u8 uce_autoenabled;     //脚架自动收放的使能状态
//  u8 gas_num5;
//  u16 gas_v5;
//  u8 gas_num6;
//  u16 gas_v6;
//  u8 gas_num7;
//  u16 gas_v7;
//  u8 gas_num8;
//  u16 gas_v8;
//  u8 reserved[1];         //备用
//}heartv2_st;

#pragma pack(pop)

//定时上传序列，心跳包周期上传时间是 TIMER_MSG_TIMEOUT，不同心跳包间隔时间是 TIMER_MSG_NEXT_TIMEOUT
enum TimerMsgUpSeq
{
  Hbv1_Seq = 0,
  Hbv2_Seq,
  TMU_End_Seq
};

//定时上传结构体
typedef struct
{
#define TIMER_HBV2PACK_TIMEOUT            100
#define TIMER_MSG_TIMEOUT                 700
#define TIMER_MSG_NEXT_TIMEOUT            100

  volatile u8 timer_hbv2_packflag; //组包定时标记
  volatile u16 timer_hbv2_packcnt;

  volatile u8 timer_msg_upflag; //数据定时上传标记
  volatile u16 timer_msg_timecnt;

} timer_upload_st;

/* Exported functions ------------------------------------------------------- */
void heartbeat_parm_init(void);
u8 zkrt_heartbeat_pack(u8* data, u8* datalen);

extern timer_upload_st hb_timer; //心跳包定时结构体
//extern zd_heartv2_st zkrt_heartv2;  //定时发送心跳包v2的数据体缓存
//extern zd_heartv1_st zkrt_heartv1;
extern zd_heartv3_3_st zkrt_heartbeat;
extern zkrt_packet_t _zkrt_packet_hb;   //发送到DJI透传数据的心跳包缓存

#endif /* __UNDERCARRIAGECTRL_H */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
