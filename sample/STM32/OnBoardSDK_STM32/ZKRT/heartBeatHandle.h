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

//������V2���ݰ��ṹ��
#pragma pack(push, 1)
//typedef struct
//{
////    u16 down_ob_distse;     //�·���
////    u16 front_ob_distse;    //ǰ�����ϰ������
////    u16 right_ob_distse;    //�ҷ����ϰ������
////    u16 back_ob_distse;     //�����ϰ������
////    u16 left_ob_distse;     //�����ϰ������
//  u16 ob_distse_v[5];     //�ϰ������
//  u8 avoid_ob_enabled;    //����ʹ��
//  u16 avoid_ob_distse;    //������Ч����
//  u16 avoid_ob_velocity;  //�����ٶ�
//  u8 uce_state;           //�ż�״̬
//  u8 uce_autoenabled;     //�ż��Զ��շŵ�ʹ��״̬
//  u16 uce_angle;          //�żܵ�ǰ�Ƕ�ֵ
//  u16 uce_autodown_ae;    //�ż��Զ�����ĽǶ�ֵ
//  u16 uce_autoup_ae;      //�ż��Զ�����ĽǶ�ֵ
//  u8 reserved[7];         //����
//}heartv2_st;
//typedef struct
//{
//  u16 ob_distse_v[5];     //�ϰ������
//  u8 avoid_ob_enabled;    //����ʹ��
//  u16 avoid_ob_distse;    //������Ч����
//  u16 avoid_ob_velocity;  //�����ٶ�
//  u8 uce_state;           //�ż�״̬
//  u8 uce_autoenabled;     //�ż��Զ��շŵ�ʹ��״̬
//  u8 gas_num5;
//  u16 gas_v5;
//  u8 gas_num6;
//  u16 gas_v6;
//  u8 gas_num7;
//  u16 gas_v7;
//  u8 gas_num8;
//  u16 gas_v8;
//  u8 reserved[1];         //����
//}heartv2_st;

#pragma pack(pop)

//��ʱ�ϴ����У������������ϴ�ʱ���� TIMER_MSG_TIMEOUT����ͬ���������ʱ���� TIMER_MSG_NEXT_TIMEOUT
enum TimerMsgUpSeq
{
  Hbv1_Seq = 0,
  Hbv2_Seq,
  TMU_End_Seq
};

//��ʱ�ϴ��ṹ��
typedef struct
{
#define TIMER_HBV2PACK_TIMEOUT            100
#define TIMER_MSG_TIMEOUT                 700
#define TIMER_MSG_NEXT_TIMEOUT            100

  volatile u8 timer_hbv2_packflag; //�����ʱ���
  volatile u16 timer_hbv2_packcnt;

  volatile u8 timer_msg_upflag; //���ݶ�ʱ�ϴ����
  volatile u16 timer_msg_timecnt;

} timer_upload_st;

/* Exported functions ------------------------------------------------------- */
void heartbeat_parm_init(void);
u8 zkrt_heartbeat_pack(u8* data, u8* datalen);

extern timer_upload_st hb_timer; //��������ʱ�ṹ��
//extern zd_heartv2_st zkrt_heartv2;  //��ʱ����������v2�������建��
//extern zd_heartv1_st zkrt_heartv1;
extern zd_heartv3_3_st zkrt_heartbeat;
extern zkrt_packet_t _zkrt_packet_hb;   //���͵�DJI͸�����ݵ�����������

#endif /* __UNDERCARRIAGECTRL_H */

/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
