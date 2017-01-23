#include "app.h"

uint8_t heart_ack_count[DEVICE_NUMBER] = {0};

uint8_t heart_ack_status[DEVICE_NUMBER] = {0};

uint8_t which_byte = 0;
uint8_t which_bit  = 0;

uint8_t heart_ack_check(uint8_t device_id, uint8_t value)
{
	which_byte = (device_id-1)/8;			
	which_bit  = device_id%8 -1;		
	device_id -= 1;										
	
	switch (heart_ack_status[device_id])
	{
		case 0X00:
			if (value == 0XAA)
			{
				heart_ack_status[device_id] |= 1<<0;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X01:
			if (value == 0XBB)
			{
				heart_ack_status[device_id] |= 1<<1;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X03:
			if (value == 0XCC)
			{
				heart_ack_status[device_id] |= 1<<2;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X07:
			if (value == 0XDD)
			{
				heart_ack_status[device_id] |= 1<<3;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X0F:
			if (value == 0XEE)
			{
				heart_ack_status[device_id] |= 1<<4;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X1F:
			if (value == (device_id+1))				
			{
				heart_ack_status[device_id] |= 1<<5;
			}
			else
			{
				heart_ack_status[device_id] = 0;
			}
			break;
		case 0X3F:
			if (device_id == (DEVICE_TYPE_THROW-1))	//如果是抛投模块的话
			{
				msg_smartbat_buffer[26] &= 0XF8;			//将字26的低3位清空并赋值上传来的数据
				msg_smartbat_buffer[26] |= value;
			}
			heart_ack_status[device_id] |= 1<<6;												
			break;
		case 0X7F:
			if (value != heart_ack_count[device_id])								
			{
				heart_ack_count[device_id] = value;											
				
				msg_smartbat_buffer[23+which_byte] |= (1<<which_bit);		
				
				heart_ack_status[device_id] |= 1<<7;
			}
			break;
		default:
		{
			break;
		}
	}
			
	if (heart_ack_status[device_id] == 0XFF)
	{
		heart_ack_status[device_id] = 0;			
		return 1;
	}
	
	return 0;
}


zkrt_packet_t can1_rx_posion;
void main_zkrt_recv(void)
{
	uint8_t value;

	while (CAN1_rx_check(DEVICE_TYPE_GAS) == 1)
	{
		value = CAN1_rx_byte(DEVICE_TYPE_GAS);
		if (zkrt_decode_char(&can1_rx_posion,value)==1)
		{
			memcpy((void *)posion_buffer, (void *)&(can1_rx_posion.data[0]), 16);

			zkrt_read_heart_posion();						
			zkrt_read_heart_posion_check();			
			
			posion_recv_flag = TimingDelay;		
		}
	}
		
	while (CAN1_rx_check(DEVICE_TYPE_THROW) == 1)
	{
		value = CAN1_rx_byte(DEVICE_TYPE_THROW);
		if (heart_ack_check(DEVICE_TYPE_THROW, value)==1)
		{
			throw_recv_flag = TimingDelay;												
		}
	}
	
	while (CAN1_rx_check(DEVICE_TYPE_CAMERA) == 1)
	{
		value = CAN1_rx_byte(DEVICE_TYPE_CAMERA);
		if (heart_ack_check(DEVICE_TYPE_CAMERA, value)==1)
		{
			camera_recv_flag = TimingDelay;										
		}
	}
}

mavlink_message_t main_mavlink_msg;                  
mavlink_status_t main_mavlink_status;             
mavlink_statustext_t main_mavlink_payload;				
zkrt_packet_t main_mavlink_statustext;   

#if 0
void main_mavlink_recv(void)
{
	uint8_t value;
	
	while (usart1_rx_check() == 1)
	{
		value = usart1_rx_byte();
		if (mavlink_parse_char(MAVLINK_COMM_0, value, &main_mavlink_msg, &main_mavlink_status)==1)
		{
			switch (main_mavlink_msg.msgid)
			{
				case MAVLINK_MSG_ID_STATUSTEXT:
				{
					main_mavlink_payload.severity = mavlink_msg_statustext_get_severity(&main_mavlink_msg);                 
					mavlink_msg_statustext_get_text(&main_mavlink_msg,main_mavlink_payload.text);                          
					memcpy((void *)&main_mavlink_statustext, (void *)&(main_mavlink_payload.text[0]), 50);       				
						
					if (main_mavlink_statustext.UAVID[3] == DEVICE_TYPE_TEMPERATURE)																				
					{
						tempture_low =  ((main_mavlink_statustext.data[1])<<8)+(main_mavlink_statustext.data[0]);				
						tempture_high = ((main_mavlink_statustext.data[3])<<8)+(main_mavlink_statustext.data[2]);
					}
					else if (main_mavlink_statustext.UAVID[3] == DEVICE_TYPE_BAOSHAN)																		
					{
						if (main_mavlink_statustext.data[0] == 0X00)		
						{
							GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_9);	
							delay_ms(1000);											
							msg_smartbat_buffer[24] &= 0XF7;			
						}
						else if (main_mavlink_statustext.data[0] == 0X01)	
						{
							GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_9);	
							delay_ms(1000);
							msg_smartbat_buffer[24] |= 0X08;			
						}
					}
					else
					{
						CAN1_send_message_fun((uint8_t *)(&main_mavlink_statustext), _TOTAL_LEN, (main_mavlink_statustext.UAVID[3]));
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
}

#endif



























