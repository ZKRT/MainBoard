/**
  ******************************************************************************
  * @file    main.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    17-January-2017
  * @brief   
  *          + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"
#ifdef __cplusplus
extern "C"
{
#include "stm32f4xx_adc.h"
#include "sys.h"
#include "adc.h"
#include "usart.h"
#include "zkrt.h"
#include "djiapp.h"
#include "flash.h"
#include "bat.h"
#include "mobileDataHandle.h"
#include "led.h"
#include "ostmr.h"
#include "osqtmr.h"	
#ifdef USE_LWIP_FUN		
#include "lwip_comm.h"
#include "lwipapp.h"
#include "usb_usr_process.h"
#endif	
#ifdef HWTEST_FUN	
#include "hwTestHandle.h"
#endif	
#include "versionzkrt.h"
#ifdef USE_OBSTACLE_AVOID_FUN
#include "obstacleAvoid.h"
#endif
#include "undercarriageCtrl.h"
#include "heartBeatHandle.h"
#ifdef USE_SESORINTEGRATED
#include "sersorIntegratedHandle.h"
#endif
#include "iwatchdog.h"
}
#endif //__cplusplus

/*-----------------------NEW DJI_LIB VARIABLE-----------------------------*/
using namespace DJI::OSDK;

bool           threadSupport = false;
bool           isFrame       = false;
RecvContainer  receivedFrame;
RecvContainer* rFrame  = &receivedFrame;
Vehicle        vehicle = Vehicle(threadSupport);
Vehicle*       v       = &vehicle;

extern TerminalCommand myTerminal; //add by yanly

Control::CtrlData flightData_zkrtctrl(Control::VERTICAL_VELOCITY | Control::HORIZONTAL_VELOCITY | Control::YAW_RATE | Control::HORIZONTAL_BODY, 0, 0, 0, 0);
/*----------ZKRT VARIABLE----------*/

/* Private function prototypes -----------------------------------------------*/
void mobile_heardbeat_packet_control(void);
void tempture_flight_control(void);
void avoid_temp_alarm(void);
#ifdef USE_OBSTACLE_AVOID_FUN
void avoid_obstacle_alarm(void);
void avoid_obstacle_alarm_V2(void);
void avoid_obstacle_alarm_V3(void);
void sys_ctrl_timetask(void);
#endif
/**
  * @brief  main. ������
  * @param  None
  * @retval None
  */
int main()
{
  BSPinit();
	delay_nms(1000);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n"); 
	printf("PRODUCT_NAME: %s\r\nPRODUCT_ID: %s\r\nPRODUCT_VERSION: %s\r\nPRODUCT_TIME: %s %s\r\n",PRODUCT_NAME,PRODUCT_ID,PRODUCT_VERSION,__DATE__,__TIME__);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n"); 
#ifdef USE_DJI_FUN	
	if(dji_init()<0)
		return -1;
#endif 
#ifdef USE_LWIP_FUN	
	lwip_prcs_init();
#endif	
#ifdef USE_OBSTACLE_AVOID_FUN	
	guidance_init();
#endif
#ifdef USE_UNDERCARRIAGE_FUN	
	undercarriage_init();
#endif	
	heartbeat_parm_init();   //put at last
	t_ostmr_insertTask(sys_ctrl_timetask, 1000, OSTMR_PERIODIC);  //1000
  while (1)
  {
#ifdef USE_DJI_FUN			
		dji_process();                        //��SDK����
#endif		
		mobile_data_process();                //�����յ���mobile͸�����ݽ��н�������
    main_zkrt_dji_recv();		              //����ģ��ͨ��CAN�������ݣ�����������Ͷ������䵽���������������λ��һ
		tempture_flight_control();            //�¶ȳ����������������ݹ���
#ifdef USE_OBSTACLE_AVOID_FUN	
#ifdef USE_SESORINTEGRATED		
		app_sersor_integrated_prcs();         //���ɰ����ݴ���
#else
		main_recv_decode_zkrt_dji_guidance(); //Guidance���ݰ���������
#endif		
#ifdef USE_UART3_TEST_FUN		
		myTerminal.terminalCommandHandler(v);
#endif		
//		avoid_obstacle_alarm();               //���ϼ��		
//		avoid_obstacle_alarm_V2();  		
    avoid_obstacle_alarm_V3();			
#endif
		mobile_heardbeat_packet_control();    //���Ӷ�ʱ����������������վ
		led_process();                        //LED����
		stmflash_process();                   //�û�������Ϣ����
#ifdef USE_LWIP_FUN			
		lwip_prcs();													//��������
#endif
#ifdef USE_USB_FUN			
		usb_user_prcs();                      //USB����
#endif    
#ifdef USE_UNDERCARRIAGE_FUN
    undercarriage_process();              //����ܴ���
#endif
	  IWDG_Feed();
  }
}
/**
  * @brief  tempture_flight_control. �¶ȿ��Ʒ���+�¶���Ϣ�������������+������ݼ�����
  * @param  None
  * @retval None
  */
void tempture_flight_control(void)
{
	if (_160_read_flag - TimingDelay >= 160)
	{
		_160_read_flag = TimingDelay;
#ifndef USE_SESORINTEGRATED		
		ADC_SoftwareStartConv(ADC1); /*����ADC*/	
#endif		
//		ZKRT_LOG(LOG_NOTICE, "ADC_SoftwareStartConv!\r\n");	
		if((_read_count%2) == 0)									
		{
#ifndef USE_SESORINTEGRATED			
			zkrt_dji_read_heart_tempture();  /*��ȡ�¶ȴ���������*/
#endif			
			avoid_temp_alarm();     //���¼��
			
			if (MAVLINK_TX_INIT_VAL - TimingDelay >= 4000)	
			{					
				dji_zkrt_read_heart_tempture_check();	/*���¶ȴ�����������䵽���������棬���ж������Ƿ�����*/	
			}
		}
		if ((_read_count%10) == 0)								
		{
			dji_bat_value_roll(); /*��ȡ���ܵ�����ݣ�����䵽���ܵ������������*/
			dji_zkrt_read_heart_vol(); /*��ȡ���ص�ѹ����ֵ*/

			if (MAVLINK_TX_INIT_VAL - TimingDelay >= 4000)	
			{
				dji_zkrt_read_heart_vol_check();	/*�Ե�ǰ���ص�ѹ����ֵ�����жϣ���������Ӧ�������������䵽������*/					
			}
		}
		_read_count++;
	}	
}
/**
  * @brief  mobile_heardbeat_packet_control. ���������վ���ֻ���������������ʱ���ͣ�+ ģ�����߱����λ
  * @param  None
  * @retval None
  */
void mobile_heardbeat_packet_control(void)
{
	if (MAVLINK_TX_INIT_VAL - TimingDelay >= 5000)  //5s��ʱ��֮��һֱ���������߼�
	{
		if ((mavlink_send_flag-TimingDelay) >= 800)
		{
			mavlink_send_flag = TimingDelay;
			#if 0   //zkrt_notice: send heartbeat1 at another place.
			switch (mavlink_type_flag_dji)
			{
				case 0:
					mavlink_type_flag_dji = 1;
					dji_zkrt_read_heart_ack(); /*������յ����ݰ������ҷ��ͳ���*/
					coreApi->sendPoll();
					break;
				case 1:
					mavlink_type_flag_dji = 0;
				  dji_bat_value_send();  /*�������ܵ�����ݣ��ܹ�32���ֽ�*/    //????Ϊʲôû�з�װzkrt packet��ʽ //modify by yanly
					coreApi->sendPoll();
					break;
				default:
					break;
			}
			#endif
		}
	}	
	
/*����can�յ���ģ����Ϣ���ж��ĸ�ģ�����ߺ󣬽������������߱����λ*/	 //zkrt_add_new_module
	if ((posion_recv_flag-TimingDelay)  >= 5000)				
	{
		memset((void *)(msg_smartbat_dji_buffer+10), 0, 13);
		msg_smartbat_dji_buffer[23] &= 0XEF;					
	}
	if ((throw_recv_flag-TimingDelay)  >= 5000)
	{
		msg_smartbat_dji_buffer[23] &= 0XDF;			
	}
	if ((camera_recv_flag-TimingDelay) >= 5000)
	{
		msg_smartbat_dji_buffer[23] &= 0XF7;	
	}
	if ((irradiate_recv_flag-TimingDelay) >= 5000)
	{
		msg_smartbat_dji_buffer[24] &= 0XFE;			
	}	
	if ((phone_recv_flag-TimingDelay) >= 5000)
	{
		msg_smartbat_dji_buffer[24] &= 0XFD;
	}
	if ((threemodeling_recv_flag-TimingDelay) >= 5000)  //��1��ʼ�㣬��12λ����Device_Status�ĵڶ����ֽڵĵ�4λ,����0XF7���롣
	{
		msg_smartbat_dji_buffer[24] &= 0XF7;
	}	
	if ((multicamera_recv_flag-TimingDelay) >= 5000)   
	{
		msg_smartbat_dji_buffer[24] &= 0XEF;  //0b: 11101111
	}	
	
	
/*------------------------------------------------------------------*/	
}
/**
  * @brief  ���¿���
  * @param  None
  * @retval None
  */
void avoid_temp_alarm(void)
{
	u8 status1_t0;
	u8 status2_t1;
	status1_t0 = msg_smartbat_dji_buffer[0];
	status2_t1 = msg_smartbat_dji_buffer[3];
	
	if(djisdk_state.run_status !=avtivated_ok_djirs)
		return;

	if((status1_t0 == TEMP_OVER_HIGH)||(status2_t1 == TEMP_OVER_HIGH))
	{
		djisdk_state.temp_alarmed = 1;
		flightData_zkrtctrl.z = 1;  //1m/s
    djisdk_state.oes_fc_controled |= 1<< fc_tempctrl_b;
		ZKRT_LOG(LOG_NOTICE, "avoid_temp_alarm open======================================\r\n");		
	}
	else
	{
		djisdk_state.temp_alarmed = 0;
		if(flightData_zkrtctrl.z)
			flightData_zkrtctrl.z = 0;
	  if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_tempctrl_b);
	}
}
#ifdef USE_OBSTACLE_AVOID_FUN
/**
*   @brief  avoid_obstacle_alarm ���Ͽ���
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm(void)
{
	u8 move_flag; //�ƶ����
	
	if(djisdk_state.run_status !=avtivated_ok_djirs) //OESû����
		return;
	
	if(GuidanceObstacleData.online_flag ==0) //Guidance������
		return;
	
	if(GuidanceObstacleData.ob_enabled ==0) //���ϲ���Ч
	{
		if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
		return;
	}
	
	move_flag = obstacle_avoidance_handle();
	if(move_flag)
	{
		djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
	
#ifdef OBSTACLE_VEL_MODE		
		if(move_flag &(1<<(GE_DIR_FRONT-1)))                     //ǰ��
		  flightData_zkrtctrl.x = OBSTACLE_VEL_FORWORD_X(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.x = OBSTACLE_VEL_FORWORD_X;
		else if(move_flag &(1<<(GE_DIR_BACK-1)))
			flightData_zkrtctrl.x = OBSTACLE_VEL_BACK_X(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.x = OBSTACLE_VEL_BACK_X;
		else
			flightData_zkrtctrl.x = 0;
		
		if(move_flag &(1<<(GE_DIR_RIGHT-1)))                     //����  
			flightData_zkrtctrl.y = OBSTACLE_VEL_RIGHT_Y(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.y = OBSTACLE_VEL_RIGHT_Y;         
		else if(move_flag &(1<<(GE_DIR_LEFT-1)))
			flightData_zkrtctrl.y = OBSTACLE_VEL_LEFT_Y(GuidanceObstacleData.ob_velocity);
//			flightData_zkrtctrl.y = OBSTACLE_VEL_LEFT_Y;
		else
			flightData_zkrtctrl.y = 0;
#else
		if(move_flag &(1<<(GE_DIR_FRONT-1)))                     //ǰ��
			flightData_zkrtctrl.y = 0;.y = OBSTACLE_ANG_FORWORD_Y;
		else if(move_flag &(1<<(GE_DIR_BACK-1)))
			flightData_zkrtctrl.y = 0;.y = OBSTACLE_ANG_BACK_Y;
		else
			flightData_zkrtctrl.y = 0;.y = 0;
		
		if(move_flag &(1<<(GE_DIR_RIGHT-1)))                     //����                    
			flightData_zkrtctrl.y = 0;.x = OBSTACLE_ANG_RIGHT_X;         
		else if(move_flag &(1<<(GE_DIR_LEFT-1)))
			flightData_zkrtctrl.y = 0;.x = OBSTACLE_ANG_LEFT_X;
		else
			flightData_zkrtctrl.y = 0;.x = 0;		
#endif		
//		printf("flight vel x=%f, y=%f! \n", flightData_obstacle.x, flightData_obstacle.y);
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	}
	else
	{
		if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
		if(flightData_zkrtctrl.x)
			flightData_zkrtctrl.x = 0;
		if(flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
}
/**
*   @brief  avoid_obstacle_alarm_V2 ���Ͽ���
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm_V2(void)
{
	u8 move_flag; //�ƶ����
	float fl_x, fl_y;
	double yaw;
	
	if(djisdk_state.run_status !=avtivated_ok_djirs) //OESû����
		return;
	
	if(GuidanceObstacleData.online_flag ==0) //Guidance������ 
		return;
	
	if(GuidanceObstacleData.ob_enabled ==0) //���ϲ���Ч
	{
		if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
		return;
	}
	yaw = getYaw(v->broadcast->getQuaternion());
	dji_get_roll_pitch(&djif_status.roll, &djif_status.pitch);
	djif_status.xnow = v->broadcast->getVelocity().x*cos(yaw)+v->broadcast->getVelocity().y*sin(yaw);
  djif_status.ynow = -v->broadcast->getVelocity().x*sin(yaw)+v->broadcast->getVelocity().y*cos(yaw);
	move_flag = obstacle_avoidance_handle_V2(&fl_x, &fl_y, v->broadcast->getRC().pitch, v->broadcast->getRC().roll); 
//	if(GuidanceObstacleData.obstacle_time_flag)
//	{
//			flightData_zkrtctrl.x = 0;
//			flightData_zkrtctrl.y = 0;
//			djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
//	}
//	else
//	{
		if(move_flag)
		{
			flightData_zkrtctrl.x = fl_x;
			flightData_zkrtctrl.y = fl_y;
			djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
			ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
		}
		else
		{
			if(	djisdk_state.oes_fc_controled)
				djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
			if(flightData_zkrtctrl.x)
				flightData_zkrtctrl.x = 0;
			if(flightData_zkrtctrl.y)
				flightData_zkrtctrl.y = 0;
		}
//	}
}
/**
*   @brief  avoid_obstacle_alarm_V5 ���Ͽ���
  * @parm   none
  * @retval none
  */
void avoid_obstacle_alarm_V3(void)
{
	u8 ctrl_flag=0;
	float fl_x=0, fl_y=0,fl_z=0,fl_yaw=0;
//	float vel_limiting = OBSTACLE_AVOID_VEL(GuidanceObstacleData.ob_velocity);
	float vel_hover = 0;
	float vel_return = 0.5;
	float vel_z_limit = 0.5;
	float rate_yaw_limit = 20;
	int i;
	if(djisdk_state.run_status !=avtivated_ok_djirs) //OESû����
		return;
	
	if(GuidanceObstacleData.online_flag ==0) //Guidance������ 
		return;
	
	if(GuidanceObstacleData.ob_enabled ==0) //���ϲ���Ч
	{
		if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
		return;
	}
	
	obstacle_control_run_reset();  //����״̬����
	
#ifdef USE_OBSTACLE_TEST2
/*
	cmd�� FA FB AA FRONT BACK RIGHT LEFT PITCH ROLL THROTTLE YAW INIT_FLAG FE
	*/	
	GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] = myTerminal.cmdIn[3]*10;
	GuidanceObstacleData.g_distance_value[GE_DIR_BACK] = myTerminal.cmdIn[4]*10;
	GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] = myTerminal.cmdIn[5]*10;
	GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] = myTerminal.cmdIn[6]*10;
	//����, �ɻ���б�Ƕȹ���ʱ
	djif_status.fiter_angle_ob = get_filter_ang_ob();
	if(djif_status.roll > djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] = DISTANCE_2HIGH_BY_ANGLE;
	if(djif_status.roll <-djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] = DISTANCE_2HIGH_BY_ANGLE;
	if(djif_status.pitch >djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_BACK] = DISTANCE_2HIGH_BY_ANGLE;			
	if(djif_status.pitch <-djif_status.fiter_angle_ob)
		GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] = DISTANCE_2HIGH_BY_ANGLE;
#endif
#ifdef USE_OBSTACLE_TEST1		
	djif_status.rc_pitch = myTerminal.cmdIn[7]*10*myTerminal.int_flag;
	djif_status.rc_roll = myTerminal.cmdIn[8]*10*myTerminal.int_flag;
	djif_status.rc_throttle = myTerminal.cmdIn[9]*10*myTerminal.int_flag;
	djif_status.rc_yaw = myTerminal.cmdIn[10]*10*myTerminal.int_flag;
	djif_status.xnow = 0;
	djif_status.ynow = 0;
#endif
	
#ifndef USE_OBSTACLE_TEST1
	dji_get_flight_parm((void*)&djif_status);   //��ȡ�ɻ�״̬����Ϣ
#endif

	//��������߼����
	obstacleAllControl.control[GE_DIR_FRONT-1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_FRONT], GE_DIR_FRONT);
	obstacleAllControl.control[GE_DIR_BACK-1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_BACK], GE_DIR_BACK);
	obstacleAllControl.control[GE_DIR_RIGHT-1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT], GE_DIR_RIGHT);
	obstacleAllControl.control[GE_DIR_LEFT-1].state = obstacle_check_per_dirc(&djif_status, GuidanceObstacleData.g_distance_value[GE_DIR_LEFT], GE_DIR_LEFT);
	
	//ǰ��ɸѡ��x�����״̬
	//����ɸѡ��y�����״̬
	for(i=0; i<4; i++)
	{
		if((obstacleAllControl.control[i].state== OCS_LIMITING_VEL)&&(obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state == OCS_VOLUNTRAY_AVOID))
		{
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state = OCS_NO_CONTROL;
		}
		if((obstacleAllControl.control[i].state== OCS_HOVER)&&(obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state == OCS_VOLUNTRAY_AVOID))
		{
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state = OCS_HOVER;
		}
		if((obstacleAllControl.control[i].state== OCS_VOLUNTRAY_AVOID)&&(obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state == OCS_VOLUNTRAY_AVOID))
		{
			obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state = OCS_HOVER;
			obstacleAllControl.control[i].state = OCS_HOVER;
		}
		if((obstacleAllControl.control[i].state== OCS_NO_CONTROL )&&(obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state == OCS_VOLUNTRAY_AVOID))
		{
			if(is_rc_goto_dir(i+1))
			{
				obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state = OCS_NO_CONTROL; //special handle
			}
			if(GuidanceObstacleData.g_distance_value[i+1] <GuidanceObstacleData.ob_distance)  //special handle
			{
				obstacleAllControl.control[(obstacleAllControl.control[i].opposite)-1].state = OCS_HOVER;
				obstacleAllControl.control[i].state = OCS_HOVER;
			}
		}
	}
//	obstacleAllControl.x_state = OCS_NO_CONTROL;
//	obstacleAllControl.y_state = OCS_NO_CONTROL;	
//	fl_x =0;fl_y=0;fl_z=0;
//limiting	
	if(obstacleAllControl.control[GE_DIR_FRONT-1].state== OCS_LIMITING_VEL)
	{
//		fl_x = vel_limiting;
		fl_x = get_limit_vx(GE_DIR_FRONT);
		obstacleAllControl.x_state = OCS_LIMITING_VEL;
	}
	if(obstacleAllControl.control[GE_DIR_BACK-1].state== OCS_LIMITING_VEL)
	{
//		fl_x = -vel_limiting;
		fl_x = get_limit_vx(GE_DIR_BACK);
		obstacleAllControl.x_state = OCS_LIMITING_VEL;
	}
	if(obstacleAllControl.control[GE_DIR_RIGHT-1].state== OCS_LIMITING_VEL)
	{
//		fl_y = vel_limiting;
		fl_y = get_limit_vy(GE_DIR_RIGHT);
		obstacleAllControl.y_state = OCS_LIMITING_VEL;
	}
	if(obstacleAllControl.control[GE_DIR_LEFT-1].state== OCS_LIMITING_VEL)
	{
//		fl_y = -vel_limiting;
		fl_y = get_limit_vy(GE_DIR_LEFT);
		obstacleAllControl.y_state = OCS_LIMITING_VEL;
	}
//hover
	if(obstacleAllControl.control[GE_DIR_FRONT-1].state== OCS_HOVER)
	{
		fl_x = vel_hover;
		obstacleAllControl.x_state = OCS_HOVER;
	}
	if(obstacleAllControl.control[GE_DIR_BACK-1].state== OCS_HOVER)
	{
		fl_x = -vel_hover;
		obstacleAllControl.x_state = OCS_HOVER;
	}
	if(obstacleAllControl.control[GE_DIR_RIGHT-1].state== OCS_HOVER)
	{
		fl_y = vel_hover;
		obstacleAllControl.y_state = OCS_HOVER;
	}
	if(obstacleAllControl.control[GE_DIR_LEFT-1].state== OCS_HOVER)
	{
		fl_y = -vel_hover;
		obstacleAllControl.y_state = OCS_HOVER;
	}
//return	
	if(obstacleAllControl.control[GE_DIR_FRONT-1].state== OCS_VOLUNTRAY_AVOID)
	{
		fl_x = -vel_return;
		obstacleAllControl.x_state = OCS_VOLUNTRAY_AVOID;
	}
	if(obstacleAllControl.control[GE_DIR_BACK-1].state== OCS_VOLUNTRAY_AVOID)
	{
		fl_x = vel_return;
		obstacleAllControl.x_state = OCS_VOLUNTRAY_AVOID;
	}
	if(obstacleAllControl.control[GE_DIR_RIGHT-1].state== OCS_VOLUNTRAY_AVOID)
	{
		fl_y = -vel_return;
		obstacleAllControl.y_state = OCS_VOLUNTRAY_AVOID;
	}
	if(obstacleAllControl.control[GE_DIR_LEFT-1].state== OCS_VOLUNTRAY_AVOID)
	{
		fl_y = vel_return;
		obstacleAllControl.y_state = OCS_VOLUNTRAY_AVOID;
	}
	
//��ͣ��������£�������rc����ʱ�ͷŸ������Ȩ	
	if((!is_rc_goto_dir(GE_DIR_FRONT))&&(!is_rc_goto_dir(GE_DIR_BACK))
		&&(obstacleAllControl.x_state ==OCS_HOVER)
	)
	{
		fl_x = 0;
		obstacleAllControl.x_state = OCS_NO_CONTROL;
	}
	if((!is_rc_goto_dir(GE_DIR_RIGHT))&&(!is_rc_goto_dir(GE_DIR_LEFT))
		&&(obstacleAllControl.y_state ==OCS_HOVER)
	)
	{
		fl_y =0;
		obstacleAllControl.y_state = OCS_NO_CONTROL;
	}
	
//OES����ʱ��ӳ��RC����״̬����OES���п���  ---y��
	if(obstacleAllControl.x_state >OCS_NO_CONTROL)
	{
		if(obstacleAllControl.y_state ==OCS_NO_CONTROL)
		{
			if(is_rc_goto_dir(GE_DIR_RIGHT))
			{
//				fl_y = vel_limiting;
				fl_y = get_limit_vy(GE_DIR_RIGHT);
				obstacleAllControl.y_state = OCS_OES_REPLACE_RC;
			}
			else if(is_rc_goto_dir(GE_DIR_LEFT))
			{
//				fl_y = -vel_limiting;
				fl_y = get_limit_vy(GE_DIR_LEFT);				
				obstacleAllControl.y_state = OCS_OES_REPLACE_RC;
			}		
		}
	}
//OES����ʱ��ӳ��RC����״̬����OES���п���  ---x��	
	if(obstacleAllControl.y_state >OCS_NO_CONTROL)
	{
		if(obstacleAllControl.x_state ==OCS_NO_CONTROL)
		{
			if(is_rc_goto_dir(GE_DIR_FRONT))
			{
//				fl_x = vel_limiting;				
				fl_x = get_limit_vx(GE_DIR_FRONT);
				obstacleAllControl.x_state = OCS_OES_REPLACE_RC;
			}
			else if(is_rc_goto_dir(GE_DIR_BACK))
			{
//				fl_x = -vel_limiting;				
				fl_x = get_limit_vx(GE_DIR_BACK);
				obstacleAllControl.x_state = OCS_OES_REPLACE_RC;
			}			
		}
	}
//OES����ʱ��ӳ��RC����״̬����OES���п���  ---z��
	if((obstacleAllControl.x_state >OCS_NO_CONTROL)||(obstacleAllControl.y_state >OCS_NO_CONTROL))
	{
		ctrl_flag = 1;
		if(djif_status.rc_throttle >500)
		{
			fl_z = vel_z_limit;
		}
		else if(djif_status.rc_throttle <-500)
		{
			fl_z = -vel_z_limit;
		}
		else
			fl_z = 0;
		if(djif_status.rc_yaw >500)
		{
			fl_yaw = rate_yaw_limit;
		}
		else if(djif_status.rc_yaw <-500)
		{
			fl_yaw = -rate_yaw_limit;
		}
		else
			fl_yaw = 0;
	}
	else
	{
		ctrl_flag = 0;
		for(i=0; i<4; i++)
		{
			obstacleAllControl.control[i].state = OCS_NO_CONTROL;
		}
	}

	//��¼���ο���״̬
	for(i=0; i<4; i++)
	{
		obstacleAllControl.control[i].last_state = obstacleAllControl.control[i].state;
	}
	
	//zkrt_debug
//	printf("control:%d,Vx=%f,Vy=%f,Vz=%f\n", ctrl_flag, fl_x, fl_y, fl_z);
//	printf("height:%f, angle:%f\n", djif_status.height, djif_status.fiter_angle_ob);
	if(ctrl_flag)
	{ 
		flightData_zkrtctrl.x = fl_x;
		flightData_zkrtctrl.y = fl_y;
		flightData_zkrtctrl.z = fl_z;
		flightData_zkrtctrl.yaw = fl_yaw;
		djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	}
	else
	{
		if(	djisdk_state.oes_fc_controled)
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
		if(flightData_zkrtctrl.x)                                                 
			flightData_zkrtctrl.x = 0;
		if(flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
}
#endif
/**
*   @brief  sys_ctrl_timetask ϵͳ��ʱ�����뼶
  * @parm   none
  * @retval none
  */
int temp_sct_i;
void sys_ctrl_timetask(void)
{
	for(temp_sct_i=0; temp_sct_i< 4; temp_sct_i++)
	{
		if(GuidanceObstacleData.constant_speed_time[temp_sct_i]--)
		{
			if(!GuidanceObstacleData.constant_speed_time[temp_sct_i])
			{
				GuidanceObstacleData.constant_speed_time_flag[temp_sct_i] = OCS_NO_CONTROL;
			}	
		}
//		if(GuidanceObstacleData.obstacle_time[temp_sct_i]--)
//		{
//			if(!GuidanceObstacleData.obstacle_time[temp_sct_i])
//			{
//				GuidanceObstacleData.obstacle_time_flag[temp_sct_i] = 0;
//			}
//		}
	}
}
