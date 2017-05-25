/**
  ******************************************************************************
  * @file    djiCtrl.cpp 
  * @author  ZKRT
  * @version V0.0.1
  * @date    15-March-2017
  * @brief   dji_ctrl communication with the a3 flight controller
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
#include "djiCtrl.h"
#ifdef __cplusplus
extern "C"
{
#include "sys.h"
#include "ostmr.h"
#include "undercarriageCtrl.h"
#include "heartBeatHandle.h"
}
#endif

extern CoreAPI *coreApi;
extern Flight flight;
extern uint8_t myFreq[16];
extern FlightData flightData_zkrtctrl;
extern VirtualRC virtualrc;

extern "C" void sendToMobile(uint8_t *data, uint8_t len);

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void dji_flight_ctrl(void);
void djizkrt_timer_task(void);
void get_flight_data_and_handle(void);
void heartbeat_ctrl(void);

/* Private variables ---------------------------------------------------------*/

/*----------ZKRT VARIABLE----------*/
dji_sdk_status djisdk_state = {init_none_djirs, 0, 0xffffffff, 0, 0};  //dji sdk ����״̬

//#define FCC_TIMEROUT         10  //200ms
#define FCC_TIMEROUT         1  //20ms //zkrt_todo : �������ڴ�����
#define GETFDATA_TIMEROUT    10  //200ms
volatile u16 fc_timercnt = FCC_TIMEROUT;//�ɿ����ڿ���ʱ�Ӽ���
volatile u16 getfdata_timercnt = GETFDATA_TIMEROUT;//���ڻ�ȡ��������ʱ�Ӽ���

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  DJI init. ��SDK��ʼ��
  * @param  None
  * @retval None
  */
void dji_init(void)
{
	delay_nms(5000); 
  ZKRT_LOG(LOG_NOTICE,"This is the example App to test DJI onboard SDK on STM32F4Discovery Board! \r\n");
  ZKRT_LOG(LOG_NOTICE,"Refer to \r\n");
  ZKRT_LOG(LOG_NOTICE,"https://developer.dji.com/onboard-sdk/documentation/github-platform-docs/STM32/README.html \r\n");
  ZKRT_LOG(LOG_NOTICE,"for supported commands!\r\n");
  ZKRT_LOG(LOG_NOTICE,"Board Initialization Done!\r\n");
  delay_nms(1000);
	t_ostmr_insertTask(djizkrt_timer_task, 20, OSTMR_PERIODIC);   //20ms task
}
/**
  * @brief  dji_process.  DJI ����
  * @param  None
  * @retval None
  */
void dji_process(void)
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
//				heartbeat_ctrl(); //zkrt_debug
			}
			break;
		case avtivated_ok_djirs:
			dji_flight_ctrl();  //���п���
		  get_flight_data_and_handle(); //�������ݴ���
		  heartbeat_ctrl();  //��������ʱ����
			break;
		default:break;
	}
	coreApi->sendPoll();
}
/**
  * @brief  dji_flight_ctrl
  * @param  None
  * @retval None
  */
void dji_flight_ctrl(void)
{
	if(virtualrc.getRCData().mode <=0) //���ң������λ��P�����⣬���п�������
		return;
	
	if(((djisdk_state.oes_fc_controled)||(djisdk_state.last_fc_controled))
		&&((fc_timercnt == 0)))   //���ڷ��ͷ��п�������
	//zkrt_notice: �ĵ�������20ms���ڿ��ƣ����ĵ���˵�����ơ�https://developer.dji.com/cn/onboard-sdk/documentation/application-development-guides/programming-guide.html
	{
		fc_timercnt = FCC_TIMEROUT;
	}
	else
		return;
	
	if(djisdk_state.oes_fc_controled)
	{
		if(flight.getStatus() == 2) //���п���ֻ�ڷ���״̬==in_air_motor_onʱ����ʹ��
		{
			if(flight.getControlDevice()!=2)  //OES���տ���Ȩ��ʱ=DEVICE_SDK //������Ϣ�㲥Ƶ������Ϊ50hz��20ms����һ�Σ����������Ϣ����Ƶ��Ϊ40ms�������ϲ����п�����Ϣ�ӳٸ���Ӱ��˴��߼��������
			{
				coreApi->setControl(1);	
				ZKRT_LOG(LOG_INOTICE, "oes setControl\n");
			}
//			printf("x=%f,y=%f\n", flightData_zkrtctrl.x, flightData_zkrtctrl.y);
			flight.setFlight(&flightData_zkrtctrl);
			ZKRT_LOG(LOG_NOTICE, "oes flight control=================\r\n")			
		}
	}
	else
	{
		if(flight.getControlDevice()==2)  //OES���տ���Ȩ��ʱ=DEVICE_SDK
		{
			coreApi->setControl(0);	   
		  ZKRT_LOG(LOG_INOTICE, "oes control closed\n");			
		}
	}
	
	if(djisdk_state.last_fc_controled != djisdk_state.oes_fc_controled)
		djisdk_state.last_fc_controled = djisdk_state.oes_fc_controled;  //���ϴο���ֵ=��ǰ����ֵ
}
/**
  * @brief  get_flight_data_and_handle
  * @param  None
  * @retval None
  */
void get_flight_data_and_handle(void)
{
	if(getfdata_timercnt == 0)
	{
		getfdata_timercnt = GETFDATA_TIMEROUT;
	}
	else
		return;
	
	float32_t f_act_height;
//	u8 controlDevice;
//	u8 flightstatus;
	f_act_height = flight.getPosition().height;     //����߶ȣ��Ǻ��θ߶�
//	printf("f_act_height:%f\n", f_act_height);	  
//	controlDevice = flight.getControlDevice();
//	printf("controlDevice:%d\n", controlDevice);  //�ɿؿ���Ȩ��
//  flightstatus = flight.getStatus();
//	printf("flightstatus:%d\n", flightstatus);  //��������״̬
//	printf("virtualrc.mode=%d\n", virtualrc.getRCData().mode); //ң����ֵ
//	printf("vel x=%f, y=%f, z=%f\n", flight.getVelocity().x, flight.getVelocity().y, flight.getVelocity().z);//����Ǵ������ϵ�ٶ�
//	printf("vel x=%f, y=%f, z=%f\n", flight.getYawRate().x, flight.getYawRate().y, flight.getYawRate().z);//����ǽ��ٶ�
//	printf("getAcceleration x=%f, y=%f, z=%f\n", flight.getAcceleration().x, flight.getAcceleration().y, flight.getAcceleration().z);//����Ǽ��ٶ�
//	printf("gps vel x=%f, y=%f, z=%f\n", coreApi->getBroadcastData().gps.velocityNorth, coreApi->getBroadcastData().gps.velocityEast, coreApi->getBroadcastData().gps.velocityGround);//�����GPS�ٶ�
//	double yaw;
//	double roll;
//	double pitch;
//	yaw = flight.getYaw();
//	roll = flight.getRoll();
//	pitch = flight.getPitch();
//	printf("yaw=%f, roll=%f, pitch=%f\n", yaw, roll, pitch);   //yaw , roll, pitch
	
	if(!undercarriage_data.uce_autoenabled)
		return;
	
	if(f_act_height > UDCAIE_TRIGGER_UP_HEIGHT)
	{
		undercarriage_data.state_bya3height = uped_udcaie_rs;
	}
	else if(f_act_height < UDCAIE_TRIGGER_DOWN_HEIGHT)
	{
		undercarriage_data.state_bya3height = downed_udcaie_rs;
	}
}
/**
  * @brief  heartbeat_ctrl ����������
  * @param  None
  * @retval None
  */
void heartbeat_ctrl(void)
{
	if(zkrt_heartbeat_pack())
	{
		sendToMobile((uint8_t*)&_zkrt_packet_hb, 50);
//		int k; //zkrt_debug
//		printf("heart start_code=0x %x\r\n",_zkrt_packet_hb.start_code);
//		for(k=0; k<30; k++)
//			printf("%x ",_zkrt_packet_hb.data[k]);
//		printf("\r\n");
	}
}
/**
  * @brief  djizkrt_timer_task, by ostmr.c module
  * @param  None
  * @retval None
  */
void djizkrt_timer_task(void)
{
  if(fc_timercnt)
	{
		fc_timercnt--;
	}	
  if(getfdata_timercnt)
	{
		getfdata_timercnt--;
	}
}
/**
  * @}
  */ 
/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

