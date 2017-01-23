/*
 *	File name   : mobile_data_handle.c
 *  Created on  : 
 *  Author      : 
 *  Description :
 *  Version     :
 *  History     : <author>		<time>		<version>		<desc>
 */

#include "sys.h"
#include "adc.h"
#include "usart.h"
#include "zkrt.h"
#include "djiapp.h"
#include "flash.h"
#include "bat.h"
#include "mobileDataHandle.h"

/////////////////////////////////////////////////////////////////////valible define 
uint8_t djidataformmobile[50]= {0};
zkrt_packet_t main_dji_rev;

////////////////////////////////////////////////////////////////////static function define
static void main_dji_recv(void);
static void copydataformmobile(void);

/*
* @brief �����յ���mobile͸�����ݽ��н�������
	mobile������Դ��DJI����͸�����ݣ�����UART1�жϽ��ս��������������������djidataformmobile[]
*/
void mobile_data_process(void)
{
	//��ȡ�ӵ���վ���������
	if(djidataformmobile[1]!=0)
	{
		copydataformmobile();
		main_dji_recv();
		djidataformmobile[1] =0;
	}
}
/***********************************************************************
		          �п���̩�������˻���������

		���� ���ƣ����մ�����
	       ���� ���ܣ�
		���� ��Σ�
		��������ֵ��
		˵     ����

************************************************************************/
void main_dji_recv(void)
{
	int k;
	//memcpy((void *)&main_dji_rev, (void *)&(Rx_buff[5]), 40);
//  printf("***************main_dji_revdata_form_mobile************************\r\n");
//  printf("main_dji_rev.start_code=0x %x\r\n",main_dji_rev.start_code);
//  printf("main_dji_rev.ver=0x %x\r\n",main_dji_rev.ver);
//  printf("main_dji_rev.session_ack=0x %x \r\n",main_dji_rev.session_ack);
//  printf("main_dji_rev.padding_enc=0x %x\r\n",main_dji_rev.cmd);
//  printf("main_dji_rev.cmd=0x %x\r\n",main_dji_rev.ver);
//  printf("main_dji_rev.length=0x %x\r\n",main_dji_rev.length);
//  printf("main_dji_rev.seq=0x %x\r\n",main_dji_rev.seq);
//  printf("main_dji_rev.APPID[0]=0x %x \r\n",main_dji_rev.APPID[0]);
//  printf("main_dji_rev.APPID[1]=0x %x \r\n",main_dji_rev.APPID[1]);
//  printf("main_dji_rev.APPID[2]=0x %x \r\n",main_dji_rev.APPID[2]);
//  printf("main_dji_rev.UAVID[0]=0x %x \r\n",main_dji_rev.UAVID[0]);
//  printf("main_dji_rev.UAVID[1]=0x %x\r\n",main_dji_rev.UAVID[1]);
//  printf("main_dji_rev.UAVID[2]=0x %x\r\n",main_dji_rev.UAVID[2]);
//  printf("main_dji_rev.UAVID[3]=0x %x\r\n",main_dji_rev.UAVID[3]);
//  printf("main_dji_rev.UAVID[4]=0x %x\r\n",main_dji_rev.UAVID[4]);
//  printf("main_dji_rev.UAVID[5]=0x %x\r\n",main_dji_rev.UAVID[5]);
//  printf("main_dji_rev.command=0x %x \r\n",main_dji_rev.command);
  ZKRT_LOG(LOG_NOTICE, "***************main_dji_revdata_form_mobile************************\r\n");
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.start_code=0x %x\r\n",main_dji_rev.start_code);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.ver=0x %x\r\n",main_dji_rev.ver);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.session_ack=0x %x \r\n",main_dji_rev.session_ack);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.padding_enc=0x %x\r\n",main_dji_rev.cmd);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.cmd=0x %x\r\n",main_dji_rev.ver);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.length=0x %x\r\n",main_dji_rev.length);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.seq=0x %x\r\n",main_dji_rev.seq);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.APPID[0]=0x %x \r\n",main_dji_rev.APPID[0]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.APPID[1]=0x %x \r\n",main_dji_rev.APPID[1]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.APPID[2]=0x %x \r\n",main_dji_rev.APPID[2]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[0]=0x %x \r\n",main_dji_rev.UAVID[0]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[1]=0x %x\r\n",main_dji_rev.UAVID[1]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[2]=0x %x\r\n",main_dji_rev.UAVID[2]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[3]=0x %x\r\n",main_dji_rev.UAVID[3]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[4]=0x %x\r\n",main_dji_rev.UAVID[4]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.UAVID[5]=0x %x\r\n",main_dji_rev.UAVID[5]);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.command=0x %x \r\n",main_dji_rev.command);
	
  for(k=0; k<30; k++)
  {
    ZKRT_LOG(LOG_NOTICE,"main_dji_rev.data[%d]=0x %x\r\n",k,main_dji_rev.data[k]);
  }
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.crc=0x %x\r\n",main_dji_rev.crc);
  ZKRT_LOG(LOG_NOTICE,"main_dji_rev.end_code=0x %x\r\n",main_dji_rev.end_code);

  if (main_dji_rev.UAVID[3] == DEVICE_TYPE_TEMPERATURE) /*�����¶����ֵ�����ֵ*/
  {
    ZKRT_LOG(LOG_NOTICE,"&&&&&&&&&&&&&&\r\n");
    glo_tempture_low =  ((main_dji_rev.data[1])<<8)+(main_dji_rev.data[0]);
    glo_tempture_high = ((main_dji_rev.data[3])<<8)+(main_dji_rev.data[2]);
  }
  else if (main_dji_rev.UAVID[3] == DEVICE_TYPE_BAOSHAN)/*���ñ�����*/
  {
    ZKRT_LOG(LOG_NOTICE,"###############\r\n");
    if (main_dji_rev.data[0] == 0X00)
    {
      GPIO_ResetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_9);
      delay_ms(1000);
      msg_smartbat_dji_buffer[24] &= 0XF7;
    }
    else if (main_dji_rev.data[0] == 0X01)
    {
      GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_9);
      delay_ms(1000);
      msg_smartbat_dji_buffer[24] |= 0X08;
    }
  }
  else
  {
    ZKRT_LOG(LOG_NOTICE,"CAN1_send_message_fun\r\n");
    CAN1_send_message_fun((uint8_t *)(&main_dji_rev), _TOTAL_LEN, (main_dji_rev.UAVID[3]));/*ͨ��CAN���߷�������*/
  }
}
/* 
* @brief: mobile�������� ת�浽 zkrt_packet_t��ʽ��ȫ�ֽṹ����
*/
void copydataformmobile(void)
{
	u32 j;
	main_dji_rev.start_code=djidataformmobile[0];
	main_dji_rev.ver=djidataformmobile[1];
	main_dji_rev.session_ack=djidataformmobile[2];
	main_dji_rev.padding_enc=djidataformmobile[3];
	main_dji_rev.cmd=djidataformmobile[4];
	main_dji_rev.length=djidataformmobile[5];
	main_dji_rev.seq=djidataformmobile[6];
	main_dji_rev.APPID[0]=djidataformmobile[7];
	main_dji_rev.APPID[1]=djidataformmobile[8];
	main_dji_rev.APPID[2]=djidataformmobile[9];
	for(j=0;j<6;j++)
	{
		main_dji_rev.UAVID[j]=djidataformmobile[10+j];
	}
	main_dji_rev.command=djidataformmobile[16]; //modify by yanly1221
	for(j=0;j<30;j++)
	{
		main_dji_rev.data[j]=djidataformmobile[17+j];
	}
	main_dji_rev.crc=djidataformmobile[47];
	main_dji_rev.crc|=djidataformmobile[48]<<8;
	main_dji_rev.end_code=djidataformmobile[49];
}
