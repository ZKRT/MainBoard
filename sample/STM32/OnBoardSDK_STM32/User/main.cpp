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
#include "main.h"
#include "stm32f4xx_adc.h"
#include "DJI_API.h"
#include "DJI_Camera.h"
#include "DJI_Type.h"
#include "DJI_VirtualRC.h"
#ifdef __cplusplus
extern "C"
{
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
}
#endif //__cplusplus

/* Private define ------------------------------------------------------------*/
#undef USE_ENCRYPT
/*-----------------------DJI_LIB VARIABLE-----------------------------*/
using namespace DJI::onboardSDK;

HardDriver* driver = new STM32F4;
CoreAPI defaultAPI = CoreAPI(driver);
CoreAPI *coreApi = &defaultAPI;

Flight flight = Flight(coreApi);
#define FLIGHTDATA_VERT_VEL_1MS	{0x48, 0, 0, 1, 0}  //��ֱ�ٶ�1m/s���Ϸ��еķ������� //add by yanly //The Control Model Flag 0x48 (0b 0100 1000) sets the command values to be X, Y, Z velocities in ground frame and Yaw rate.
#define FLIGHT_ZKRT_CONTROL_MODE  0x4A  //0x4A: non-stable mode����������ϵ,HORI_VEL,VERT_VEL,YAW_RATE
FlightData flightData;
FlightData flightData_zkrtctrl = {FLIGHT_ZKRT_CONTROL_MODE, 0, 0,0, 0};
#ifdef USE_OBSTACLE_AVOID_FUN
FlightData flightData_obstacle= {OBSTACLE_MODE, 0, 0,0, 0};
#endif
Camera camera=Camera(coreApi);
GimbalSpeedData gimbalSpeedData;
VirtualRC virtualrc = VirtualRC(coreApi);
VirtualRCData myVRCdata =
{ 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
    1024, 1024 };

extern TerminalCommand myTerminal;
extern LocalNavigationStatus droneState;
extern uint8_t myFreq[16];

/*----------ZKRT VARIABLE----------*/
dji_sdk_status djisdk_state = {init_none_djirs, 0, 0xffffffff, 0};  //dji sdk ����״̬
uint8_t tempture_invalid=0; //�����¶�ֵ�Ƿ���Ч����ЧΪ1����ЧΪ0
//uint8_t msgbuffer[25] = {0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77};

/* Private function prototypes -----------------------------------------------*/

void dji_init(void);                         
void dji_process(void);
void mobile_heardbeat_packet_control(void);
void tempture_flight_control(void);
void avoid_temp_alarm(void);
#ifdef USE_OBSTACLE_AVOID_FUN
void avoid_obstacle_alarm(void);
#endif
//#ifdef __cplusplus
//extern "C" void ADC_SoftwareStartConv(ADC_TypeDef* ADCx);
//#endif //__cplusplus

/**
  * @brief  main. ������
  * @param  None
  * @retval None
  */
int main()
{
  BSPinit();
	delay_nms(5000);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n"); 
	printf("PRODUCT_NAME: %s\r\nPRODUCT_ID: %s\r\nPRODUCT_VERSION: %s\r\nPRODUCT_TIME: %s %s\r\n",PRODUCT_NAME,PRODUCT_ID,PRODUCT_VERSION,__DATE__,__TIME__);
	ZKRT_LOG(LOG_INOTICE, "==================================================\r\n"); 
#ifdef USE_DJI_FUN	
	dji_init();
#endif 
#ifdef USE_LWIP_FUN	
	lwip_prcs_init();
#endif	
#ifdef USE_OBSTACLE_AVOID_FUN	
	guidance_init();
#endif	
  while (1)
  {
#ifdef USE_DJI_FUN			
		dji_process();                        //��SDK����
#endif		
		mobile_data_process();                //�����յ���mobile͸�����ݽ��н�������
    main_zkrt_dji_recv();		              //����ģ��ͨ��CAN�������ݣ�����������Ͷ������䵽���������������λ��һ
		tempture_flight_control();            //�¶ȳ����������������ݹ���
#ifdef USE_OBSTACLE_AVOID_FUN	
		main_recv_decode_zkrt_dji_guidance(); //Guidance���ݰ���������
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
		IWDG_Feed();
  }
}
/**
  * @brief  DJI init. ��SDK��ʼ��
  * @param  None
  * @retval None
  */
void dji_init()
{
	delay_nms(10000); 
  ZKRT_LOG(LOG_NOTICE,"This is the example App to test DJI onboard SDK on STM32F4Discovery Board! \r\n");
  ZKRT_LOG(LOG_NOTICE,"Refer to \r\n");
  ZKRT_LOG(LOG_NOTICE,"https://developer.dji.com/onboard-sdk/documentation/github-platform-docs/STM32/README.html \r\n");
  ZKRT_LOG(LOG_NOTICE,"for supported commands!\r\n");
  ZKRT_LOG(LOG_NOTICE,"Board Initialization Done!\r\n");
  delay_nms(1000);
}
/**
  * @brief  dji_process.  DJI ����
  * @param  None
  * @retval None
  */
void dji_process()
{
	switch(djisdk_state.run_status)
	{
		case init_none_djirs:
			djisdk_state.run_status = set_avtivate_djirs;
			djisdk_state.cmdres_timeout = TimingDelay;
//>>>>>dji oes standby work start			
      coreApi->setBroadcastFreq(myFreq);
   		delay_nms(50);

      //! Since OSDK 3.2.1, the new versioning system does not require you to set version.
      //! It automatically sets activation version through a call to getDroneVersion.
      coreApi->getDroneVersion();
      delay_nms(1000);
		  coreApi->sendPoll();
      User_Activate();      
      delay_nms(50);
//>>>>>dji oes standby work end
			break;
		case set_avtivate_djirs:
			if(djisdk_state.cmdres_timeout - TimingDelay >= 5000) //����ɹ�ʱ��ÿ5�����¼���һ��
			{
				djisdk_state.run_status = init_none_djirs;
			}
			break;
		case avtivated_ok_djirs:
			break;
		default:break;
	}
	coreApi->sendPoll();
}
///**
//  * @brief  dji_process.  old dji_process function before A3_FW < 1.7.0
//  * @param  None
//  * @retval None
//  */
//void dji_process() 
//{
//	switch(djisdk_state.run_status)
//	{
//		case init_none_djirs:
//			coreApi->setBroadcastFreq(myFreq, zkrt_setFrequencyCallback);
//			coreApi->setBroadcastCallback(myRecvCallback,(DJI::UserData)(&droneState));
//			djisdk_state.run_status = set_broadcastFreq_djirs;
//			djisdk_state.cmdres_timeout = TimingDelay;
//			break;
//		case set_broadcastFreq_djirs:
//			if(djisdk_state.cmdres_timeout - TimingDelay >= 5000)
//			{
//				ZKRT_LOG(LOG_NOTICE,"setBroadcastFreq\n");
//				coreApi->setBroadcastFreq(myFreq, zkrt_setFrequencyCallback);
//				coreApi->setBroadcastCallback(myRecvCallback,(DJI::UserData)(&droneState));
//				User_Activate(); //zkrt_notice: �����setBroadcastFreqһ���ͣ��������Ϊ����ԭ�򲻳ɹ�������һ��ʼ���Կ����ɿط������ļ���ʧ����Ϣ 
//				djisdk_state.cmdres_timeout = TimingDelay;
//			}	
//			break;
//		case set_broadcastFreq_ok:
//			User_Activate(); 
//			djisdk_state.run_status = set_avtivate_djirs;
//			djisdk_state.cmdres_timeout = TimingDelay;
//			break;
//		case set_avtivate_djirs:
//			if(djisdk_state.cmdres_timeout - TimingDelay >= 5000) //����ɹ�ʱ��ÿ5�뼤��һ��
//			{
//				User_Activate(); 
//				djisdk_state.cmdres_timeout = TimingDelay;
//			}
//			break;
//		case avtivated_ok_djirs:
//			break;
//		default:break;
//	}
//	coreApi->sendPoll();
//}
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
		ADC_SoftwareStartConv(ADC1); /*����ADC*/	
//		ZKRT_LOG(LOG_NOTICE, "ADC_SoftwareStartConv!\r\n");	
		if((_read_count%2) == 0)									
		{
			zkrt_dji_read_heart_tempture();  /*��ȡ�¶ȴ���������*/
			
			avoid_temp_alarm(); //���¿���
#ifdef USE_OBSTACLE_AVOID_FUN			
			avoid_obstacle_alarm(); //���Ͽ���
#endif			
			
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
			#if 1
			switch (mavlink_type_flag_dji)
			{
				case 0:
					mavlink_type_flag_dji = 1;
					dji_zkrt_read_heart_ack(); /*������յ����ݰ������ҷ��ͳ���*/
					coreApi->sendPoll();
					break;
				case 1:
					mavlink_type_flag_dji = 0;
				//dji_bat_value_send();  /*�������ܵ�����ݣ��ܹ�32���ֽ�*/    //????Ϊʲôû�з�װzkrt packet��ʽ //modify by yanly
					coreApi->sendPoll();
					break;
				default:
					break;
			}
			#endif
//			printf("T1 = %d T2 = %d  ADC1_25_dji=%d  ADC1_5_dji=%d  ADC1_I_dji=%d   LOW_VALUE = %d HIGH_VALUE = %d STA1 = %x STA2 = %x\r\n",
//			tempture0, tempture1, ADC1_25_dji,ADC1_5_dji,ADC1_I_dji,glo_tempture_low, glo_tempture_high, msg_smartbat_dji_buffer[0],
//			msg_smartbat_dji_buffer[3]);
		}
	}	
	
/*����can�յ���ģ����Ϣ���ж��ĸ�ģ�����ߺ󣬽������������߱����λ*/	
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
		coreApi->setControl(1);
		flightData_zkrtctrl.z = 1;
		flight.setFlight(&flightData_zkrtctrl);
		ZKRT_LOG(LOG_NOTICE, "avoid_temp_alarm open======================================\r\n");
    djisdk_state.oes_fc_controled |= 1<< fc_tempctrl_b;
	}
	else
	{
		djisdk_state.temp_alarmed = 0;
		if(flightData_zkrtctrl.z)
			flightData_zkrtctrl.z = 0;
	  if(	djisdk_state.oes_fc_controled)
		{
			djisdk_state.oes_fc_controled &= ~(1<< fc_tempctrl_b);
			if(djisdk_state.oes_fc_controled ==0)
			{
				coreApi->setControl(0);  //�ͷſ���Ȩ
				ZKRT_LOG(LOG_INOTICE, "oes control closed\n");
			}
	  }
//		virtualrc.setControl(0,virtualrc.CutOff_ToRealRC);
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
	if(djisdk_state.run_status !=avtivated_ok_djirs) //DJI������
		return;
	
	if(GuidanceObstacleData.online_flag ==0) //Guidance������
		return;
	
	move_flag = obstacle_avoidance_handle();
	if(move_flag)
	{
//		if(djisdk_state.oes_fc_controled ==0)
//		{
		  coreApi->setControl(1);			//ң�����л���λ�󣬿���Ȩ����ռ����Ҫ���»�ȡ����Ȩ����ʱ������ʱÿ�ζ���ȡ����Ȩ��//zkrt_todo
//		}	
		djisdk_state.oes_fc_controled |= 1<< fc_obstacle_b;
	
#ifdef OBSTACLE_VEL_MODE		
		if(move_flag &(1<<(GE_DIR_FRONT-1)))                     //ǰ��
			flightData_zkrtctrl.x = OBSTACLE_VEL_FORWORD_X;
		else if(move_flag &(1<<(GE_DIR_BACK-1)))
			flightData_zkrtctrl.x = OBSTACLE_VEL_BACK_X;
		else
			flightData_zkrtctrl.x = 0;
		
		if(move_flag &(1<<(GE_DIR_RIGHT-1)))                     //����                    
			flightData_zkrtctrl.y = OBSTACLE_VEL_RIGHT_Y;         
		else if(move_flag &(1<<(GE_DIR_LEFT-1)))
			flightData_zkrtctrl.y = OBSTACLE_VEL_LEFT_Y;
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
		flight.setFlight(&flightData_zkrtctrl);
		ZKRT_LOG(LOG_NOTICE, "avoid_obstacle_alarm open=================\r\n")
	}
	else
	{
		if(	djisdk_state.oes_fc_controled)
		{	
			djisdk_state.oes_fc_controled &= ~(1<< fc_obstacle_b);
			if(djisdk_state.oes_fc_controled ==0)
			{
				coreApi->setControl(0);
				ZKRT_LOG(LOG_INOTICE, "oes control closed\n");
			}
	  }
		if(flightData_zkrtctrl.x)
			flightData_zkrtctrl.x = 0;
		if(flightData_zkrtctrl.y)
			flightData_zkrtctrl.y = 0;
	}
}
#endif
extern "C" void sendToMobile(uint8_t *data, uint8_t len)
{
  coreApi->sendToMobile(data,  len);//����͸��������վ���
//  GPIO_ResetBits(GPIOD, GPIO_Pin_0);
	_FLIGHT_UART_TX_LED = 0;
  usart1_tx_flag = TimingDelay;
}
extern "C" void sendpoll()
{
  coreApi->sendPoll();//
}
