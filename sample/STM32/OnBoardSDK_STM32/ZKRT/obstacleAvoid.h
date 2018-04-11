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
#define CAMERA_PAIR_NUM                 5      //5个传感器
#define GUIDANCE_ONLINE_TIMEOUT         5000   //5s
#define OBSTACLE_ALARM_DISTANCE         500    //500cm 
#define OBSTACLE_DISTACNE_INITV         2000   //20m 初值
#define OBSTACLE_AVOID_VEL_10TIMES      10     //避障速度除以10，单位m/s    //==1m/s
#define OBSTACLE_AVOID_VEL(vel10times)  (vel10times*0.1)     //避障速度，单位m/s
//new
#define OBSTACLE_SAFE_DISTANCE	        OBSTACLE_ALARM_DISTANCE    //避障绝对安全距离
#define OBSTACLE_RETURN_DISTANCE	      200    //主动避障生效距离
#define OBSTACLE_SAFEH_VEL	            3      //避障最高安全速度3m/s   
#define OBSTACLE_ENABLED_DISTANCE	      1000   //避障新算法，避障的生效距离 10米 
#define RC_H_VEL	                      13     //遥控器最高时速 //zkrt_notice 测试结果？
#define RC_H_VEL_IN5000CH	              3     //遥控器最高时速在遥控器阈值在5000以内时

//障碍物距离对应的最大机体倾斜角度：过大时，重置对应面的障碍物距离
#define ANGLE2GREAT_DISE15							0.134
#define ANGLE2GREAT_DISE12							0.167
#define ANGLE2GREAT_DISE10							0.190
#define ANGLE2GREAT_DISE                ANGLE2GREAT_DISE10
//倾斜角度校准值，因为TOF本身有垂直方向的1度左右检测
#define ANGLE2GREAT_CALIBRA             0.0175f   //1度
//需要过滤的有效高度
#define ANGLE2GREAT_HEIGHT              4  //4m

//special distance define
#define DISTANCE_NONE                   5000   //invalid
#define DISTANCE_2LOW                   7000   //<30
#define DISTANCE_2HIGH                  8000   //>5000
#define DISTANCE_2HIGH_BY_ANGLE         9000

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
//special
#define GE_DIR_THROTTLE						5
#define GE_DIR_YAW						    6
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
//避障控制状态
enum OBSTACLE_CONTROL_STATE
{
    OCS_NO_CONTROL = 0,                       //不控制，释放控制权予遥控
    OCS_LIMITING_VEL	= 1,                    //限速控制，遥控器没有权限
    OCS_HOVER = 2,                            //控制悬停，遥控器没有权限
    OCS_VOLUNTRAY_AVOID = 3,                  //主动躲避障碍，遥控器没有权限
    OCS_OES_REPLACE_RC                        //OES代替RC控制
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
    unsigned char g_distance_char[CAMERA_PAIR_NUM*2];     //每个超声波数据的char值
    unsigned short g_distance_value[CAMERA_PAIR_NUM];     //5个超声波数据值，short数据类型，单位cm。
    unsigned short
    g_distance_valid;                      //超声波char值数据有效标记，bit位的1代表有效，0代表无效。全部有效时，g_distance_valid值>=0x03ff
    unsigned char
    online_flag;                            //在线标记，1-在线，0-不在线。有接收到数据时置在线，30秒会清除一次在线标记。不在线时，视障碍物数据无效，不做避障处理。
    volatile unsigned int online_timing;                  //guidane在线超时时间标记
    unsigned char ob_enabled;                             //避障使能标记，0-不使能，1-使能
    unsigned short ob_distance;                           //避障生效距离
    unsigned short ob_velocity;                           //值除以10，单位m/s
    volatile unsigned char obstacle_time[4];
    volatile unsigned char obstacle_time_flag[4];
    volatile unsigned char constant_speed_time[4];
    volatile unsigned char constant_speed_time_flag[4];
} obstacleData_st;

//dji控制需要考虑的机体参数结构体
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
//避障控制结构体，4个方向需要定义一个数组结构体
typedef struct
{
    uint8_t state;  //避障控制状态
    uint8_t last_state;  //避障控制状态上一次
    uint8_t opposite;              //对立面，如前的对立面是后，左的对立面是右
//	uint8_t adjoin1;               //相邻1
//	uint8_t adjoin2;               //相邻2
} obstacleControl_st;
//避障控制结构体
typedef struct
{
    obstacleControl_st control[4];
    uint8_t x_state; //x轴控制状态 OBSTACLE_X_Y_STATE
    uint8_t y_state; //y轴控制状态 OBSTACLE_X_Y_STATE

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
