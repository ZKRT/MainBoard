/**
  ******************************************************************************
  * @file    djiCtrl.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    08-March-2017
  * @brief   Header for djiCtrl.cpp module
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DJICTRL_H
#define __DJICTRL_H

/* Includes ------------------------------------------------------------------*/
#include "DJI_API.h"
#include "DJI_Camera.h"
#include "DJI_Type.h"
#include "DJI_VirtualRC.h"
#include "main.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//dji ����״̬
enum dji_run_state
{
	init_none_djirs = 0,		  //û������
	set_broadcastFreq_djirs,  //���͹㲥����
	set_broadcastFreq_ok,     //���ĳɹ�
	set_avtivate_djirs,		    //���� 
	avtivated_ok_djirs		    //����ɹ�
}; 	

//dji �ɿؿ���״̬��ǣ����ºͱ��Ͽ���ʱ��λ
enum dji_oes_fc_controlbit
{
	fc_tempctrl_b = 0,		  //���¿���λ��
	fc_obstacle_b           //���Ͽ���λ��
}; 	

		
/* Exported typedef ------------------------------------------------------------*/

//dji ����״̬�ṹ��
typedef struct
{
	char run_status;
	char temp_alarmed; //1-�������ݣ�0-����
	unsigned int cmdres_timeout;
	char oes_fc_controled; //���¿�����1<<0; ���Ͽ�����1<<1; ȫ����û�п�����0
	char last_fc_controled; //��һ�ε�oes_fc_controledֵ
}dji_sdk_status;

/* Exported functions ------------------------------------------------------- */
void dji_init(void);
void dji_process(void);
void dji_get_roll_pitch(double* roll, double* pitch);
extern dji_sdk_status djisdk_state;
#endif /* __DJICTRL_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
