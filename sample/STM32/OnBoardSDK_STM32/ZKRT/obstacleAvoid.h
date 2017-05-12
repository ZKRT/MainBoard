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
#define CAMERA_PAIR_NUM          5      //5个传感器
#define GUIDANCE_ONLINE_TIMEOUT  5000   //5s
#define OBSTACLE_ALARM_DISTANCE  500    //500cm 
#define OBSTACLE_DISTACNE_INITV  2000   //20m 初值
#define OBSTACLE_AVOID_VEL       15     //避障速度除以10，单位m/s

//CONTROL MODE
#define OBSTACLE_VEL_MODE        0x4A    //0x4A: non-stable mode，机体坐标系,HORI_VEL,VERT_VEL,YAW_RATE
//#define OBSTACLE_HORI_ANGLE_MODE 0x0A     //0x0A: 0b00001010 : non-stable mode，机体坐标系,HORI_ATTI_TILT_ANG,VERT_VEL,YAW_RATE

#ifdef OBSTACLE_VEL_MODE
//VEL X AND Y FOR OBSTACLE
//flightData_obstacle.x = 0; //+向前 -向后
//flightData_obstacle.y = 1; //+向右 -向左
//#define OBSTACLE_VEL_FORWORD_X   (1.5)    //zkrt_notice：暂定为1.5m/s，因为不管什么速度刹车效果差不多 
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
#define OBSTACLE_ANG_FORWORD_Y   (-4)      //Roll_X-正向右-负向左，Pitch_Y-正向后-负向前 //zkrt_notice：暂定为5度
#define OBSTACLE_ANG_BACK_Y      (4)
#define OBSTACLE_ANG_RIGHT_X     (4)
#define OBSTACLE_ANG_LEFT_X      (-4)
#define OBSTACLE_MODE             OBSTACLE_HORI_ANGLE_MODE
#endif

/** @defcomment g_distance_value show below: 
  * @{
g_distance_value[0]: Guidance VBUS ⑤ Port 下
g_distance_value[1]: Guidance VBUS ① Port 前
g_distance_value[2]: Guidance VBUS ② Port 右
g_distance_value[3]: Guidance VBUS ③ Port 后
g_distance_value[4]: Guidance VBUS ④ Port 左
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
#define GE_ALARM_DISE_DOWN        3000   //Unit: cm  //向下方向不避障
#define GE_ALARM_DISE_LEFT        OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_RIGHT       OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_BACK        OBSTACLE_ALARM_DISTANCE
#define GE_ALARM_DISE_FRONT       OBSTACLE_ALARM_DISTANCE
/** 
  * @}
  */ 	
		
/* Exported typedef ------------------------------------------------------------*/
typedef struct{
	unsigned char g_distance_char[CAMERA_PAIR_NUM*2];     //每个超声波数据的char值
  unsigned short g_distance_value[CAMERA_PAIR_NUM];     //5个超声波数据值，short数据类型，单位cm。
	unsigned short g_distance_valid;                      //超声波char值数据有效标记，bit位的1代表有效，0代表无效。全部有效时，g_distance_valid值>=0x03ff
	unsigned char online_flag;                            //在线标记，1-在线，0-不在线。有接收到数据时置在线，30秒会清除一次在线标记。不在线时，视障碍物数据无效，不做避障处理。
	volatile unsigned int online_timing;                  //guidane在线超时时间标记
	unsigned char ob_enabled;                             //避障使能标记，0-不使能，1-使能
	unsigned short ob_distance;                           //避障生效距离
	unsigned short ob_velocity;                           //值除以10，单位m/s     
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
