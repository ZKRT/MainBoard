/**
  ******************************************************************************
  * @file    sersorIntegratedHandle.c 
  * @author  ZKRT
  * @version V1.0
  * @date    08-March-2017
  * @brief    集成板传感器数据处理
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sersorIntegratedHandle.h"
#include "can.h"
#include "commonzkrt.h"
#include "obstacleAvoid.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SERSOR_HB_ID             1
/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
sensorIntegratedInfo_st sensorIntegratedInfo_t;
si_data_st *si_data_t = (si_data_st *)&sensorIntegratedInfo_t.data[0];
si_temp_data_st si_tempdata_t;
/* Private functions ---------------------------------------------------------*/
static uint8_t sersor_integrated_parse(sensorIntegratedInfo_st *packet, uint8_t ch);
/**
  * @brief  app_sersor_integrated_prcs
  * @param  None
  * @retval None
  */
void app_sersor_integrated_prcs(void)
{
	uint8_t value;
	
	//在线检测
	if(GuidanceObstacleData.online_timing - TimingDelay > GUIDANCE_ONLINE_TIMEOUT)
	{
		guidance_parmdata_init();
		GuidanceObstacleData.online_timing = TimingDelay;
		ZKRT_LOG(LOG_ERROR, "Guidance UART not online!\n");
	}
	
	//parse can2 message
	while (CAN2_rx_check(SERSOR_HB_ID) == 1)  
	{
		value = CAN2_rx_byte(SERSOR_HB_ID);
		if (sersor_integrated_parse(&sensorIntegratedInfo_t,value)==1) 
		{
			//避障全局变量置
			GuidanceObstacleData.g_distance_value[GE_DIR_LEFT] = si_data_t->left_D;
			GuidanceObstacleData.g_distance_value[GE_DIR_RIGHT] = si_data_t->right_D;
			GuidanceObstacleData.g_distance_value[GE_DIR_BACK] = si_data_t->back_D;
			GuidanceObstacleData.g_distance_value[GE_DIR_FRONT] = si_data_t->front_D;
			//zkrt_todo 避温变量置
			tempture0 = si_data_t->body_T;
			tempture1 = si_data_t->body_T;
			si_tempdata_t.body_T = si_data_t->body_T;
			si_tempdata_t.left_T = si_data_t->left_T;
			si_tempdata_t.right_T = si_data_t->right_T;
			si_tempdata_t.m1_T = si_data_t->m1_T;
			si_tempdata_t.m2_T = si_data_t->m2_T;
			si_tempdata_t.m3_T = si_data_t->m3_T;
			si_tempdata_t.m4_T = si_data_t->m4_T;
			//传感器在线置位
			GuidanceObstacleData.online_flag = 1;
			GuidanceObstacleData.online_timing = TimingDelay;
		}
	}
}

uint8_t si_curser_index = 0;
/**
  * @brief  sersor_integrated_parse
  * @param  None
  * @retval None
  */
static uint8_t sersor_integrated_parse(sensorIntegratedInfo_st *packet, uint8_t ch)
{
	uint8_t si_ret = 0;
	
	if ((si_curser_index == 0)&&(ch == SI_START_CODE))
	{
		memset(packet, 0, sizeof(sensorIntegratedInfo_st));
		packet->start_code = ch;
		si_curser_index++;
	}
	else if ((si_curser_index == 1)&&(ch == SI_HEARTBEAT_CMD))
	{
		packet->cmd_code = ch;
		si_curser_index++;
	}
	else if ((si_curser_index == 2)&&(ch == SI_CANUP_DATALEN))
	{
		packet->datalen = ch;
		si_curser_index++;
	}	
	
	else if ((si_curser_index >=3)&&(si_curser_index <=26))									
	{
		packet->data[si_curser_index-3] = ch;
		si_curser_index++;
	}
	else if (si_curser_index == 27)
	{
		packet->crc = ch;
		si_curser_index++;
	}
	else if (si_curser_index == 28)
	{
		packet->crc = (ch<<8)|packet->crc;
		si_curser_index++;
	}
	else if ((si_curser_index == 29)&&(ch == SI_END_CODE))
	{
		packet->end_code = ch;
		si_curser_index = 0;
		if(packet->crc == (CRC16_Cal(packet->data, packet->datalen))) //check crc
		{
			si_ret = 1;
		}	
	}
	else
	{
		si_curser_index = 0;
	}
	
	return si_ret;
}



/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
