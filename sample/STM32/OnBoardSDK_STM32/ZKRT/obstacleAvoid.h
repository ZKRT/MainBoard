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
#define OB_ENABLED_INIT                 0      //
#define CAMERA_PAIR_NUM                 5      //5��������
#define GUIDANCE_ONLINE_TIMEOUT         5000   //5s
#define OBSTACLE_ALARM_DISTANCE         500    //500cm 
#define OBSTACLE_DISTACNE_INITV         2000   //20m ��ֵ
#define OBSTACLE_AVOID_VEL_10TIMES      10     //�����ٶȳ���10����λm/s    //==1m/s
#define OBSTACLE_AVOID_VEL(vel10times)  (vel10times*0.1)     //�����ٶȣ���λm/s
//new
#define OBSTACLE_SAFE_DISTANCE	        OBSTACLE_ALARM_DISTANCE    //���Ͼ��԰�ȫ����
#define OBSTACLE_RETURN_DISTANCE	      200    //����������Ч����
#define OBSTACLE_SAFEH_VEL	            3      //������߰�ȫ�ٶ�3m/s   
#define OBSTACLE_ENABLED_DISTANCE	      1000   //�������㷨�����ϵ���Ч���� 10�� 
#define RC_H_VEL	                      13     //ң�������ʱ�� //zkrt_notice ���Խ����
#define RC_H_VEL_IN5000CH	              3     //ң�������ʱ����ң������ֵ��5000����ʱ

//�ϰ�������Ӧ����������б�Ƕȣ�����ʱ�����ö�Ӧ����ϰ������
#define ANGLE2GREAT_DISE15							0.134
#define ANGLE2GREAT_DISE12							0.167
#define ANGLE2GREAT_DISE10							0.190
#define ANGLE2GREAT_DISE                ANGLE2GREAT_DISE10
//��б�Ƕ�У׼ֵ����ΪTOF�����д�ֱ�����1�����Ҽ��
#define ANGLE2GREAT_CALIBRA             0.0175f   //1��
//��Ҫ���˵���Ч�߶�
#define ANGLE2GREAT_HEIGHT              4  //4m

//special distance define
#define DISTANCE_NONE                   5000   //invalid
#define DISTANCE_2LOW                   7000   //<30
#define DISTANCE_2HIGH                  8000   //>5000
#define DISTANCE_2HIGH_BY_ANGLE         9000

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
//special
#define GE_DIR_THROTTLE						5
#define GE_DIR_YAW						    6
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
//���Ͽ���״̬
enum OBSTACLE_CONTROL_STATE
{
    OCS_NO_CONTROL = 0,                       //�����ƣ��ͷſ���Ȩ��ң��
    OCS_LIMITING_VEL	= 1,                    //���ٿ��ƣ�ң����û��Ȩ��
    OCS_HOVER = 2,                            //������ͣ��ң����û��Ȩ��
    OCS_VOLUNTRAY_AVOID = 3,                  //��������ϰ���ң����û��Ȩ��
    OCS_OES_REPLACE_RC                        //OES����RC����
};
//enum OBSTACLE_X_Y_STATE
//{
//	XYS_FRONT_RIGHT =0,
//	XYS_BACK_LIFT,
//	XYS_HOVER,
//	XYS_RC
//};
/* Exported typedef ------------------------------------------------------------*/
typedef struct
{
    unsigned char g_distance_char[CAMERA_PAIR_NUM*2];     //ÿ�����������ݵ�charֵ
    unsigned short g_distance_value[CAMERA_PAIR_NUM];     //5������������ֵ��short�������ͣ���λcm��
    unsigned short
    g_distance_valid;                      //������charֵ������Ч��ǣ�bitλ��1������Ч��0������Ч��ȫ����Чʱ��g_distance_validֵ>=0x03ff
    unsigned char
    online_flag;                            //���߱�ǣ�1-���ߣ�0-�����ߡ��н��յ�����ʱ�����ߣ�30������һ�����߱�ǡ�������ʱ�����ϰ���������Ч���������ϴ���
    volatile unsigned int online_timing;                  //guidane���߳�ʱʱ����
    unsigned char ob_enabled;                             //����ʹ�ܱ�ǣ�0-��ʹ�ܣ�1-ʹ��
    unsigned short ob_distance;                           //������Ч����
    unsigned short ob_velocity;                           //ֵ����10����λm/s
    volatile unsigned char obstacle_time[4];
    volatile unsigned char obstacle_time_flag[4];
    volatile unsigned char constant_speed_time[4];
    volatile unsigned char constant_speed_time_flag[4];
} obstacleData_st;

//dji������Ҫ���ǵĻ�������ṹ��
typedef struct
{
    double roll;
    double pitch;
    float xnow;
    float ynow;
    int16_t rc_roll;
    int16_t rc_pitch;
    int16_t rc_throttle;
    int16_t rc_yaw;
    float height;
    float fiter_angle_ob;
} dji_flight_status;
//���Ͽ��ƽṹ�壬4��������Ҫ����һ������ṹ��
typedef struct
{
    uint8_t state;  //���Ͽ���״̬
    uint8_t last_state;  //���Ͽ���״̬��һ��
    uint8_t opposite;              //�����棬��ǰ�Ķ������Ǻ���Ķ���������
//	uint8_t adjoin1;               //����1
//	uint8_t adjoin2;               //����2
} obstacleControl_st;
//���Ͽ��ƽṹ��
typedef struct
{
    obstacleControl_st control[4];
    uint8_t x_state; //x�����״̬ OBSTACLE_X_Y_STATE
    uint8_t y_state; //y�����״̬ OBSTACLE_X_Y_STATE

} obstacleAllControl_st;
/* Exported functions ------------------------------------------------------- */
void guidance_init(void);
void obstacle_control_parm_init(void);
void obstacle_control_run_reset(void);
#ifndef USE_SESORINTEGRATED
void main_recv_decode_zkrt_dji_guidance(void);
#endif
unsigned char obstacle_avoidance_handle(void);
unsigned char obstacle_avoidance_self_handle(float *flight_x, float *flight_y, char *obstacle_dir);
unsigned char obstacle_avoidance_handle_V2(float *flight_x, float *flight_y,  int16_t RCData_x,
        int16_t RCData_y);
unsigned char obstacle_avoidance_handle_V3(float *flight_x, float *flight_y,  int16_t RCData_x,
        int16_t RCData_y, const float *flight_x_now, const float *flight_y_now);
void guidance_parmdata_init(void);
uint8_t obstacle_check_per_dirc(dji_flight_status *dfs, uint16_t distance, uint8_t direction);
uint8_t is_rc_goto_dir(uint8_t dir);
float get_filter_ang_ob(void);
extern obstacleData_st GuidanceObstacleData;
extern dji_flight_status djif_status;
extern obstacleAllControl_st obstacleAllControl;
#endif /* __OBSTACLEAVOID_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
