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
#define  UDCAIE_TRIGGER_UP_HEIGHT         4 //4m    //起落架触发高度
#define  UDCAIE_TRIGGER_DOWN_HEIGHT       3 //3m    //起落架触发高度
#define  UDCAIE_CHANGE_TIME               14 //7s    //起落架运动时间
#define  UCE_DOWNED_ANGLE                 700
#define  UCE_UPED_ANGLE                   0

/** @def OBSTACLE_ALARM_DISTANCE definition
  */

/**
  * @}
  */

/* Exported typedef ------------------------------------------------------------*/

//起落架运行状态值
enum udcaie_run_state
{
  downed_udcaie_rs = -1,      //降落稳态  //初始状态
  downing_udcaie_rs = -2 ,      //正在降落状态
  uping_udcaie_rs = 1,         //正在升起状态
  uped_udcaie_rs = 2,          //升起稳态

};

typedef struct {
  int run_state;               //起落架运行状态
  volatile u8 run_timeout;             //起落架运动时间  //3s
  volatile u8 run_timeoutflag;         //超时标记
  int state_bya3height;        //起落架应该处于的状态，根据飞控运行的高度来确定。if height>3m, state_bya3height = uped_udcaie_rs; if height < 3m, state_bya3height = downed_udcaie_rs
  u8 uce_autoenabled;     //脚架自动收放的使能状态
  u16 uce_angle;          //脚架当前角度值   : 与大地夹角， 最大降落角度是70度，最大升起角度是0度，运动时角度是
  u16 uce_autodown_ae;    //脚架自动降落的角度值
  u16 uce_autoup_ae;      //脚架自动升起的角度值
//  u8 uce_updown_runtime;      //起落架上升、下降需要运动的时间
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
