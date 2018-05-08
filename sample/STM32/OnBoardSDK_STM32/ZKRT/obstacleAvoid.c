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
volatile uint8_t guidance_v_index = 0; //���ݰ������index������һ���ֽڣ�indexָ����һ���ֽ�����
dji_flight_status djif_status;
obstacleAllControl_st obstacleAllControl;  //�����㷨��Ҫ���ƽṹ��
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
*   @brief  obstacle_control_run_reset  ÿ�α��ϼ������ʱ��Ҫ���ô˺������ò���
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
*   @brief  main_recv_decode_zkrt_dji_guidance Guidance���ݰ����������˺���ֻ���������ϰ���������ݣ�
  * @parm   none
  * @retval none
  */
void main_recv_decode_zkrt_dji_guidance(void)
{
//	int i;
	uint8_t guidance_vlaue = 0;

	//���߼��
	if (GuidanceObstacleData.online_timing - TimingDelay > GUIDANCE_ONLINE_TIMEOUT)
	{
		guidance_parmdata_init();
		GuidanceObstacleData.online_timing = TimingDelay;
		ZKRT_LOG(LOG_ERROR, "Guidance UART not online!\n");
	}

	//��������guidance���ݰ�
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

	if (GuidanceObstacleData.g_distance_valid == 0x03ff)	//������Чʱ��װ���ϰ������ݰ�
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
���ϲ��ԣ�
1. ֻ��һ���������ϰ���ʱ�����෴�����ƶ����ϡ�
2. ���������������ϰ���ʱ�����෴�����������ƶ����ϡ�
3. ���������������ϰ���ʱ������4�������ƶ����ϡ�
4. �෴�������������ϰ���ʱ�����������������һ�������ƶ������ϰ������Զ���Ǹ����򣩡�
5. �ĸ��������ϰ���ʱ�����ϰ������Զ���Ǹ������ƶ���
  * @parm   none
  * @retval char: the direction of need move, bit0 is front, bit1 is right, bit2 is back, bit3 is left.
             (0b xxxx left back right front)
              front:  (1<<(GE_DIR_FRONT-1))
              right:  (1<<(GE_DIR_RIGHT-1))
              back :  (1<<(GE_DIR_BACK-1))
              left :  (1<<(GE_DIR_LEFT-1))
						when retval = 0x80, �ɻ���ͣ
  */
char g_obstacle_cnt = 0; //���ϴ����ķ������
char g_obstacle_move_flag = 0; //�����㷨��������ı����ƶ����
char g_obstacle_dir = 0; //�ϰ��﷽�򳬹���ֵ�ı�ǣ����λ�ö�����obstacle_move_flagһ��
unsigned char obstacle_avoidance_handle(void)
{
	char temp_flag;
	g_obstacle_cnt = 0; //���ϴ����ķ������
//	g_obstacle_move_flag=0;  //�����㷨��������ı����ƶ����
	g_obstacle_dir = 0; //�ϰ��﷽�򳬹���ֵ�ı�ǣ����λ�ö�����obstacle_move_flagһ��

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

		if (g_obstacle_dir == 10) //0b1010: �����෴������
		{
////first method
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT])
//					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
////second method
			if (GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] - GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] > 50) //>50m ��ֵ����50cm���ƶ�
				g_obstacle_move_flag = (1 << (GE_DIR_LEFT - 1));
			else if (GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] - GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > 50)
				g_obstacle_move_flag = (1 << (GE_DIR_RIGHT - 1));
			else
				g_obstacle_move_flag = 0x80;  //��ͣ
////third method
//				g_obstacle_move_flag = 0x80;  //��ͣ

		}
		else if (g_obstacle_dir == 5) //0b0101: ǰ���෴������
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
				g_obstacle_move_flag = 0x80;  //��ͣ
////third method
//				g_obstacle_move_flag = 0x80;  //��ͣ

		}
		else  //���ڷ�����
		{
			g_obstacle_move_flag = ~g_obstacle_dir;
		}
		break;

	case 3:
////first method
//			g_obstacle_move_flag = (~g_obstacle_dir)&0x0f;
////second method
		g_obstacle_move_flag = 0x80;  //��ͣ
		break;

	case 4:
		temp_flag = arry_max_item(GuidanceObstacleData.g_distance_value + GE_DIR_FRONT, 4);
		if (GuidanceObstacleData.g_distance_value[temp_flag] < 100)
			g_obstacle_move_flag = 0x80;  //��ͣ
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
��ܱ��ϲ��ԣ�
1. ֻ��һ���������ϰ���ʱ�����෴�����ƶ����ϡ�
2. ���������������ϰ���ʱ�����෴�����������ƶ����ϡ�
3. ���������������ϰ���ʱ����ͣ��
4. �෴�������������ϰ���ʱ����ͣ��
5. �ĸ��������ϰ���ʱ����ͣ��
  * @parm flight_x ǰ���ٶ�ֵ��ǰ����
  * @parm flight_y �����ٶ�ֵ��������
  * @retval char: the direction of need move, bit0 is front, bit1 is right, bit2 is back, bit3 is left.
             (0b xxxx left back right front)
              front:  (1<<(GE_DIR_FRONT-1))
              right:  (1<<(GE_DIR_RIGHT-1))
              back :  (1<<(GE_DIR_BACK-1))
              left :  (1<<(GE_DIR_LEFT-1))
						when retval = 0x80, �ɻ���ͣ
  */
unsigned char obstacle_avoidance_self_handle(float *flight_x, float *flight_y, char *obstacle_dir)
{
	char temp_flag = 0; //�����㷨��������ı����ƶ����
	g_obstacle_cnt = 0; //���ϴ����ķ������
	g_obstacle_dir = 0; //�ϰ��﷽�򳬹���ֵ�ı�ǣ����λ�ö�����obstacle_move_flagһ��

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
		if (g_obstacle_dir == 10) //0b1010: �����෴������
		{
			temp_flag = 0x80;  //��ͣ
		}
		else if (g_obstacle_dir == 5) //0b0101: ǰ���෴������
		{
			temp_flag = 0x80;  //��ͣ
		}
		else  //���ڷ�����
		{
			temp_flag = ~g_obstacle_dir;
		}
		break;

	case 3:
		temp_flag = 0x80;  //��ͣ
		break;
	case 4:
		temp_flag = 0x80;  //��ͣ
		break;
	default:
		break;
	}
	//vel calculate
	if (temp_flag)
	{
		if (temp_flag & (1 << (GE_DIR_FRONT - 1)))               //ǰ��
			*flight_x = (OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else if (temp_flag & (1 << (GE_DIR_BACK - 1)))
			*flight_x = -(OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity));
		else
			*flight_x = 0;
		if (temp_flag & (1 << (GE_DIR_RIGHT - 1)))               //����
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
���ɻ���ǰ�����ٶȺͽǶȣ�ʶ���Ƿ���Ҫ�����ƶ��ɻ���
�Ƕ�>0.24��
�ٶ�>3m/s
  * @parm
  * @parm
  * @parm
	* @parm
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
ң�����ٶ�<=���������ٶ�ʱ��OES�����Ʊ���, ������нǶ����ο���
  * @parm flight_ch ���п���ֵ
  * @parm RCData_ch ң����ͨ��ֵ
  * @parm distance �ϰ������
	* @parm rad
  * @retval 1-OES��ȫ���������Ч��2-���ٱ�����Ч��3-���پ����ڹ��ٱ�����Ч��0-���ϲ���Ч
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
ң�����ٶ�<=���������ٶ�ʱ��OES�����Ʊ���
  * @parm flight_ch ���п���ֵ
  * @parm RCData_ch ң����ͨ��ֵ
  * @parm distance �ϰ������
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
�����ٶ�<=���������ٶ�ʱ��OES�����Ʊ���
  * @parm flight_ch ���п���ֵ
  * @parm RCData_ch ң����ͨ��ֵ
  * @parm distance �ϰ������
  * @parm flight_now �ɻ���ǰ�����ٶ�
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
ң�����ٶ�<=���������ٶ�ʱ��OES�����Ʊ���  ң����5000���µ�б�ʲ�ͬ����
  * @parm flight_ch ���п���ֵ
  * @parm RCData_ch ң����ͨ��ֵ
  * @parm distance �ϰ������
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
���ϲ��ԣ�����ң�����Ŀ��������OESִ�и�������
�������ϲ��ԣ�
�ڱ�����Ч�����ڣ���ң�������ϰ��﷽����п���ʱ��OES�������ٶȡ������ٶ�����ң��������ֵ
�ھ��԰�ȫ�����ڣ���ң�������ϰ��﷽����п���ʱ��OES������ֹ����С�
  * @parm flight_x ǰ���ٶ�ֵ��ǰ����
  * @parm flight_y �����ٶ�ֵ��������
  * @parm RCData_x ǰ��ң����ͨ��ֵ��pitch, [-10000,10000] 	Down: -10000, Up: 10000
  * @parm RCData_y ����ң����ͨ��ֵ��roll, [-10000,10000] 	Left: -10000, Right: 10000
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
���ϲ��ԣ�����ң�����Ŀ��������OESִ�и�������
�������ϲ��ԣ�
�ڱ�����Ч�����ڣ���ң�������ϰ��﷽����п���ʱ��OES�������ٶȡ������ٶ�����ң�����ٶ�ֵ����嵱ǰ�ٶ�ֵ
�ھ��԰�ȫ�����ڣ���ң�������ϰ��﷽����п���ʱ��OES������ֹ����С�
  * @parm flight_x ǰ���ٶ�ֵ��ǰ����
  * @parm flight_y �����ٶ�ֵ��������
  * @parm RCData_x ǰ��ң����ͨ��ֵ��pitch, [-10000,10000] 	Down: -10000, Up: 10000
  * @parm RCData_y ����ң����ͨ��ֵ��roll, [-10000,10000] 	Left: -10000, Right: 10000
  * @parm flight_x_now �ɻ����嵱ǰ�ٶ�ֵ
  * @parm flight_y_now �ɻ����嵱ǰ�ٶ�ֵ
  * @retval 1-OES������Ч��0-���ϲ���Ч
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
//ǰ��ң����ͨ��ֵ��pitch, [-10000,10000] 	Down: -10000, Up: 10000
//����ң����ͨ��ֵ��roll, [-10000,10000] 	Left: -10000, Right: 10000
//����, throttle, [-10000,10000] 	Down: -10000, Up: 10000
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

	if (rc_to_dir) //����RC�����߼�:ֻ��RC���ϰ��﷽�����ʱ�Ŵ��������߼�
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
////�Ƕȿ���ˮƽ�������С�
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
//		return;
//	coreApi->setControl(1);
//	flightData_obstacle.y = OBSTACLE_ANG_BACK_Y;
//	flightData_obstacle.x = 0;
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
//�Ƕȿ���ˮƽ���ҷ�����С�
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
//		return;
//	coreApi->setControl(1);
//	flightData_obstacle.y = 0;
//	flightData_obstacle.x = OBSTACLE_ANG_RIGHT_X;
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////���ٿ���ˮƽ�������С�
//void avoid_obstacle_alarm(void)
//{
////	u8 move_flag; //�ƶ����
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
//		return;
//	coreApi->setControl(1);
//	djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	flightData_obstacle.x = -1; //+��ǰ -���
//	flightData_obstacle.y = 0; //+���� -����
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////���ٿ���ˮƽ���ҷ�����С�
//void avoid_obstacle_alarm(void)
//{
////	u8 move_flag; //�ƶ����
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
//		return;
//	coreApi->setControl(1);
//	djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	flightData_obstacle.x = 0; //+��ǰ -���
//	flightData_obstacle.y = 1; //+���� -����
//	printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
//	flight.setFlight(&flightData_obstacle);
//	ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
//}
////�򵥱��Ͽ���
//void avoid_obstacle_alarm(void)
//{
//	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
//		return;
//
//	if(GuidanceObstacleData.online_flag ==0) //Guidance������
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
