#include "heart.h"
#include "ds18b20.h"
#include "adc.h"
#include "Mavlink_msg_statustext.h"
#include "zkrt.h"
#include "tempture.h"


uint16_t ADC1_25;
uint16_t ADC1_5;
uint16_t ADC1_I;

uint8_t V5_error_flag = 0;

//关闭5V电源
void turn_off_5V_power(void)
{
	if (V5_error_flag == 0)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);	//关闭5V电源的使能
		delay_ms(1000);
	}
	V5_error_flag = 1;
}

//打开5V电源
void turn_on_5V_power(void)
{
	if (V5_error_flag == 1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_8);	//打开5V电源的使能
		delay_ms(1000);
	}
	V5_error_flag = 0;
}

//将当前的板载电压值进行判断，将异常情况写入到心跳数组里，并且执行断电或恢复供电处理
void zkrt_read_heart_vol_check(void)
{
	if (ADC1_5 < 4800)					  				//如果小于4.8V
	{
		turn_off_5V_power();
		msg_smartbat_buffer[29] &= 0XFC;
		msg_smartbat_buffer[29] |= 0X01;
	}
	else if (ADC1_5 > 5565)								//如果大于5.3V的5%
	{
		turn_off_5V_power();
		msg_smartbat_buffer[29] &= 0XFC;
		msg_smartbat_buffer[29] |= 0X02;
	}
	else
	{
		V5_error_flag = 0;
		msg_smartbat_buffer[29] &= 0XFC;		//如果电压正常
	}

	if (ADC1_25 < 20000)									//如果25V电源小于20V
	{
		msg_smartbat_buffer[29] &= 0XF3;
		msg_smartbat_buffer[29] |= 0X04;
	}
	else if (ADC1_25 > 26000)							//如果25V电源大于26V
	{
		msg_smartbat_buffer[29] &= 0XF3;
		msg_smartbat_buffer[29] |= 0X08;
	}
	else
	{
		msg_smartbat_buffer[29] &= 0XF3;
	}

	if (ADC1_I > 3000)										//如果电流超过了3A，那么异常
	{
		turn_off_5V_power();
		msg_smartbat_buffer[29] |= 0X10;
	}
	else
	{
		V5_error_flag = 0;
		msg_smartbat_buffer[29] &= 0XEF;	  //如果电流小于3A，那么正常
	}
}


//读取板载25V电压、5V电压、5V电流
void zkrt_read_heart_vol(void)
{
	ADC1_25 = ADC1_get_value(_25V_value);
	ADC1_5  = ADC1_get_value(_5V_value);
	ADC1_I  = ADC1_get_value(_5A_value);
}

uint8_t posion_buffer[16] = {0};

void zkrt_read_heart_posion_check(void)
{
	//填充数组的第10位，记录状态
	if (posion_buffer[0] == 0XFE)			//如果正常，那么置一；如果异常，那么置零
	{
		msg_smartbat_buffer[10] |= 0X01;
	}
	else if (posion_buffer[0] == 0XFD)
	{
		msg_smartbat_buffer[10] &= 0XFE;
	}

	if (posion_buffer[4] == 0XFE)
	{
		msg_smartbat_buffer[10] |= 0X02;
	}
	else if (posion_buffer[4] == 0XFD)
	{
		msg_smartbat_buffer[10] &= 0XFD;
	}

	if (posion_buffer[8] == 0XFE)
	{
		msg_smartbat_buffer[10] |= 0X04;
	}
	else if (posion_buffer[8] == 0XFD)
	{
		msg_smartbat_buffer[10] &= 0XFB;
	}

	if (posion_buffer[12] == 0XFE)
	{
		msg_smartbat_buffer[10] |= 0X08;
	}
	else if (posion_buffer[12] == 0XFD)
	{
		msg_smartbat_buffer[10] &= 0XF7;
	}
}

void zkrt_read_heart_posion(void)
{
	msg_smartbat_buffer[11] = posion_buffer[1];
	msg_smartbat_buffer[12] = posion_buffer[2];
	msg_smartbat_buffer[13] = posion_buffer[3];

	msg_smartbat_buffer[14] = posion_buffer[5];
	msg_smartbat_buffer[15] = posion_buffer[6];
	msg_smartbat_buffer[16] = posion_buffer[7];

	msg_smartbat_buffer[17] = posion_buffer[9];
	msg_smartbat_buffer[18] = posion_buffer[10];
	msg_smartbat_buffer[19] = posion_buffer[11];

	msg_smartbat_buffer[20] = posion_buffer[13];
	msg_smartbat_buffer[21] = posion_buffer[14];
	msg_smartbat_buffer[22] = posion_buffer[15];

	if ((msg_smartbat_buffer[12] != 0) || (msg_smartbat_buffer[13] != 0) || (msg_smartbat_buffer[15] != 0) || (msg_smartbat_buffer[16] != 0)
	        || (msg_smartbat_buffer[18] != 0) || (msg_smartbat_buffer[19] != 0) || (msg_smartbat_buffer[21] != 0) || (msg_smartbat_buffer[22] != 0))
	{
		msg_smartbat_buffer[23] |= 0X10;
	}
}

#if defined _TEMPTURE_IO_
uint8_t tempture_change_too_fast[4] = {0};
#endif
uint8_t tempture_error_flag[2] = {0};
uint8_t mavlink_type_flag = 0;
void zkrt_read_heart_tempture_check_send(uint8_t num)
{
	if (tempture_error_flag[num] == 0)
	{
		mavlink_send_flag = TimingDelay + 500;
		mavlink_type_flag = 0;
		tempture_error_flag[num] = 1;
	}
}

#if 0
//检查温度值的正确与否，并在对应的心跳数组里写入标志位
void zkrt_read_heart_tempture_check(void)
{
	if (tempture0 < TEMPTURE_LOW_EXTRA)
	{
		tempture0 = TEMPTURE_LOW_EXTRA;
		msg_smartbat_buffer[0] = 0XFD;
		zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 > TEMPTURE_HIGH_EXTRA)
	{
		tempture0 = TEMPTURE_HIGH_EXTRA;
		msg_smartbat_buffer[0] = 0XFD;
		zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 < glo_tempture_low)
	{
		msg_smartbat_buffer[0] = 0XFB;									//温度超出下限
		zkrt_read_heart_tempture_check_send(0);
	}
	else if (tempture0 > glo_tempture_high)
	{
		msg_smartbat_buffer[0] = 0XFC;									//温度超出上限
		zkrt_read_heart_tempture_check_send(0);
	}
#if defined _TEMPTURE_IO_
	else if (tempture0 < last_tempture0 - TEMPTURE_DIFF)
	{
		msg_smartbat_buffer[0] = 0XFE;
		tempture_change_too_fast[0]++;
		if (tempture_change_too_fast[0] == 2)
		{
			tempture_change_too_fast[0] = 0;
			msg_smartbat_buffer[0] = 0XFB;
			zkrt_read_heart_tempture_check_send(0);
		}
	}
	else if (tempture0 > last_tempture0 + TEMPTURE_DIFF)
	{
		msg_smartbat_buffer[0] = 0XFE;
		tempture_change_too_fast[1]++;
		if (tempture_change_too_fast[1] == 2)
		{
			tempture_change_too_fast[1] = 0;
			msg_smartbat_buffer[0] = 0XFC;
			zkrt_read_heart_tempture_check_send(0);
		}
	}
#endif
	else
	{
#if defined _TEMPTURE_IO_
		tempture_change_too_fast[0] = 0;
		tempture_change_too_fast[1] = 0;
#endif
		tempture_error_flag[0] = 0;
		msg_smartbat_buffer[0] = 0XFE;
	}


	if (tempture1 < TEMPTURE_LOW_EXTRA)
	{
		tempture1 = TEMPTURE_LOW_EXTRA;
		msg_smartbat_buffer[3] = 0XFD;
		zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 > TEMPTURE_HIGH_EXTRA)
	{
		tempture1 = TEMPTURE_HIGH_EXTRA;
		msg_smartbat_buffer[3] = 0XFD;
		zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 < glo_tempture_low)
	{
		msg_smartbat_buffer[3] = 0XFB;									//温度超出下限
		zkrt_read_heart_tempture_check_send(1);
	}
	else if (tempture1 > glo_tempture_high)
	{
		msg_smartbat_buffer[3] = 0XFC;
		zkrt_read_heart_tempture_check_send(1);
	}
#if defined _TEMPTURE_IO_
	else if (tempture1 < last_tempture1 - TEMPTURE_DIFF)
	{
		msg_smartbat_buffer[3] = 0XFE;
		tempture_change_too_fast[2]++;
		if (tempture_change_too_fast[2] == 2)
		{
			tempture_change_too_fast[2] = 0;
			msg_smartbat_buffer[3] = 0XFB;
			zkrt_read_heart_tempture_check_send(1);
		}
	}
	else if (tempture1 > last_tempture1 + TEMPTURE_DIFF)
	{
		msg_smartbat_buffer[3] = 0XFE;
		tempture_change_too_fast[3]++;
		if (tempture_change_too_fast[3] == 2)
		{
			tempture_change_too_fast[3] = 0;
			msg_smartbat_buffer[3] = 0XFC;
			zkrt_read_heart_tempture_check_send(1);
		}
	}
#endif
	else
	{
#if defined _TEMPTURE_IO_
		tempture_change_too_fast[2] = 0;
		tempture_change_too_fast[3] = 0;
#endif
		tempture_error_flag[1] = 0;
		msg_smartbat_buffer[3] = 0XFE;
	}

#if defined _TEMPTURE_IO_
	if ((tempture0 != 0XFFF) || (tempture1 != 0XFFF))
	{
		msg_smartbat_buffer[23] |= 0X01;
	}
	else
	{
		msg_smartbat_buffer[23] &= 0XFE;
	}

	last_tempture0 = tempture0;
	last_tempture1 = tempture1;
#elif defined _TEMPTURE_ADC_
	if ((tempture0 > 50) && (tempture1 > 50))
	{
		msg_smartbat_buffer[23] |= 0X01;
	}
	else
	{
		msg_smartbat_buffer[23] &= 0XFE;
	}
#endif
}




//读取温度值，并且将温度值、温度上下限填充到心跳数组
void zkrt_read_heart_tempture(void)
{
#if defined _TEMPTURE_IO_
	tempture0 = DS18B20_Get_Temp(DS18B20_NUM1);
	tempture1 = DS18B20_Get_Temp(DS18B20_NUM2);
	if ((tempture0 == 0XFFF) || (tempture1 == 0XFFF))
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;	//modify by yanly
	}
	else
	{
//		GPIO_SetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 1;
	}
#elif defined _TEMPTURE_ADC_
	tempture0 = ADC1_get_value(_T1_value);
	tempture1 = ADC1_get_value(_T1_value);
	ZKRT_LOG(LOG_NOTICE, "tempture0= %d   tempture1= %d!\r\n", tempture0, tempture1);
#endif
}

#endif
/****************************************心跳数组发送函数*********************************************/
//心跳数组填充完成后，利用该函数将数组发送出去
void zkrt_read_heart_ack(void)
{
	zkrt_packet_t packet;

	packet.cmd = UAV_TO_APP;
	packet.command = DEFAULT_NUM;

	packet.UAVID[0] = now_uav_type;                 //哪种飞机
	packet.UAVID[1] = (uint8_t)(now_uav_num & 0xff);	//飞机编号
	packet.UAVID[2] = (uint8_t)(now_uav_num >> 8);
	packet.UAVID[3] = DEVICE_TYPE_HEART;						//设备类型
	packet.UAVID[4] = DEFAULT_NUM;									//设备编号
	packet.UAVID[5] = DEFAULT_NUM;

	msg_smartbat_buffer[1] = (uint8_t)(tempture0 & 0xff);
	msg_smartbat_buffer[2] = (uint8_t)(tempture0 >> 8);
	msg_smartbat_buffer[4] = (uint8_t)(tempture1 & 0xff);
	msg_smartbat_buffer[5] = (uint8_t)(tempture1 >> 8);

	//将温度上下限保存下来
	msg_smartbat_buffer[6] = (uint8_t)(glo_tempture_low & 0xff);
	msg_smartbat_buffer[7] = (uint8_t)(glo_tempture_low >> 8);
	msg_smartbat_buffer[8] = (uint8_t)(glo_tempture_high & 0xff);
	msg_smartbat_buffer[9] = (uint8_t)(glo_tempture_high >> 8);


	memcpy((void *)(packet.data), (void *)(msg_smartbat_buffer), 30);

	zkrt_final_encode(&packet);

	mavlink_msg_statustext_send(MAVLINK_COMM_0, DEVICE_TYPE_HEART, (const char*)(&packet));//通过status_text发送packet
}


