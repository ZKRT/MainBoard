/**
  ******************************************************************************
  * @file    obstacleAvoid.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    08-March-2017
  * @brief   obstacleAvoid program body use guidance
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "obstacleAvoid.h"
#include "usart.h"
#include "commonzkrt.h"
#include <math.h>

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//obstacleData_st GuidanceObstacleData = {{0}, {0}, 0, 0, 0xffffffff, 1, OBSTACLE_ALARM_DISTANCE, OBSTACLE_AVOID_VEL_10TIMES};
obstacleData_st GuidanceObstacleData;
volatile uint8_t guidance_v_index = 0; //数据包解包的index，解完一个字节，index指向下一个字节数据
dji_flight_status djif_status;
obstacleAllControl_st obstacleAllControl;  //避障算法主要控制结构体
/* Private functions ---------------------------------------------------------*/

/**
*   @brief  guidance_parmdata_init
  * @parm   none
  * @retval none
  */
void guidance_parmdata_init(void)
{
	GuidanceObstacleData.g_distance_value[0] = OBSTACLE_DISTACNE_INITV;
	GuidanceObstacleData.g_distance_value[1] = OBSTACLE_DISTACNE_INITV;
	GuidanceObstacleData.g_distance_value[2] = OBSTACLE_DISTACNE_INITV;
	GuidanceObstacleData.g_distance_value[3] = OBSTACLE_DISTACNE_INITV;
	GuidanceObstacleData.g_distance_value[4] = OBSTACLE_DISTACNE_INITV;

//	GuidanceObstacleData.g_distance_value[0] = 2000;
//	GuidanceObstacleData.g_distance_value[1] = 2000;
//	GuidanceObstacleData.g_distance_value[2] = 1500;
//	GuidanceObstacleData.g_distance_value[3] = 500;
//	GuidanceObstacleData.g_distance_value[4] = 300;

	GuidanceObstacleData.online_flag = 0;
	GuidanceObstacleData.g_distance_valid = 0;
	GuidanceObstacleData.online_timing = 0xffffffff;
//	GuidanceObstacleData.ob_enabled = 1;                                     //this parameter init read from flash in function of STMFLASH_Init().
//	GuidanceObstacleData.ob_distance = OBSTACLE_ALARM_DISTANCE;
//	GuidanceObstacleData.ob_velocity = OBSTACLE_AVOID_VEL_10TIMES;
}
/**
*   @brief  guidance_init
  * @parm   none
  * @retval none
  */
void guidance_init(void)
{
	guidance_parmdata_init();
	obstacle_control_parm_init();
}
/**
*   @brief  obstacle_control_init
  * @parm   none
  * @retval none
  */
void obstacle_control_parm_init(void)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		obstacleAllControl.control[i].state = OCS_NO_CONTROL;
		obstacleAllControl.control[i].last_state = obstacleAllControl.control[i].state;
	}
	obstacleAllControl.control[GE_DIR_FRONT - 1].opposite = GE_DIR_BACK;
	obstacleAllControl.control[GE_DIR_BACK - 1].opposite = GE_DIR_FRONT;
	obstacleAllControl.control[GE_DIR_RIGHT - 1].opposite = GE_DIR_LEFT;
	obstacleAllControl.control[GE_DIR_LEFT - 1].opposite = GE_DIR_RIGHT;
	obstacleAllControl.x_state = OCS_NO_CONTROL;
	obstacleAllControl.y_state = OCS_NO_CONTROL;

}
/**
*   @brief  obstacle_control_run_reset  每次避障检测启动时都要调用此函数重置参数
  * @parm   none
  * @retval none
  */
void obstacle_control_run_reset(void)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		obstacleAllControl.control[i].state = OCS_NO_CONTROL;
	}
	obstacleAllControl.x_state = OCS_NO_CONTROL;
	obstacleAllControl.y_state = OCS_NO_CONTROL;
}
#ifndef USE_SESORINTEGRATED
/**
*   @brief  main_recv_decode_zkrt_dji_guidance Guidance数据包解析处理（此函数只解析处理障碍物距离数据）
  * @parm   none
  * @retval none
  */
void main_recv_decode_zkrt_dji_guidance(void)
{
//	int i;
	uint8_t guidance_vlaue = 0;

	//在线检测
	if (GuidanceObstacleData.online_timing - TimingDelay > GUIDANCE_ONLINE_TIMEOUT)
	{
		guidance_parmdata_init();
		GuidanceObstacleData.online_timing = TimingDelay;
		ZKRT_LOG(LOG_ERROR, "Guidance UART not online!\n");
	}

	//解析处理guidance数据包
	while (usart6_rx_check() == 1)
	{
		guidance_vlaue = usart6_rx_byte();
		//   printf("  0x %x",guidance_vlaue);
		//    printf("\r\n");
		switch (guidance_v_index)
		{
		case 0:
			if (guidance_vlaue == 0xaa)
			{
				GuidanceObstacleData.online_flag = 1;
				GuidanceObstacleData.online_timing = TimingDelay;
				guidance_v_index = 1;
			}
			else
				guidance_v_index = 0;
			break;
		case 1:
			if (guidance_vlaue == 0x80)
			{
				guidance_v_index = 2;
			}
			else
				guidance_v_index = 0;
			break;
		case 13:
			if (guidance_vlaue == 0x4)
			{
				guidance_v_index = 14;
			}
			else
				guidance_v_index = 0;
			break;
		case 22:
			GuidanceObstacleData.g_distance_char[0] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 23;
			break;
		case 23:
			GuidanceObstacleData.g_distance_char[1] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			GuidanceObstacleData.g_distance_valid |= 0x0001;
			guidance_v_index = 24;
			break;
		case 24:
			GuidanceObstacleData.g_distance_char[2] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 25;
			break;
		case 25:
			GuidanceObstacleData.g_distance_char[3] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 26;
			break;
		case 26:
			GuidanceObstacleData.g_distance_char[4] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 27;
			break;
		case 27:
			GuidanceObstacleData.g_distance_char[5] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 28;
			break;
		case 28:
			GuidanceObstacleData.g_distance_char[6] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 29;
			break;
		case 29:
			GuidanceObstacleData.g_distance_char[7] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 30;
			break;
		case 30:
			GuidanceObstacleData.g_distance_char[8] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 31;
			break;
		case 31:
			GuidanceObstacleData.g_distance_char[9] = guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1 << (guidance_v_index - 22));
			guidance_v_index = 0;
			break;
		default: guidance_v_index++;
			break;
		}
	}

	if (GuidanceObstacleData.g_distance_valid == 0x03ff)	//数据有效时才装载障碍物数据包
	{
		GuidanceObstacleData.g_distance_value[0] = GuidanceObstacleData.g_distance_char[0] + ((GuidanceObstacleData.g_distance_char[1] << 8) & 0xff00);
		GuidanceObstacleData.g_distance_value[1] = GuidanceObstacleData.g_distance_char[2] + ((GuidanceObstacleData.g_distance_char[3] << 8) & 0xff00);
		GuidanceObstacleData.g_distance_value[2] = GuidanceObstacleData.g_distance_char[4] + ((GuidanceObstacleData.g_distance_char[5] << 8) & 0xff00);
		GuidanceObstacleData.g_distance_value[3] = GuidanceObstacleData.g_distance_char[6] + ((GuidanceObstacleData.g_distance_char[7] << 8) & 0xff00);
		GuidanceObstacleData.g_distance_value[4] = GuidanceObstacleData.g_distance_char[8] + ((GuidanceObstacleData.g_distance_char[9] << 8) & 0xff00);
		GuidanceObstacleData.g_distance_valid = 0;
//		for(i=0;i<5;i++)
//		{
//			printf("[%d]=%d, ",i,GuidanceObstacleData.g_distance_value[i]);
//		}
//		printf("\r\n");
	}
}
#endif
/**
*   @brief  obstacle_avoidance_handle
避障策略：
1. 只有一个方向有障碍物时，往相反方向移动避障。
2. 相邻两个方向有障碍物时，往相反的两个方向移动避障。
3. 相邻三个方向有障碍物时，往第4个方向移动避障。
4. 相反的两个方向有障碍物时，往两个方向的其中一个方向移动（距障碍物距离远的那个方向）。
5. 四个方向都有障碍物时，往障碍物距离远的那个方向移动。
  * @parm   none
  * @retval char: the direction of need move, bit0 is front, bit1 is right, bit2 is back, bit3 is left.
             (0b xxxx left back right front)
              front:  (1<<(GE_DIR_FRONT-1))
              right:  (1<<(GE_DIR_RIGHT-1))
              back :  (1<<(GE_DIR_BACK-1))
              left :  (1<<(GE_DIR_LEFT-1))
						when retval = 0x80, 飞机悬停
  */
char g_obstacle_cnt = 0; //避障触发的方向个数
char g_obstacle_move_flag = 0; //避障算法计算出来的避障移动标记
char g_obstacle_dir = 0; //障碍物方向超过阈值的标记，标记位置定义与obstacle_move_flag一样
unsigned char obstacle_avoidance_handle(void)
{
	char temp_flag;
	g_obstacle_cnt = 0; //避障触发的方向个数
//	g_obstacle_move_flag=0;  //避障算法计算出来的避障移动标记
	g_obstacle_dir = 0; //障碍物方向超过阈值的标记，标记位置定义与obstacle_move_flag一样

	if (GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_FRONT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_BACK - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_RIGHT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_LEFT - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_BACK] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_BACK - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_FRONT - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_LEFT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_RIGHT - 1));
	}

	switch (g_obstacle_cnt)
	{
	case 0:
		g_obstacle_move_flag = 0;
		break;

	case 1:
		//nothing to do
		break;

	case 2:

		if (g_obstacle_dir == 10) //0b1010: 左右相反方向处理
		{
////first method
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT])
//					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
////second method
			if (GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] - GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] > 50) //>50m 差值大于50cm才移动
				g_obstacle_move_flag = (1 << (GE_DIR_LEFT - 1));
			else if (GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] - GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > 50)
				g_obstacle_move_flag = (1 << (GE_DIR_RIGHT - 1));
			else
				g_obstacle_move_flag = 0x80;  //悬停
////third method
//				g_obstacle_move_flag = 0x80;  //悬停

		}
		else if (g_obstacle_dir == 5) //0b0101: 前后相反方向处理
		{
////first method
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] > GuidanceObstacleData.g_distance_value[GE_DIR_BACK])
//					g_obstacle_move_flag = (1<<(GE_DIR_FRONT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_BACK-1));
////second method
			if (GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] - GuidanceObstacleData.g_distance_value[GE_DIR_BACK] > 50)
				g_obstacle_move_flag = (1 << (GE_DIR_FRONT - 1));
			else if (GuidanceObstacleData.g_distance_value[GE_DIR_BACK] - GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] > 50)
				g_obstacle_move_flag = (1 << (GE_DIR_BACK - 1));
			else
				g_obstacle_move_flag = 0x80;  //悬停
////third method
//				g_obstacle_move_flag = 0x80;  //悬停

		}
		else  //相邻方向处理
		{
			g_obstacle_move_flag = ~g_obstacle_dir;
		}
		break;

	case 3:
////first method
//			g_obstacle_move_flag = (~g_obstacle_dir)&0x0f;
////second method
		g_obstacle_move_flag = 0x80;  //悬停
		break;

	case 4:
		temp_flag = arry_max_item(GuidanceObstacleData.g_distance_value + GE_DIR_FRONT, 4);
		if (GuidanceObstacleData.g_distance_value[temp_flag] < 100)
			g_obstacle_move_flag = 0x80;  //悬停
		else
			g_obstacle_move_flag = (1 << (temp_flag - 1));
		break;

	default:
		break;
	}

	return g_obstacle_move_flag;
}
/**
*   @brief  obstacle_avoidance_handle
躲避避障策略：
1. 只有一个方向有障碍物时，往相反方向移动避障。
2. 相邻两个方向有障碍物时，往相反的两个方向移动避障。
3. 相邻三个方向有障碍物时，悬停。
4. 相反的两个方向有障碍物时，悬停。
5. 四个方向都有障碍物时，悬停。
  * @parm flight_x 前后速度值，前正后负
  * @parm flight_y 左右速度值，右正左负
  * @retval char: the direction of need move, bit0 is front, bit1 is right, bit2 is back, bit3 is left.
             (0b xxxx left back right front)
              front:  (1<<(GE_DIR_FRONT-1))
              right:  (1<<(GE_DIR_RIGHT-1))
              back :  (1<<(GE_DIR_BACK-1))
              left :  (1<<(GE_DIR_LEFT-1))
						when retval = 0x80, 飞机悬停
  */
unsigned char obstacle_avoidance_self_handle(float *flight_x, float *flight_y, char *obstacle_dir)
{
	char temp_flag = 0; //避障算法计算出来的避障移动标记
	g_obstacle_cnt = 0; //避障触发的方向个数
	g_obstacle_dir = 0; //障碍物方向超过阈值的标记，标记位置定义与obstacle_move_flag一样

	if (GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] < GuidanceObstacleData.ob_distance - 50)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_FRONT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_BACK - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] < GuidanceObstacleData.ob_distance - 50)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_RIGHT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_LEFT - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_BACK] < GuidanceObstacleData.ob_distance - 50)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_BACK - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_FRONT - 1));
	}
	if (GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] < GuidanceObstacleData.ob_distance - 50)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |= (1 << (GE_DIR_LEFT - 1));
		g_obstacle_move_flag = (1 << (GE_DIR_RIGHT - 1));
	}

	temp_flag = g_obstacle_move_flag;
	switch (g_obstacle_cnt)
	{
	case 0:
		temp_flag = 0;
		g_obstacle_move_flag = 0;
		break;

	case 1:
		break;

	case 2:
		if (g_obstacle_dir == 10) //0b1010: 左右相反方向处理
		{
			temp_flag = 0x80;  //悬停
		}
		else if (g_obstacle_dir == 5) //0b0101: 前后相反方向处理
		{
			temp_flag = 0x80;  //悬停
		}
		else  //相邻方向处理
		{
			temp_flag = ~g_obstacle_dir;
		}
		break;

	case 3:
		temp_flag = 0x80;  //悬停
		break;
	case 4:
		temp_flag = 0x80;  //悬停
		break;
	default:
		break;
	}
	//vel calculate
	if (temp_flag)
	{
		if (temp_flag & (1 << (GE_DIR_FRONT - 1)))               //前后
			*flight_x = (OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else if (temp_flag & (1 << (GE_DIR_BACK - 1)))
			*flight_x = -(OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else
			*flight_x = 0;
		if (temp_flag & (1 << (GE_DIR_RIGHT - 1)))               //左右
			*flight_y = (OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else if (temp_flag & (1 << (GE_DIR_LEFT - 1)))
			*flight_y = -(OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else
			*flight_y = 0;
	}
	return temp_flag;
}
/**
*   @brief  vel_angle_checkout_in_obstacle
检测飞机当前运行速度和角度，识别是否需要紧急制动飞机。
角度>0.24；
速度>3m/s
  * @parm
  * @parm
  * @parm
	* @parm
  * @retval 1-OES避障生效，0-避障不生效
  */
char vel_angle_checkout_in_obstacle(char direction, const double *roll, const double *pitch, const float *x, const float *y)
{
	if (((*pitch <= -0.24) || (*x >= 3)) && (direction == GE_DIR_FRONT))
	{
		return 1;
	}
	else if (((*pitch >= 0.24) || (*x <= -3)) && (direction == GE_DIR_BACK))
	{
		return 1;
	}
	else if (((*roll >= 0.24) || (*y >= 3)) && (direction == GE_DIR_RIGHT))
	{
		return 1;
	}
	else if (((*roll <= -0.24) || (*y <= -3)) && (direction == GE_DIR_LEFT))
	{
		return 1;
	}
	else
		return 0;
}

/**
*   @brief  obstacle_ctrl_check_by_rc_and_distance
遥控器速度<=允许的最高速度时，OES不控制避障, 加入飞行角度做参考量
  * @parm flight_ch 飞行控制值
  * @parm RCData_ch 遥控器通道值
  * @parm distance 障碍物距离
	* @parm rad
  * @retval 1-OES安全距离避障生效，2-限速避障生效，3-限速距离内过速避障生效，0-避障不生效
  */
unsigned char obstacle_ctrl_check_by_rc_and_distance_V4(char direction, float *flight_ch, int16_t RCData_ch, unsigned short distance)
{
	char ret = 0;

	if (GuidanceObstacleData.obstacle_time_flag[direction - 1])
	{
		goto ostacleStopKeepOn;
	}

	if (distance > OBSTACLE_ENABLED_DISTANCE)
	{
		if (GuidanceObstacleData.constant_speed_time_flag[direction - 1])
		{
			goto ostacleConstantSpeedKeepOn;
		}
		return ret;
	}

	if (distance <= GuidanceObstacleData.ob_distance)
	{
		*flight_ch = 0;
		ret = 1;
	}
	else
	{
		if (vel_angle_checkout_in_obstacle(direction, &djif_status.roll, &djif_status.pitch, &djif_status.xnow, &djif_status.ynow))
		{
			*flight_ch = 0;
			GuidanceObstacleData.obstacle_time_flag[direction - 1] = 1;
			GuidanceObstacleData.obstacle_time[direction - 1] = 5; //5 seconds
			ret = 3;
		}
		else
		{
			*flight_ch = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
			GuidanceObstacleData.constant_speed_time_flag[direction - 1] = 1;
			GuidanceObstacleData.constant_speed_time[direction - 1] = 2;
			ret = 2;
		}
	}
	return ret;
ostacleConstantSpeedKeepOn:
	*flight_ch = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
	ret = 2;
	return ret;
ostacleStopKeepOn:
	*flight_ch = 0;
	ret = 3;
	return ret;
}
/**
*   @brief  obstacle_ctrl_check_by_rc_and_distance
遥控器速度<=允许的最高速度时，OES不控制避障
  * @parm flight_ch 飞行控制值
  * @parm RCData_ch 遥控器通道值
  * @parm distance 障碍物距离
  * @retval 1-OES避障生效，0-避障不生效
  */
unsigned char obstacle_ctrl_check_by_rc_and_distance(float *flight_ch, int16_t RCData_ch, unsigned short distance)
{
	char ret = 0;
//	float safe_vel;
//	float rcdata_vel;

//	if(distance >= GuidanceObstacleData.ob_distance)
	if (distance > OBSTACLE_ENABLED_DISTANCE)
		return ret;

//	if(distance <= OBSTACLE_SAFE_DISTANCE)
	if (distance <= GuidanceObstacleData.ob_distance)
	{
		*flight_ch = 0;
		ret = 1;
	}
	else
	{
//		safe_vel = (float)(OBSTACLE_SAFEH_VEL*(distance-OBSTACLE_SAFE_DISTANCE))/(OBSTACLE_ENABLED_DISTANCE-OBSTACLE_SAFE_DISTANCE);
//		rcdata_vel = (float)(RCData_ch*RC_H_VEL)/10000;
//		printf("safe_vel:%f,rcdata_vel:%f\n", safe_vel,rcdata_vel);
//		if(fabs(rcdata_vel)<=safe_vel)
//		{
//			ret =0;
//		}
//		else
		{
//			*flight_ch = safe_vel;
			*flight_ch = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
			ret = 1;
		}
	}
	return ret;
}
/**
*   @brief  obstacle_ctrl_check_by_rc_and_distance
机体速度<=允许的最高速度时，OES不控制避障
  * @parm flight_ch 飞行控制值
  * @parm RCData_ch 遥控器通道值
  * @parm distance 障碍物距离
  * @parm flight_now 飞机当前运行速度
  * @retval 1-OES避障生效，0-避障不生效
  */
unsigned char obstacle_ctrl_check_by_rc_and_distance_V2(float *flight_ch, int16_t RCData_ch, unsigned short distance, const float *flight_now)
{
	char ret = 0;
	float safe_vel;
//	float rcdata_vel;
	float nowvel = *flight_now;

//	if(distance >= GuidanceObstacleData.ob_distance)
	if (distance > OBSTACLE_ENABLED_DISTANCE)
		return ret;

	if (distance <= OBSTACLE_SAFE_DISTANCE)
	{
		*flight_ch = 0;
		ret = 1;
	}
	else
	{
		safe_vel = (float)(OBSTACLE_SAFEH_VEL * (distance - OBSTACLE_SAFE_DISTANCE)) / (OBSTACLE_ENABLED_DISTANCE - OBSTACLE_SAFE_DISTANCE);
//		rcdata_vel = (float)(RCData_ch*RC_H_VEL)/10000;
//		printf("rcdata_vel:%f,now_vel:%f,channel:%d, ch_vel_rel:%f\n", rcdata_vel,nowvel, RCData_ch, fabs(nowvel)-fabs(rcdata_vel));
		if (nowvel <= safe_vel)
		{
			ret = 0;
		}
		else
		{
			*flight_ch = safe_vel;
////			printf("safe_vel:%f,now_vel:%f,channel:%d\n", safe_vel,nowvel, RCData_ch);
			ret = 1;
		}
	}
	return ret;
}
/**
*   @brief  obstacle_ctrl_check_by_rc_and_distance
遥控器速度<=允许的最高速度时，OES不控制避障  遥控器5000上下的斜率不同设置
  * @parm flight_ch 飞行控制值
  * @parm RCData_ch 遥控器通道值
  * @parm distance 障碍物距离
  * @retval 1-OES避障生效，0-避障不生效
  */
unsigned char obstacle_ctrl_check_by_rc_and_distance_V3(float *flight_ch, int16_t RCData_ch, unsigned short distance)
{
	char ret = 0;
	float safe_vel;
	float rcdata_vel;

	if (distance > OBSTACLE_ENABLED_DISTANCE)
		return ret;

	if (distance <= OBSTACLE_SAFE_DISTANCE)
	{
		*flight_ch = 0;
		ret = 1;
	}
	else
	{
		safe_vel = (float)(OBSTACLE_SAFEH_VEL * (distance - OBSTACLE_SAFE_DISTANCE)) / (OBSTACLE_ENABLED_DISTANCE - OBSTACLE_SAFE_DISTANCE);
		if (fabs((double)RCData_ch) <= 5000)
		{
			rcdata_vel = (float)(RCData_ch * RC_H_VEL_IN5000CH) / 5000;
		}
		else
		{
			rcdata_vel = (float)(RCData_ch * RC_H_VEL) / 10000;
		}
//		printf("safe_vel:%f,rcdata_vel:%f\n", safe_vel,rcdata_vel);
		if (fabs(rcdata_vel) <= safe_vel)
		{
			ret = 0;
		}
		else
		{
			*flight_ch = safe_vel;
			ret = 1;
		}
	}
	return ret;
}
/**
*   @brief  obstacle_avoidance_handle
避障策略：根据遥控器的控制情况，OES执行辅助避障
辅助避障策略：
在避障生效距离内，当遥控器往障碍物方向飞行控制时，OES限制其速度。限制速度依靠遥控器控制值
在绝对安全距离内，当遥控器往障碍物方向飞行控制时，OES限制制止其飞行。
  * @parm flight_x 前后速度值，前正后负
  * @parm flight_y 左右速度值，右正左负
  * @parm RCData_x 前后遥控器通道值，pitch, [-10000,10000] 	Down: -10000, Up: 10000
  * @parm RCData_y 左右遥控器通道值，roll, [-10000,10000] 	Left: -10000, Right: 10000
  * @retval 1-OES避障生效，0-避障不生效
  */
unsigned char obstacle_avoidance_handle_V2(float *flight_x, float *flight_y,  int16_t RCData_x, int16_t RCData_y)
{
	char ret1 = 0, ret2 = 0, ret = 0;

	if (RCData_x > 0) //front
	{
		ret1 = obstacle_ctrl_check_by_rc_and_distance_V4(GE_DIR_FRONT, flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT]);
//		ret1 = obstacle_ctrl_check_by_rc_and_distance(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT]);
	}
	else if (RCData_x < 0) //back
	{
		ret1 = obstacle_ctrl_check_by_rc_and_distance_V4(GE_DIR_BACK, flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_BACK]);
//		ret1 = obstacle_ctrl_check_by_rc_and_distance(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_BACK]);
		*flight_x = -(*flight_x);
	}
	else
	{
		*flight_x = 0;
	}

	if (RCData_y > 0) //right
	{
		ret2 = obstacle_ctrl_check_by_rc_and_distance_V4(GE_DIR_RIGHT, flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT]);
//		ret2 = obstacle_ctrl_check_by_rc_and_distance(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT]);
	}
	else if (RCData_y < 0) //left
	{
		ret2 = obstacle_ctrl_check_by_rc_and_distance_V4(GE_DIR_LEFT, flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT]);
//		ret2 = obstacle_ctrl_check_by_rc_and_distance(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT]);
		*flight_y = -(*flight_y);
	}
	else
	{
		*flight_y = 0;
	}
	if ((ret1) || (ret2))
	{
		if (ret1)
			ret = ret1;
		else if (ret2)
			ret = ret2;
	}
	return ret;
}
/**
*   @brief  obstacle_avoidance_handle
避障策略：根据遥控器的控制情况，OES执行辅助避障
辅助避障策略：
在避障生效距离内，当遥控器往障碍物方向飞行控制时，OES限制其速度。限制速度依靠遥控器速度值与机体当前速度值
在绝对安全距离内，当遥控器往障碍物方向飞行控制时，OES限制制止其飞行。
  * @parm flight_x 前后速度值，前正后负
  * @parm flight_y 左右速度值，右正左负
  * @parm RCData_x 前后遥控器通道值，pitch, [-10000,10000] 	Down: -10000, Up: 10000
  * @parm RCData_y 左右遥控器通道值，roll, [-10000,10000] 	Left: -10000, Right: 10000
  * @parm flight_x_now 飞机机体当前速度值
  * @parm flight_y_now 飞机机体当前速度值
  * @retval 1-OES避障生效，0-避障不生效
  */
unsigned char obstacle_avoidance_handle_V3(float *flight_x, float *flight_y,  int16_t RCData_x, int16_t RCData_y, const float *flight_x_now, const float *flight_y_now)
{
	char ret1 = 0, ret2 = 0, ret = 0;
	float xnow = *flight_x_now, ynow = *flight_y_now;

	if (RCData_x > 0) //front
	{
		if (xnow < 0)
			xnow = 0;
		ret1 = obstacle_ctrl_check_by_rc_and_distance_V2(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT], &xnow);
	}
	else if (RCData_x < 0) //back
	{
		if (xnow > 0)
			xnow = 0;
		else
			xnow = fabs(xnow);
		ret1 = obstacle_ctrl_check_by_rc_and_distance_V2(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_BACK], &xnow);
		*flight_x = -(*flight_x);
	}
	else
	{
		*flight_x = 0;
	}

	if (RCData_y > 0) //right
	{
		if (ynow < 0)
			ynow = 0;
		ret2 = obstacle_ctrl_check_by_rc_and_distance_V2(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT], &ynow);
	}
	else if (RCData_y < 0) //left
	{
		if (ynow > 0)
			ynow = 0;
		else
			ynow = fabs(ynow);
		ret2 = obstacle_ctrl_check_by_rc_and_distance_V2(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT], &ynow);
		*flight_y = -(*flight_y);
	}
	else
	{
		*flight_y = 0;
	}
	if ((ret1) || (ret2))
	{
//		printf("x=%f,y=%f\n", *flight_x, *flight_y);
		ret = 1;
	}
	return ret;
}
/**
*   @brief  is_rc_goto_dir
  * @parm   dir
  * @retval tempflag 1->yes, 0-no
  */
uint8_t is_rc_goto_dir(uint8_t dir)
{
//前后遥控器通道值，pitch, [-10000,10000] 	Down: -10000, Up: 10000
//左右遥控器通道值，roll, [-10000,10000] 	Left: -10000, Right: 10000
//油门, throttle, [-10000,10000] 	Down: -10000, Up: 10000
	uint8_t tempflag = 0;
	switch (dir)
	{
	case GE_DIR_LEFT:
		if (djif_status.rc_roll < -500)
			tempflag = 1;
		break;
	case GE_DIR_RIGHT:
		if (djif_status.rc_roll > 500)
			tempflag = 1;
		break;
	case GE_DIR_BACK:
		if (djif_status.rc_pitch < -500)
			tempflag = 1;
		break;
	case GE_DIR_FRONT:
		if (djif_status.rc_pitch > 500)
			tempflag = 1;
		break;
	case GE_DIR_THROTTLE:
		if ((djif_status.rc_throttle > 500) || (djif_status.rc_throttle < -500))
			tempflag = 1;
		break;
	case GE_DIR_YAW:
		if ((djif_status.rc_yaw > 500) || (djif_status.rc_yaw < -500))
			tempflag = 1;
		break;
	default: break;
	}
	return tempflag;
}
/**
*   @brief  obstacle_handle_per_dirc
  * @parm   none
  * @retval none
  */
uint8_t obstacle_check_per_dirc(dji_flight_status *dfs, uint16_t distance, uint8_t direction)
{
	uint8_t rc_to_dir;
	uint8_t c_state = obstacleAllControl.control[direction - 1].state;
	uint8_t c_laststate = obstacleAllControl.control[direction - 1].last_state;

	rc_to_dir = is_rc_goto_dir(direction);

	if (rc_to_dir) //辅助RC控制逻辑:只在RC往障碍物方向操纵时才触发辅助逻辑
	{
		if (distance >= OBSTACLE_ENABLED_DISTANCE)
		{
			c_state = GuidanceObstacleData.constant_speed_time_flag[direction - 1];
		}
		else
		{
			if (distance < GuidanceObstacleData.ob_distance)
			{
				c_state = OCS_HOVER;
				GuidanceObstacleData.constant_speed_time_flag[direction - 1] = OCS_HOVER;
				GuidanceObstacleData.constant_speed_time[direction - 1] = 2;
			}
			else
			{
				c_state = OCS_LIMITING_VEL;
				GuidanceObstacleData.constant_speed_time_flag[direction - 1] = OCS_LIMITING_VEL;
				GuidanceObstacleData.constant_speed_time[direction - 1] = 2;
			}
		}
	}

	if (distance < OBSTACLE_RETURN_DISTANCE)
	{
		c_state = OCS_VOLUNTRAY_AVOID;
	}

	if ((distance < GuidanceObstacleData.ob_distance) && (c_laststate == OCS_VOLUNTRAY_AVOID))
	{
		c_state = OCS_VOLUNTRAY_AVOID;
	}

	if ((distance < GuidanceObstacleData.ob_distance + 50) && (c_laststate == OCS_HOVER) && (c_state == OCS_LIMITING_VEL)) //add logic for safe distance change to limit distance cause the flighter shake
	{
		c_state = OCS_HOVER;
	}

	return c_state;
}
/**
  * @brief  get_filter_ang_ob
  * @param  None
  * @retval None
  */
float get_filter_ang_ob(void)
{
	float ang = 10; //10 mean invalid
	if ((djif_status.height <= OBSTACLE_ENABLED_DISTANCE / 100) && (djif_status.height > 0.2f))
	{
		ang = asin(djif_status.height / (OBSTACLE_ENABLED_DISTANCE / 100));
		if (ang > ANGLE2GREAT_CALIBRA)
			ang -= ANGLE2GREAT_CALIBRA;
	}
	return ang;
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
////角度控制水平向后方向飞行。
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI不在线
//		return;
//	coreApi->setControl(1);
//	flightData_obstacle.y = OBSTACLE_ANG_BACK_Y;
//	flightData_obstacle.x = 0;
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
//角度控制水平向右方向飞行。
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI不在线
//		return;
//	coreApi->setControl(1);
//	flightData_obstacle.y = 0;
//	flightData_obstacle.x = OBSTACLE_ANG_RIGHT_X;
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////匀速控制水平向后方向飞行。
//void avoid_obstacle_alarm(void)
//{
////	u8 move_flag; //移动标记
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI不在线
//		return;
//	coreApi->setControl(1);
//	djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	flightData_obstacle.x = -1; //+向前 -向后
//	flightData_obstacle.y = 0; //+向右 -向左
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////匀速控制水平向右方向飞行。
//void avoid_obstacle_alarm(void)
//{
////	u8 move_flag; //移动标记
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI不在线
//		return;
//	coreApi->setControl(1);
//	djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	flightData_obstacle.x = 0; //+向前 -向后
//	flightData_obstacle.y = 1; //+向右 -向左
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////简单避障控制
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI不在线
//		return;
//
//	if(GuidanceObstacleData.online_flag ==0) //Guidance不在线
//		return;
//
//	if(djisdk_state.temp_alarmed == 1)
//		return;
//
//	if((GuidanceObstacleData.g_distance_value[GE_DIR_DOWN] < GE_ALARM_DISE_DOWN)||(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] < GE_ALARM_DISE_LEFT)
//		||(GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] < GE_ALARM_DISE_RIGHT)||(GuidanceObstacleData.g_distance_value[GE_DIR_BACK] < GE_ALARM_DISE_BACK)
//	  ||(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] < GE_ALARM_DISE_FRONT))
//	{
//		coreApi->setControl(1);
//		flight.setFlight(&flightData_obstacle);
//		ZKRT_LOG(LOG_INOTICE, "avoid_obstacle_alarm open=================\r\n")
//		djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	}
//	else
//	{
//		if(	djisdk_state.oes_fc_controled)
//		{
//			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
//			if(djisdk_state.oes_fc_controled ==0)
//			{
//				coreApi->setControl(0);
//				ZKRT_LOG(LOG_INOTICE, "oes control closed\n");
//			}
//	  }
//	}
//}
