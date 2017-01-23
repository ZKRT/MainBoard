#include "tempture.h"
#include "ds18b20.h"									
#include "adc.h"
#include "Mavlink_msg_statustext.h"		
#include "zkrt.h"											



uint8_t error_count = 0;

void zkrt_read_tempture_ack(void)
{
	uint8_t i;
	zkrt_packet_t packet;
	
#if defined _TEMPTURE_IO_	
	tempture0 = DS18B20_Get_Temp(DS18B20_NUM1);									
	tempture1 = DS18B20_Get_Temp(DS18B20_NUM2);
#elif defined _TEMPTURE_ADC_
	tempture0 = ADC1_get_value(_T1_value);								
	tempture1 = ADC1_get_value(_T1_value);
	ZKRT_LOG(LOG_NOTICE, "tempture0= %d   tempture1= %d!\r\n",tempture0,tempture1);
//	   printf("tempture0= %d   tempture1= %d!\r\n",tempture0,tempture1);
#endif
	
	if (tempture0 < TEMPTURE_LOW_EXTRA)
		tempture0 = TEMPTURE_LOW_EXTRA;
	if (tempture0 > TEMPTURE_HIGH_EXTRA)
		tempture0 = TEMPTURE_HIGH_EXTRA;
	if (tempture1 < TEMPTURE_LOW_EXTRA)
		tempture1 = TEMPTURE_LOW_EXTRA;
	if (tempture1 > TEMPTURE_HIGH_EXTRA)
		tempture1 = TEMPTURE_HIGH_EXTRA;
	
	packet.cmd = UAV_TO_APP;   
	packet.command= DEFAULT_NUM;
	
	packet.UAVID[0] = now_uav_type;                
	packet.UAVID[1] = (uint8_t)(now_uav_num&0xff);	
	packet.UAVID[2] = (uint8_t)(now_uav_num>>8);
	packet.UAVID[3] = DEVICE_TYPE_TEMPERATURE;		
	packet.UAVID[4] = DEFAULT_NUM;									
	packet.UAVID[5] = DEFAULT_NUM;
	
	packet.data[0] = 0XFE;
	packet.data[1] = (uint8_t)(tempture0&0xff);
	packet.data[2] = (uint8_t)(tempture0>>8);
	packet.data[3] = 0XFE;
	packet.data[4] = (uint8_t)(tempture1&0xff);
	packet.data[5] = (uint8_t)(tempture1>>8);
	
	for(i=6;i<25;i++)
	{
		packet.data[i] = 0;
	}
	
	
	if ((MAVLINK_TX_INIT_VAL-TimingDelay) > 5000)
	{
#if defined _TEMPTURE_IO_		
		if ((tempture0>last_tempture0+TEMPTURE_DIFF)||(tempture0<last_tempture0-TEMPTURE_DIFF)
		||(tempture1>last_tempture1+TEMPTURE_DIFF)||(tempture1<last_tempture1-TEMPTURE_DIFF))
		{
			error_count++;
			if (error_count == 2)
			{
				error_count = 0;
				packet.data[25] = 0XFD;
			}
			//packet.data[25] = 0XFD;
		}
		else //modify by yanly
		{
			error_count = 0;
			packet.data[25] = 0XFE;
		}
#elif defined _TEMPTURE_ADC_
		if (1 > 0)
		{
			packet.data[25] = 0XFE;
		}
#endif
		if ((tempture0>glo_tempture_high)||(tempture0<glo_tempture_low)||(tempture1>glo_tempture_high)||(tempture1<glo_tempture_low))
		{
			packet.data[25] = 0XFD;
		}
	}
	else
	{
		packet.data[25] = 0XFE;
	}
	
	last_tempture0 = tempture0;
	last_tempture1 = tempture1;
	
	packet.data[26] = (uint8_t)(glo_tempture_low&0xff);
	packet.data[27] = (uint8_t)(glo_tempture_low>>8);
	packet.data[28] = (uint8_t)(glo_tempture_high&0xff);
	packet.data[29] = (uint8_t)(glo_tempture_high>>8);
	
	zkrt_final_encode(&packet);
	
	mavlink_msg_statustext_send(MAVLINK_COMM_0, DEVICE_TYPE_TEMPERATURE, (const char*)(&packet));//Í¨¹ýstatus_text·¢ËÍpacket
}
