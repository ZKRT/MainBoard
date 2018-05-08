/**
  ******************************************************************************
  * @file    undercarriageCtrl.h
  * @author  ZKRT
  * @version V1.0.0
  * @date    15-March-2017
  * @brief   Header for undercarriageCtrl.c module
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UNDERCARRIAGECTRL_H
#define __UNDERCARRIAGECTRL_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/

//out1 and out2
#define UDCAIE_LEFT_UP                 {PFout(8)=0;PAout(0)=1;}
#define UDCAIE_LEFT_DOWN               {PFout(8)=1;PAout(0)=0;}
#define UDCAIE_LEFT_STOP               {PFout(8)=0;PAout(0)=0;}
//out3 and out4
#define UDCAIE_RIGHT_UP                {PFout(7)=0;PFout(9)=1;}
#define UDCAIE_RIGHT_DOWN              {PFout(7)=1;PFout(9)=0;}
#define UDCAIE_RIGHT_STOP              {PFout(7)=0;PFout(9)=0;}

/* Exported constants --------------------------------------------------------*/
#define  UDCAIE_TRIGGER_UP_HEIGHT         4 //4m    //����ܴ����߶�
#define  UDCAIE_TRIGGER_DOWN_HEIGHT       3 //3m    //����ܴ����߶�
#define  UDCAIE_CHANGE_TIME               14 //7s    //������˶�ʱ��
#define  UCE_DOWNED_ANGLE                 700
#define  UCE_UPED_ANGLE                   0

/** @def OBSTACLE_ALARM_DISTANCE definition
  */

/**
  * @}
  */

/* Exported typedef ------------------------------------------------------------*/

//���������״ֵ̬
enum udcaie_run_state
{
  downed_udcaie_rs = -1,      //������̬  //��ʼ״̬
  downing_udcaie_rs = -2 ,      //���ڽ���״̬
  uping_udcaie_rs = 1,         //��������״̬
  uped_udcaie_rs = 2,          //������̬

};

typedef struct {
  int run_state;               //���������״̬
  volatile u8 run_timeout;             //������˶�ʱ��  //3s
  volatile u8 run_timeoutflag;         //��ʱ���
  int state_bya3height;        //�����Ӧ�ô��ڵ�״̬�����ݷɿ����еĸ߶���ȷ����if height>3m, state_bya3height = uped_udcaie_rs; if height < 3m, state_bya3height = downed_udcaie_rs
  u8 uce_autoenabled;     //�ż��Զ��շŵ�ʹ��״̬
  u16 uce_angle;          //�żܵ�ǰ�Ƕ�ֵ   : ���ؼнǣ� �����Ƕ���70�ȣ��������Ƕ���0�ȣ��˶�ʱ�Ƕ���
  u16 uce_autodown_ae;    //�ż��Զ�����ĽǶ�ֵ
  u16 uce_autoup_ae;      //�ż��Զ�����ĽǶ�ֵ
//  u8 uce_updown_runtime;      //������������½���Ҫ�˶���ʱ��
} undercarriage_st;
/* Exported functions ------------------------------------------------------- */
void undercarriage_init(void);
void undercarriage_process(void);

extern undercarriage_st undercarriage_data;
#endif /* __UNDERCARRIAGECTRL_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
