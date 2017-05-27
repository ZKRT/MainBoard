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
//obstacleData_st GuidanceObstacleData = {{0}, {0}, 0, 0, 0xffffffff, 1, OBSTACLE_ALARM_DISTANCE, OBSTACLE_AVOID_VEL};
obstacleData_st GuidanceObstacleData;
volatile uint8_t guidance_v_index=0; //���ݰ������index������һ���ֽڣ�indexָ����һ���ֽ�����

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
//	//zkrt_debug
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
//	GuidanceObstacleData.ob_velocity = OBSTACLE_AVOID_VEL;
}
/**
*   @brief  guidance_init
  * @parm   none
  * @retval none
  */
void guidance_init(void)
{
	guidance_parmdata_init();
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
  uint8_t guidance_vlaue=0;
	
	//���߼��
	if(GuidanceObstacleData.online_timing - TimingDelay > GUIDANCE_ONLINE_TIMEOUT)
	{
		guidance_parmdata_init();
		GuidanceObstacleData.online_timing = TimingDelay;
		ZKRT_LOG(LOG_ERROR, "Guidance UART not online!\n");
	}
	
	//��������guidance���ݰ�
	while (usart6_rx_check() == 1)
	{
		guidance_vlaue= usart6_rx_byte();
		//   printf("  0x %x",guidance_vlaue);
		//    printf("\r\n");
		switch (guidance_v_index)
		{
			case 0:
				if (guidance_vlaue== 0xaa)
				{
					GuidanceObstacleData.online_flag = 1;
					GuidanceObstacleData.online_timing = TimingDelay;
					guidance_v_index=1;
				}
				else
					guidance_v_index=0;	
			break;
			case 1:
				if (guidance_vlaue== 0x80)
				{
					guidance_v_index=2;
				}
				else
					guidance_v_index=0;
			break;
			case 13:
				if (guidance_vlaue== 0x4)
				{
					guidance_v_index=14;
				}
				else
					guidance_v_index=0;
			break;
			case 22:
				GuidanceObstacleData.g_distance_char[0]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=23;
			break;
			case 23:
				GuidanceObstacleData.g_distance_char[1]=guidance_vlaue;
			GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
			  GuidanceObstacleData.g_distance_valid |= 0x0001; 
				guidance_v_index=24;
			break;
			case 24:
				GuidanceObstacleData.g_distance_char[2]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=25;
			break;
			case 25:
				GuidanceObstacleData.g_distance_char[3]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=26;
			break;
			case 26:
				GuidanceObstacleData.g_distance_char[4]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=27;
			break;
			case 27:
				GuidanceObstacleData.g_distance_char[5]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=28;
			break;
			case 28:
				GuidanceObstacleData.g_distance_char[6]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=29;
			break;
			case 29:
				GuidanceObstacleData.g_distance_char[7]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=30;
			break;
			case 30:
				GuidanceObstacleData.g_distance_char[8]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=31;
			break;
			case 31:
				GuidanceObstacleData.g_distance_char[9]=guidance_vlaue;
				GuidanceObstacleData.g_distance_valid |= (1<<(guidance_v_index-22));
				guidance_v_index=0;
			break;
			default:guidance_v_index++;
			break;
		}
	}
	
	if(GuidanceObstacleData.g_distance_valid ==0x03ff)	//������Чʱ��װ���ϰ������ݰ�
	{
		GuidanceObstacleData.g_distance_value[0]=GuidanceObstacleData.g_distance_char[0]+((GuidanceObstacleData.g_distance_char[1]<<8)&0xff00);
		GuidanceObstacleData.g_distance_value[1]=GuidanceObstacleData.g_distance_char[2]+((GuidanceObstacleData.g_distance_char[3]<<8)&0xff00);
		GuidanceObstacleData.g_distance_value[2]=GuidanceObstacleData.g_distance_char[4]+((GuidanceObstacleData.g_distance_char[5]<<8)&0xff00);
		GuidanceObstacleData.g_distance_value[3]=GuidanceObstacleData.g_distance_char[6]+((GuidanceObstacleData.g_distance_char[7]<<8)&0xff00);
		GuidanceObstacleData.g_distance_value[4]=GuidanceObstacleData.g_distance_char[8]+((GuidanceObstacleData.g_distance_char[9]<<8)&0xff00);	
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
char g_obstacle_cnt=0;  //���ϴ����ķ������
char g_obstacle_move_flag=0;  //�����㷨��������ı����ƶ����
char g_obstacle_dir=0; //�ϰ��﷽�򳬹���ֵ�ı�ǣ����λ�ö�����obstacle_move_flagһ��
unsigned char obstacle_avoidance_handle(void)
{
	char temp_flag;
	g_obstacle_cnt=0;  //���ϴ����ķ������
//	g_obstacle_move_flag=0;  //�����㷨��������ı����ƶ����
	g_obstacle_dir=0; //�ϰ��﷽�򳬹���ֵ�ı�ǣ����λ�ö�����obstacle_move_flagһ��

	if(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_FRONT-1));
		g_obstacle_move_flag = (1<<(GE_DIR_BACK-1));
	}
	if(GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_RIGHT-1));
		g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
	}
	if(GuidanceObstacleData.g_distance_value[GE_DIR_BACK] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_BACK-1));
		g_obstacle_move_flag = (1<<(GE_DIR_FRONT-1));
	}	
	if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] < GuidanceObstacleData.ob_distance)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_LEFT-1));
		g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
	}
	
	switch(g_obstacle_cnt)
	{
		case 0:
			g_obstacle_move_flag = 0;
			break;
		
		case 1:
			//nothing to do
			break;
		
		case 2:

		  if(g_obstacle_dir==10) //0b1010: �����෴������
			{
////first method				
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT])
//					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
////second method							
				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] - GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] > 50) //>50m ��ֵ����50cm���ƶ�
					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
				else if(GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] - GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > 50) 
					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
				else 
					g_obstacle_move_flag = 0x80;  //��ͣ
////third method				
//				g_obstacle_move_flag = 0x80;  //��ͣ				
				
			}	
			else if(g_obstacle_dir == 5) //0b0101: ǰ���෴������
			{
////first method
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] > GuidanceObstacleData.g_distance_value[GE_DIR_BACK])
//					g_obstacle_move_flag = (1<<(GE_DIR_FRONT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_BACK-1));	
////second method				
				if(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] - GuidanceObstacleData.g_distance_value[GE_DIR_BACK] > 50) 
					g_obstacle_move_flag = (1<<(GE_DIR_FRONT-1));
				else if(GuidanceObstacleData.g_distance_value[GE_DIR_BACK] - GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] > 50) 
					g_obstacle_move_flag = (1<<(GE_DIR_BACK-1));
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
//			g_obstacle_move_flag = (~g_obstacle_dir)&0x0f; //zkrt_todo: ���Ż� 
////second method	
			g_obstacle_move_flag = 0x80;  //��ͣ
			break;
		
		case 4:
			temp_flag = arry_max_item(GuidanceObstacleData.g_distance_value+GE_DIR_FRONT, 4);
	    if(GuidanceObstacleData.g_distance_value[temp_flag] <100)
				g_obstacle_move_flag = 0x80;  //��ͣ
			else
				g_obstacle_move_flag = (1<<(temp_flag-1));
			break;
		
		default:
			break;
	}	
	
	return g_obstacle_move_flag;
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
	char ret =0;
	float safe_vel;
	float rcdata_vel;
	
//	if(distance >= GuidanceObstacleData.ob_distance)
	if(distance > OBSTACLE_ENABLED_DISTANCE)
		return ret;
	
	if(distance <= OBSTACLE_SAFE_DISTANCE)
	{
		*flight_ch = 0;
		ret =1;
	}
	else
	{	
		safe_vel = (float)(OBSTACLE_SAFEH_VEL*(distance-OBSTACLE_SAFE_DISTANCE))/(OBSTACLE_ENABLED_DISTANCE-OBSTACLE_SAFE_DISTANCE);
		rcdata_vel = (float)(RCData_ch*RC_H_VEL)/10000;
//		printf("safe_vel:%f,rcdata_vel:%f\n", safe_vel,rcdata_vel);  //zkrt_debug
		if(fabs(rcdata_vel)<=safe_vel)
		{
			ret =0;
		}
		else
		{
			*flight_ch = safe_vel;
			ret =1;
		}
	}
	return ret;
}
/**
*   @brief  obstacle_avoidance_handle
���ϲ��ԣ�����ң�����Ŀ��������OESִ�и�������
�������ϲ��ԣ�
�ڱ�����Ч�����ڣ���ң�������ϰ��﷽����п���ʱ��OES�������ٶȡ�
�ھ��԰�ȫ�����ڣ���ң�������ϰ��﷽����п���ʱ��OES������ֹ����С�
  * @parm flight_x ǰ���ٶ�ֵ��ǰ����
  * @parm flight_y �����ٶ�ֵ��������
  * @parm RCData_x ǰ��ң����ͨ��ֵ��pitch, [-10000,10000] 	Down: -10000, Up: 10000
  * @parm RCData_y ����ң����ͨ��ֵ��roll, [-10000,10000] 	Left: -10000, Right: 10000
  * @retval 1-OES������Ч��0-���ϲ���Ч
  */
unsigned char obstacle_avoidance_handle_V2(float *flight_x, float *flight_y,  int16_t RCData_x, int16_t RCData_y)
{
	char ret1=0, ret2=0, ret=0;
	
	if(RCData_x>0) //front
	{
		ret1 = obstacle_ctrl_check_by_rc_and_distance(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT]);
	}
	else if(RCData_x<0) //back
	{
		ret1 = obstacle_ctrl_check_by_rc_and_distance(flight_x, RCData_x, GuidanceObstacleData.g_distance_value[GE_DIR_BACK]);
		*flight_x = -(*flight_x);
	}
	else
	{
		*flight_x = 0; 
	}
	
	if(RCData_y>0) //right
	{
		ret2 = obstacle_ctrl_check_by_rc_and_distance(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT]);
	}
	else if(RCData_y<0) //left
	{
		ret2 = obstacle_ctrl_check_by_rc_and_distance(flight_y, RCData_y, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT]);
		*flight_y = -(*flight_y);
	}
	else
	{
		*flight_y = 0;
	}
  if((ret1)||(ret2))
	{
//		printf("x=%f,y=%f\n", *flight_x, *flight_y);
		ret = 1;
	}
	return ret;
}
/**
*   @brief  guidance_init
  * @parm   none
  * @retval none
  */
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
