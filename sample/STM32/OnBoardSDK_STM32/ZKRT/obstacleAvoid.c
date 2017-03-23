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

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
obstacleData_st GuidanceObstacleData = {{0}, {0}, 0, 0, 0xffffffff};
volatile uint8_t guidance_v_index=0; //数据包解包的index，解完一个字节，index指向下一个字节数据

/* Private functions ---------------------------------------------------------*/

/**
*   @brief  guidance_parmdata_init
  * @parm   none
  * @retval none
  */
static void guidance_parmdata_init(void)
{
	memset(&GuidanceObstacleData, 0x00, sizeof(obstacleData_st));
	GuidanceObstacleData.g_distance_value[0] = 2000;
	GuidanceObstacleData.g_distance_value[1] = 2000;
	GuidanceObstacleData.g_distance_value[2] = 2000;
	GuidanceObstacleData.g_distance_value[3] = 2000;
	GuidanceObstacleData.g_distance_value[4] = 2000;
	GuidanceObstacleData.online_timing = 0xffffffff;
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

/**
*   @brief  main_recv_decode_zkrt_dji_guidance Guidance数据包解析处理（此函数只解析处理障碍物距离数据）
  * @parm   none
  * @retval none
  */
void main_recv_decode_zkrt_dji_guidance(void)
{
//	int i;
  uint8_t guidance_vlaue=0;
	
	//在线检测
	if(GuidanceObstacleData.online_timing - TimingDelay > GUIDANCE_ONLINE_TIMEOUT)
	{
		guidance_parmdata_init();
		GuidanceObstacleData.online_timing = TimingDelay;
		ZKRT_LOG(LOG_ERROR, "Guidance UART not online!\n");
	}
	
	//解析处理guidance数据包
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
//					delay_ms(1);
//					printf("11111111111\r\n");
				}
				else
					guidance_v_index=0;	
			break;
			case 1:
				if (guidance_vlaue== 0x80)
				{
					guidance_v_index=2;
//					delay_ms(1);
//					printf("2222222222222\r\n");
				}
				else
					guidance_v_index=0;
			break;
			case 13:
				if (guidance_vlaue== 0x4)
				{
					guidance_v_index=14;
//					delay_ms(1);
//					printf("13131313\r\n");
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
	
	if(GuidanceObstacleData.g_distance_valid ==0x03ff)	//数据有效时才装载障碍物数据包
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
char g_obstacle_cnt=0;  //避障触发的方向个数
char g_obstacle_move_flag=0;  //避障算法计算出来的避障移动标记
char g_obstacle_dir=0; //障碍物方向超过阈值的标记，标记位置定义与obstacle_move_flag一样
unsigned char obstacle_avoidance_handle(void)
{
	char temp_flag;
	g_obstacle_cnt=0;  //避障触发的方向个数
//	g_obstacle_move_flag=0;  //避障算法计算出来的避障移动标记
	g_obstacle_dir=0; //障碍物方向超过阈值的标记，标记位置定义与obstacle_move_flag一样

	if(GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] < GE_ALARM_DISE_FRONT)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_FRONT-1));
		g_obstacle_move_flag = (1<<(GE_DIR_BACK-1));
	}
	if(GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] < GE_ALARM_DISE_RIGHT)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_RIGHT-1));
		g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
	}
	if(GuidanceObstacleData.g_distance_value[GE_DIR_BACK] < GE_ALARM_DISE_BACK)
	{
		g_obstacle_cnt++;
		g_obstacle_dir |=(1<<(GE_DIR_BACK-1));
		g_obstacle_move_flag = (1<<(GE_DIR_FRONT-1));
	}	
	if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] < GE_ALARM_DISE_LEFT)
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

		  if(g_obstacle_dir==10) //0b1010: 左右相反方向处理
			{
////first method				
//				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT])
//					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
//				else
//					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
////second method							
				if(GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] - GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] > 50) //>50m 差值大于50cm才移动
					g_obstacle_move_flag = (1<<(GE_DIR_LEFT-1));
				else if(GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] - GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] > 50) 
					g_obstacle_move_flag = (1<<(GE_DIR_RIGHT-1));
				else 
					g_obstacle_move_flag = 0x80;  //悬停
////third method				
//				g_obstacle_move_flag = 0x80;  //悬停				
				
			}	
			else if(g_obstacle_dir == 5) //0b0101: 前后相反方向处理
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
//			g_obstacle_move_flag = (~g_obstacle_dir)&0x0f; //zkrt_todo: 待优化 
////second method	
			g_obstacle_move_flag = 0x80;  //悬停
			break;
		
		case 4:
			temp_flag = arry_max_item(GuidanceObstacleData.g_distance_value+GE_DIR_FRONT, 4);
	    if(GuidanceObstacleData.g_distance_value[temp_flag] <100)
				g_obstacle_move_flag = 0x80;  //悬停
			else
				g_obstacle_move_flag = (1<<(temp_flag-1));
			break;
		
		default:
			break;
	}	
	
	return g_obstacle_move_flag;
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
