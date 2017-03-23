/*! @file djiapp.c
 *  @version 1.0
 *  @date 13 09 2016
 *
 *  @brief
 *  An exmaple program of DJI-onboard-SDK portable for stm32
 *
 *  Copyright 2016 ZKRT. All right reserved.
 *
 *******************************************************************************
 *                                                                             *
 *          --------               --------                 --------           *
 *         |        |  LB2  |                             |    USART1    |        |          *
 *         |   PC   | <---------> |  |    A3<----------> | stm32  |          *
 *         |        |    |        |               |        |      (USB-TTL)   *
 *         |        |             |        |               |        |          *
 *          --------               --------                 --------           *
 *                                                                             *
 *                                                                             *
 *******************************************************************************
 * */

#include "djiapp.h"
#define TEMPTURE_DIFF         8

uint8_t posion_dji_buffer[16] = {0};
uint8_t msg_smartbat_dji_buffer[30] = {0};
uint32_t dji_tempture_error_count = 0;
uint8_t tempture_change_too_fast_dji[4] = {0};
uint8_t dji_tempture_error_flag[2] = {0};
uint8_t mavlink_type_flag_dji = 0;
uint16_t ADC1_25_dji;
uint16_t ADC1_5_dji;
uint16_t ADC1_I_dji;
uint8_t V5_error_flag_dji = 0;

zkrt_packet_t can1_rx_dji_posion;

uint8_t msgbuffer11[10] = {0xcc,0xcc,0xcc,0xcc,0xcc,0x55,0x55,0x55,0x55,0x55};

void sendToMobile(uint8_t *data, uint8_t len);


uint8_t heart_ack_dji_count[DEVICE_NUMBER] = {0};

uint8_t heart_ack_dji_status[DEVICE_NUMBER] = {0};

uint8_t dji_which_byte = 0;
uint8_t dji_which_bit  = 0;



/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
uint8_t dji_heart_ack_check(uint8_t device_id, uint8_t value)
{
	dji_which_byte = (device_id-1)/8;			
	dji_which_bit  = device_id%8 -1;		
	device_id -= 1;										
	
	switch (heart_ack_dji_status[device_id])
	{
		case 0X00:
			if (value == 0XAA)
			{
				heart_ack_dji_status[device_id] |= 1<<0;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X01:
			if (value == 0XBB)
			{
				heart_ack_dji_status[device_id] |= 1<<1;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X03:
			if (value == 0XCC)
			{
				heart_ack_dji_status[device_id] |= 1<<2;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X07:
			if (value == 0XDD)
			{
				heart_ack_dji_status[device_id] |= 1<<3;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X0F:
			if (value == 0XEE)
			{
				heart_ack_dji_status[device_id] |= 1<<4;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X1F:
			if (value == (device_id+1))				
			{
				heart_ack_dji_status[device_id] |= 1<<5;
			}
			else
			{
				heart_ack_dji_status[device_id] = 0;
			}
			break;
		case 0X3F:
			if (device_id == (DEVICE_TYPE_THROW-1))	//如果是抛投模块的话
			{
				msg_smartbat_dji_buffer[26] &= 0XF8;			//将字26的低3位清空并赋值上传来的数据
				msg_smartbat_dji_buffer[26] |= value;
			}
			else if (device_id == (DEVICE_TYPE_IRRADIATE-1))				//如果强光
			{
				msg_smartbat_dji_buffer[26] &= 0XF7;								//将字26的位3清空
				msg_smartbat_dji_buffer[26] |= value<<3;
			}
			else if (device_id == (DEVICE_TYPE_MEGAPHONE-1))				//如果喊话
			{
				msg_smartbat_dji_buffer[26] &= 0XEF;									//将字26的位4清空
				msg_smartbat_dji_buffer[26] |= value<<4;						
			}
			else if (device_id == (DEVICE_TYPE_3DMODELING-1))				//如果三维建模
			{
				msg_smartbat_dji_buffer[26] &= 0XDF;						//将字26的位5清空
				msg_smartbat_dji_buffer[26] |= value<<5;						
			}
			heart_ack_dji_status[device_id] |= 1<<6;												
			break;
		case 0X7F:
			if (value != heart_ack_dji_count[device_id])								
			{
				heart_ack_dji_count[device_id] = value;											
				
				msg_smartbat_dji_buffer[23+dji_which_byte] |= (1<<dji_which_bit);		
				
				heart_ack_dji_status[device_id] |= 1<<7;
			}
			break;
		default:
		{
			break;
		}
	}
			
	if (heart_ack_dji_status[device_id] == 0XFF)
	{
		heart_ack_dji_status[device_id] = 0;			
		return 1;
	}
	
	return 0;
}


extern zkrt_packet_t can1_rx_posion;

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
void zkrt_read_heart_dji_posion(void)
{
	msg_smartbat_dji_buffer[11] = posion_dji_buffer[1];
	msg_smartbat_dji_buffer[12] = posion_dji_buffer[2];
	msg_smartbat_dji_buffer[13] = posion_dji_buffer[3];
		
	msg_smartbat_dji_buffer[14] = posion_dji_buffer[5];
	msg_smartbat_dji_buffer[15] = posion_dji_buffer[6];
	msg_smartbat_dji_buffer[16] = posion_dji_buffer[7];
		
	msg_smartbat_dji_buffer[17] = posion_dji_buffer[9];
	msg_smartbat_dji_buffer[18] = posion_dji_buffer[10];
	msg_smartbat_dji_buffer[19] = posion_dji_buffer[11];
	
	msg_smartbat_dji_buffer[20] = posion_dji_buffer[13];
	msg_smartbat_dji_buffer[21] = posion_dji_buffer[14];
	msg_smartbat_dji_buffer[22] = posion_dji_buffer[15];
	
	if ((msg_smartbat_dji_buffer[12] != 0)||(msg_smartbat_dji_buffer[13] != 0)||(msg_smartbat_dji_buffer[15] != 0)||(msg_smartbat_dji_buffer[16] != 0)
		||(msg_smartbat_dji_buffer[18] != 0)||(msg_smartbat_dji_buffer[19] != 0)||(msg_smartbat_dji_buffer[21] != 0)||(msg_smartbat_dji_buffer[22] != 0))
	{
		msg_smartbat_dji_buffer[23] |= 0X10;
	}
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

void zkrt_read_heart_dji_posion_check(void)
{
	//填充数组的第10位，记录状态
	if (posion_dji_buffer[0] == 0XFE)			//如果正常，那么置一；如果异常，那么置零
	{
		msg_smartbat_dji_buffer[10] |= 0X01;
	}
	else if (posion_dji_buffer[0] == 0XFD)
	{
		msg_smartbat_dji_buffer[10] &= 0XFE;
	}
	
	if (posion_dji_buffer[4] == 0XFE)
	{
		msg_smartbat_dji_buffer[10] |= 0X02;
	}
	else if (posion_dji_buffer[4] == 0XFD)
	{
		msg_smartbat_dji_buffer[10] &= 0XFD;
	}
	
	if (posion_dji_buffer[8] == 0XFE)
	{
		msg_smartbat_dji_buffer[10] |= 0X04;
	}
	else if (posion_dji_buffer[8] == 0XFD)
	{
		msg_smartbat_dji_buffer[10] &= 0XFB;
	}
	
	if (posion_dji_buffer[12] == 0XFE)
	{
		msg_smartbat_dji_buffer[10] |= 0X08;
	}
	else if (posion_dji_buffer[12] == 0XFD)
	{
		msg_smartbat_dji_buffer[10] &= 0XF7;
	}
}


/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
//CAN总线数据接收（can数据在后台中断接收，并缓冲到can1_rx_buff缓存数组里，前台从缓存数组里取数据解析处理）处理，CAN总线连接传感器设备。
void main_zkrt_dji_recv(void)
{
	uint8_t value;

	while (CAN1_rx_check(DEVICE_TYPE_GAS) == 1)  /*毒气检测模块*/
	{
//	      printf("CAN1_rx_check\r\n");
		value = CAN1_rx_byte(DEVICE_TYPE_GAS);
//		printf("CAN1_rx_byte=0x %x\r\n",value);
		if (zkrt_decode_char(&can1_rx_dji_posion,value)==1)/*如果当中一个数据丢失，整个数据包就得重新接收，这样是不是有点问题*/
		{
			memcpy((void *)posion_dji_buffer, (void *)&(can1_rx_dji_posion.data[0]), 16);

			zkrt_read_heart_dji_posion();						
			zkrt_read_heart_dji_posion_check();			
			
			posion_recv_flag = TimingDelay;		
		}
	}
		
	while (CAN1_rx_check(DEVICE_TYPE_THROW) == 1)  /*抛投模块*/
	{
		value = CAN1_rx_byte(DEVICE_TYPE_THROW);
		if (dji_heart_ack_check(DEVICE_TYPE_THROW, value)==1)
		{
			throw_recv_flag = TimingDelay;												
		}
	}
	while (CAN1_rx_check(DEVICE_TYPE_CAMERA) == 1) /*相机，可见光，红外*/
	{
		value = CAN1_rx_byte(DEVICE_TYPE_CAMERA);
		if (dji_heart_ack_check(DEVICE_TYPE_CAMERA, value)==1)
		{
			camera_recv_flag = TimingDelay;										
		}
	}
	while (CAN1_rx_check(DEVICE_TYPE_IRRADIATE) == 1) //照射灯
	{
		value = CAN1_rx_byte(DEVICE_TYPE_IRRADIATE);
		if (dji_heart_ack_check(DEVICE_TYPE_IRRADIATE, value)==1)
		{
			irradiate_recv_flag = TimingDelay;
		}
	}
	
	while (CAN1_rx_check(DEVICE_TYPE_MEGAPHONE) == 1)				//喊话
	{
		value = CAN1_rx_byte(DEVICE_TYPE_MEGAPHONE);
		if (dji_heart_ack_check(DEVICE_TYPE_MEGAPHONE, value)==1)
		{
			phone_recv_flag = TimingDelay;
		}
	}
	
	while (CAN1_rx_check(DEVICE_TYPE_3DMODELING) == 1)				//三维吊舱
	{
		value = CAN1_rx_byte(DEVICE_TYPE_3DMODELING);
		if (dji_heart_ack_check(DEVICE_TYPE_3DMODELING, value)==1)
		{
			threemodeling_recv_flag = TimingDelay;
		}
	}	
}


/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

//读取温度值，并且将温度值、温度上下限填充到心跳数组
void zkrt_dji_read_heart_tempture(void)
{
#if defined _TEMPTURE_IO_
  tempture0 = DS18B20_Get_Temp(DS18B20_NUM1);
  tempture1 = DS18B20_Get_Temp(DS18B20_NUM2);
  if ((tempture0 == 0XFFF)||(tempture1 == 0XFFF))
  {
//    GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;	//modify by yanly
  }
  else
  {
//    GPIO_SetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 1;	//modify by yanly
  }
#elif defined _TEMPTURE_ADC_
  tempture0 = ADC1_get_value(_T1_value); 
  tempture1 = ADC1_get_value(_T2_value);
  ZKRT_LOG(LOG_NOTICE,"#######tempture0= %d   tempture1= %d!\r\n",tempture0,tempture1);
#endif
}
/***********************************************************************
		          中科瑞泰消防无人机函数定义

		函数 名称：
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
//读取板载25V电压、5V电压、5V电流
void dji_zkrt_read_heart_vol(void)
{
  ADC1_25_dji = ADC1_get_value(_25V_value);
  ADC1_5_dji  = ADC1_get_value(_5V_value);
  ADC1_I_dji  = ADC1_get_value(_5A_value);
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义

		函数 名称：
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
//将当前的板载电压值进行判断，将异常情况写入到心跳数组里，并且执行断电或恢复供电处理
void dji_zkrt_read_heart_vol_check(void)
{
//	if (ADC1_5_dji < 4800)					  				//如果小于4.8V
//	{
  
//	{
//		turn_off_5V_power();
//		msg_smartbat_dji_buffer[29] &= 0XFC;	  
//		msg_smartbat_dji_buffer[29] |= 0X01;	
//	}
	 if (ADC1_5_dji > 5565)								//如果大于5.3V的5%
	{
		turn_off_5V_power();
		msg_smartbat_dji_buffer[29] &= 0XFC;		
		msg_smartbat_dji_buffer[29] |= 0X02;	
	}
	else
	{
		V5_error_flag_dji = 0;
		msg_smartbat_dji_buffer[29] &= 0XFC;		//如果电压正常
	}
	
	if (ADC1_25_dji < 20000)									//如果25V电源小于20V
	{
		msg_smartbat_dji_buffer[29] &= 0XF3;		
		msg_smartbat_dji_buffer[29] |= 0X04;	
	}
	else if (ADC1_25_dji > 26000)							//如果25V电源大于26V
	{
		msg_smartbat_dji_buffer[29] &= 0XF3;		
		msg_smartbat_dji_buffer[29] |= 0X08;		
	}
	else
	{
		msg_smartbat_dji_buffer[29] &= 0XF3;		
	}
	
	if (ADC1_I_dji > 3000)										//如果电流超过了3A，那么异常
	{
		turn_off_5V_power();
		msg_smartbat_dji_buffer[29] |= 0X10;	  
	}
	else
	{
		V5_error_flag_dji = 0;
		msg_smartbat_dji_buffer[29] &= 0XEF;	  //如果电流小于3A，那么正常
	}
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

void dji_zkrt_read_heart_tempture_check_send(uint8_t num)
{
	if (dji_tempture_error_flag[num] == 0)				
		{
			mavlink_send_flag = TimingDelay+500;	
			mavlink_type_flag_dji = 0;			
			dji_tempture_error_flag[num] = 1;				
		}
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/
//检查温度值的正确与否，并在对应的心跳数组里写入标志位
void dji_zkrt_read_heart_tempture_check(void)
{
      if (tempture0 < TEMPTURE_LOW_EXTRA) 
	{
		tempture0 = TEMPTURE_LOW_EXTRA;	 
		msg_smartbat_dji_buffer[0] = 0XFD;	 
		dji_zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 > TEMPTURE_HIGH_EXTRA)
	{
		tempture0 = TEMPTURE_HIGH_EXTRA;
		msg_smartbat_dji_buffer[0] = 0XFD;
		dji_zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 < glo_tempture_low)
	{
		msg_smartbat_dji_buffer[0] = 0XFB;								
		dji_zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 > glo_tempture_high)
	{
		msg_smartbat_dji_buffer[0] = 0XFC;								
		dji_zkrt_read_heart_tempture_check_send(0);
	}
	#if defined _TEMPTURE_IO_	
	else if (tempture0 < last_tempture0-TEMPTURE_DIFF)
	{
		msg_smartbat_dji_buffer[0] = 0XFE;
		tempture_change_too_fast_dji[0]++;
		if (tempture_change_too_fast_dji[0] == 2)
		{
			tempture_change_too_fast_dji[0] = 0;
			msg_smartbat_dji_buffer[0] = 0XFB;									
			dji_zkrt_read_heart_tempture_check_send(0);
		}
	}
	else if (tempture0 > last_tempture0+TEMPTURE_DIFF)	
	{
		msg_smartbat_dji_buffer[0] = 0XFE;
		tempture_change_too_fast_dji[1]++;
		if (tempture_change_too_fast_dji[1] == 2)
		{
			tempture_change_too_fast_dji[1] = 0;
			msg_smartbat_dji_buffer[0] = 0XFC;								
			dji_zkrt_read_heart_tempture_check_send(0);
		}
	}
	#endif	
	else
	{
	#if defined _TEMPTURE_IO_
		tempture_change_too_fast_dji[0] = 0;
		tempture_change_too_fast_dji[1] = 0;	
	#endif	
		dji_tempture_error_flag[0] = 0;						
		msg_smartbat_dji_buffer[0] = 0XFE;
	}
	

      if (tempture1 < TEMPTURE_LOW_EXTRA)
	{
		tempture1 = TEMPTURE_LOW_EXTRA;
		msg_smartbat_dji_buffer[3] = 0XFD;
		dji_zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 > TEMPTURE_HIGH_EXTRA)
	{
		tempture1 = TEMPTURE_HIGH_EXTRA;
		msg_smartbat_dji_buffer[3] = 0XFD;
		dji_zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 < glo_tempture_low)
	{
		msg_smartbat_dji_buffer[3] = 0XFB;									
		dji_zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 > glo_tempture_high)
	{
		msg_smartbat_dji_buffer[3] = 0XFC;									
		dji_zkrt_read_heart_tempture_check_send(1);
	}
	#if defined _TEMPTURE_IO_
	else if (tempture1 < last_tempture1-TEMPTURE_DIFF)	
	{
		msg_smartbat_dji_buffer[3] = 0XFE;
		tempture_change_too_fast_dji[2]++;
		if (tempture_change_too_fast_dji[2] == 2)
		{
			tempture_change_too_fast_dji[2] = 0;
			msg_smartbat_dji_buffer[3] = 0XFB;
			dji_zkrt_read_heart_tempture_check_send(1);
		}
	}
	else if (tempture1 > last_tempture1+TEMPTURE_DIFF)
	{
		msg_smartbat_dji_buffer[3] = 0XFE;
		tempture_change_too_fast_dji[3]++;
		if (tempture_change_too_fast_dji[3] == 2)
		{
			tempture_change_too_fast_dji[3] = 0;
			msg_smartbat_dji_buffer[3] = 0XFC;
			dji_zkrt_read_heart_tempture_check_send(1);
		}
	}
	#endif	
	else
	{
	#if defined _TEMPTURE_IO_
		tempture_change_too_fast_dji[2] = 0;
		tempture_change_too_fast_dji[3] = 0;
	#endif
		dji_tempture_error_flag[1] = 0;
		msg_smartbat_dji_buffer[3] = 0XFE;
	}
	#if defined _TEMPTURE_IO_	
	if ((tempture0 != 0XFFF)||(tempture1 != 0XFFF))
	{
		msg_smartbat_dji_buffer[23] |= 0X01;					
	}
	else
	{
		msg_smartbat_dji_buffer[23] &= 0XFE;					
	}
	
	last_tempture0 = tempture0;
	last_tempture1 = tempture1;
#elif defined _TEMPTURE_ADC_
//	if ((tempture0 > 50)&&(tempture1 > 50))  //zkrt_notice: 温度大于500摄氏度，表示AD检测异常
//	{
//		msg_smartbat_dji_buffer[23] |= 0X01;
//	}
//	else
//	{
//		msg_smartbat_dji_buffer[23] &= 0XFE; //AD采集温度数据异常时置0xFE 
//	}
	
	if((msg_smartbat_dji_buffer[0] != TEMP_INVALID)&&(msg_smartbat_dji_buffer[3] != TEMP_INVALID))  //zkrt_notice: 两个AD检测异常，置温度传感器不在线
	{
		msg_smartbat_dji_buffer[23] |= 0X01;
	}
	else
	{
		msg_smartbat_dji_buffer[23] &= 0XFE; //AD采集温度数据异常时置0xFE 
	}
#endif
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：心跳数组发送函数  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

//心跳数组填充完成后，利用该函数将数组发送出去
void dji_zkrt_read_heart_ack(void)
{
  zkrt_packet_t packet;

  packet.cmd = UAV_TO_APP;
  packet.command= DEFAULT_NUM;

  packet.UAVID[0] = now_uav_type;                 //哪种飞机
  packet.UAVID[1] = (uint8_t)(now_uav_num&0xff);	//飞机编号
  packet.UAVID[2] = (uint8_t)(now_uav_num>>8);
  packet.UAVID[3] = DEVICE_TYPE_HEART;						//设备类型
  packet.UAVID[4] = DEFAULT_NUM;									//设备编号
  packet.UAVID[5] = DEFAULT_NUM;

  msg_smartbat_dji_buffer[1] = (uint8_t)(tempture0&0xff);
  msg_smartbat_dji_buffer[2] = (uint8_t)(tempture0>>8);
  msg_smartbat_dji_buffer[4] = (uint8_t)(tempture1&0xff);
  msg_smartbat_dji_buffer[5] = (uint8_t)(tempture1>>8);

  //将温度上下限保存下来
  msg_smartbat_dji_buffer[6] = (uint8_t)(glo_tempture_low&0xff);
  msg_smartbat_dji_buffer[7] = (uint8_t)(glo_tempture_low>>8);
  msg_smartbat_dji_buffer[8] = (uint8_t)(glo_tempture_high&0xff);
  msg_smartbat_dji_buffer[9] = (uint8_t)(glo_tempture_high>>8);


  memcpy((void *)(packet.data), (void *)(msg_smartbat_dji_buffer), 30);

  zkrt_final_encode(&packet);
	
//  ZKRT_LOG(LOG_NOTICE,"sendpoll()$$$$$$$$!\r\n");
  sendToMobile((uint8_t*)&packet,50);
}
