/**
  ******************************************************************************
  * @file    obstacleAvoid.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    08-March-2017
  * @brief   Header for obstacleAvoid.c module
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OBSTACLEAVOID_H
#define __OBSTACLEAVOID_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CAMERA_PAIR_NUM          5      //5��������
#define GUIDANCE_ONLINE_TIMEOUT  5000   //5s
#define OBSTACLE_ALARM_DISTANCE  500    //500cm 
#define OBSTACLE_DISTACNE_INITV  2000   //20m ��ֵ
#define OBSTACLE_AVOID_VEL       15     //�����ٶȳ���10����λm/s

//CONTROL MODE
#define OBSTACLE_VEL_MODE        0x4A    //0x4A: non-stable mode����������ϵ,HORI_VEL,VERT_VEL,YAW_RATE
//#define OBSTACLE_HORI_ANGLE_MODE 0x0A     //0x0A: 0b00001010 : non-stable mode����������ϵ,HORI_ATTI_TILT_ANG,VERT_VEL,YAW_RATE

#ifdef OBSTACLE_VEL_MODE
//VEL X AND Y FOR OBSTACLE
//flightData_obstacle.x = 0; //+��ǰ -���
//flightData_obstacle.y = 1; //+���� -����
//#define OBSTACLE_VEL_FORWORD_X   (1.5)    //zkrt_notice���ݶ�Ϊ1.5m/s����Ϊ����ʲô�ٶ�ɲ��Ч����� 
//#define OBSTACLE_VEL_BACK_X      (-1.5)
//#define OBSTACLE_VEL_RIGHT_Y     (1.5)
//#define OBSTACLE_VEL_LEFT_Y      (-1.5)
#define OBSTACLE_VEL_FORWORD_X(vel)   (vel*0.1)
#define OBSTACLE_VEL_BACK_X(vel)      (vel*(-0.1))
#define OBSTACLE_VEL_RIGHT_Y(vel)     (vel*0.1)
#define OBSTACLE_VEL_LEFT_Y(vel)      (vel*(-0.1))
#define OBSTACLE_MODE            OBSTACLE_VEL_MODE
#endif
#ifdef OBSTACLE_HORI_ANGLE_MODE
#define OBSTACLE_ANG_FORWORD_Y   (-4)      //Roll_X-������-������Pitch_Y-�����-����ǰ //zkrt_notice���ݶ�Ϊ5��
#define OBSTACLE_ANG_BACK_Y      (4)
#define OBSTACLE_ANG_RIGHT_X     (4)
#define OBSTACLE_ANG_LEFT_X      (-4)
#define OBSTACLE_MODE             OBSTACLE_HORI_ANGLE_MODE
#endif

/** @defcomment g_distance_value show below: 
  * @{
g_distance_value[0]: Guidance VBUS �� Port ��
g_distance_value[1]: Guidance VBUS �� Port ǰ
g_distance_value[2]: Guidance VBUS �� Port ��
g_distance_value[3]: Guidance VBUS �� Port ��
g_distance_value[4]: Guidance VBUS �� Port ��
  * @}
  */ 	

/** @def VBUS PORT to g_distance_value array mapping 
  */ 	
#define GE_VBUS1                  1
#define GE_VBUS2                  2
#define GE_VBUS3                  3
#define GE_VBUS4                  4
#define GE_VBUS5                  0
/** 
  * @}
  */ 	
	
/** @def Guidance direction mapping 
  */ 	
#define GE_DIR_DOWN               GE_VBUS5
#define GE_DIR_LEFT               GE_VBUS4
#define GE_DIR_RIGHT              GE_VBUS2
#define GE_DIR_BACK               GE_VBUS3
#define GE_DIR_FRONT              GE_VBUS1
/** 
  * @}
  */ 	
		
/** @def OBSTACLE_ALARM_DISTANCE definition
  */ 	
#define GE_ALARM_DISE_DOWN        3000   //Unit: cm  //���·��򲻱���
#define GE_ALARM_DISE_LEFT        OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_RIGHT       OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_BACK        OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_FRONT       OBSTACLE_ALARM_DISTANCE
/** 
  * @}
  */ 	
		
/* Exported typedef ------------------------------------------------------------*/
typedef struct{
	unsigned char g_distance_char[CAMERA_PAIR_NUM*2];     //ÿ�����������ݵ�charֵ
  unsigned short g_distance_value[CAMERA_PAIR_NUM];     //5������������ֵ��short�������ͣ���λcm��
	unsigned short g_distance_valid;                      //������charֵ������Ч��ǣ�bitλ��1������Ч��0������Ч��ȫ����Чʱ��g_distance_validֵ>=0x03ff
	unsigned char online_flag;                            //���߱�ǣ�1-���ߣ�0-�����ߡ��н��յ�����ʱ�����ߣ�30������һ�����߱�ǡ�������ʱ�����ϰ���������Ч���������ϴ���
	volatile unsigned int online_timing;                  //guidane���߳�ʱʱ����
	unsigned char ob_enabled;                             //����ʹ�ܱ�ǣ�0-��ʹ�ܣ�1-ʹ��
	unsigned short ob_distance;                           //������Ч����
	unsigned short ob_velocity;                           //ֵ����10����λm/s     
}obstacleData_st;

/* Exported functions ------------------------------------------------------- */
void guidance_init(void);
#ifndef USE_SESORINTEGRATED
void main_recv_decode_zkrt_dji_guidance(void);
#endif
unsigned char obstacle_avoidance_handle(void);
void guidance_parmdata_init(void);
extern obstacleData_st GuidanceObstacleData;

#endif /* __OBSTACLEAVOID_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
