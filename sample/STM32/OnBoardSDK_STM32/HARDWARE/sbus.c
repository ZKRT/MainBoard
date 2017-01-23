#include "sbus.h"

#define EIGHT_CHANNELS 1
//#define ALL_CHANNELS   2
zkrt_sbus  tx_sbus;
zkrt_sbus  rx_sbus;

uint16_t tx_channel_in [16]={1500, 1500, 1500, 1500, 1000, 1000, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}; 
uint16_t tx_channel_out[16]={0};  																																															

uint16_t rx_channel_in [16]={1500, 1500, 1500, 1500, 1000, 1000, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}; 
uint16_t rx_channel_out[16]={0};  					  																																								 


//被调用情况：已经将所需的pwm值输入到channel_in里
//用于将通道值整理到tx_sbus里
uint8_t  sbus_send(void)
{
	uint8_t i=0;
	uint32_t temp = 0;
	uint8_t ch_count = 0;
	uint8_t data_count = 0;
	uint8_t rels_count = 0;
	
	memset((void *)(tx_sbus.data), 0, 22);									
	
  for(i=0;i<16;i++)  																					
	{
		tx_channel_out[i]=(tx_channel_in[i]-874)/5*8;							
	}
	
	tx_sbus.startbyte=0x0f;  																	

	//数据的初始化
	temp = tx_channel_out[0];																		
	ch_count = 1;																								
	data_count = 0;																							

#if defined EIGHT_CHANNELS
	while (ch_count < 9)																				
#elif defined ALL_CHANNELS
	while (ch_count < 17)																				
#endif		
	{
		if ((rels_count = ch_count*11-data_count*8) < 8)					
																														
		{
			temp = temp | (tx_channel_out[ch_count] << rels_count); 
			ch_count++;																							
		}
	
		tx_sbus.data[data_count] = temp&0XFF;                    
		data_count++;																							
		temp >>= 8;																								
	}
		
	tx_sbus.flags=0x00;
	tx_sbus.endbyte=0x00;
	
	return 0;
}


//被调用情况：接收到一个完整的sbus指令，并将其存在rx_sbus里
//这个与上面的相反，是一种sbus转化为ch的算法
uint8_t sbus_recv(void)
{
	uint8_t i=0;
	uint32_t temp = 0;
	uint8_t data_count = 0;
	uint8_t ch_count = 0;
	uint8_t rels_count = 0;
	
	memset((void *)(rx_channel_in), 0, 16);											
	
	//数据的初始化
	temp = rx_sbus.data[0] | (rx_sbus.data[1]<<8);							
	data_count = 2;																							
	ch_count = 0;																								
	
#if defined EIGHT_CHANNELS
	while (data_count < 12)																			
#elif defined ALL_CHANNELS
	while (data_count < 23)																			
#endif		
	{
		while ((rels_count= data_count*8 - ch_count*11) < 11)			
																															
		{
			temp = temp | (rx_sbus.data[data_count] << rels_count); 
			data_count++;																						
		}
		
		
		
		rx_channel_in[ch_count] = temp&0X7FF;                   
		ch_count++;																							
		temp >>= 11;																						
	}
	
	for(i=0;i<16;i++)  																					
	{
		rx_channel_out[i]=(rx_channel_in[i])*5/8+875;							
	}
	
	return 0;
}

	

