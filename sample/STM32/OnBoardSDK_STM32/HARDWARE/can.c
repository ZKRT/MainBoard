/**
  ******************************************************************************
  * @file    can.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   CAN����
  *           + .. 
  *           + [2] CAN1H��PA12�޸�ΪPB9, CAN1L��PA11�޸�ΪPB8(��ΪоƬ��100pin��VET6����144pin��ZET6); --161213 by yanly
	*           + [3] add CAN2H-PB13, CAN2L-PB12
  *         
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */ 
	
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "zkrt.h"		
#include "led.h"

/* Private variables ---------------------------------------------------------*/

volatile uint8_t can1_rx_buff[DEVICE_NUMBER][CAN_BUFFER_SIZE];
volatile uint16_t can1_rx_buff_store[DEVICE_NUMBER]; //�����ﱣ��ÿ��CAN�豸�������ݵ��ֽ��������ܵ����ݺ�can1_rx_buff_store[type]++һֱ�ۼӣ�����CAN_BUFFER_SIZE������0�����ۼ�
uint16_t can1_rx_buff_get[DEVICE_NUMBER]; //ÿ����һ��can1_rx_buff[type][can1_rx_buff_store[type]]��can1_rx_buff_get++�ۼӣ�ѭ���ۼ�
#ifdef USE_CAN2_FUN
#define CAN2_IDNUMBER 1
volatile uint8_t can2_rx_buff[CAN2_IDNUMBER][CAN_BUFFER_SIZE];
volatile uint16_t can2_rx_buff_store[CAN2_IDNUMBER]; //�����ﱣ��ÿ��CAN�豸�������ݵ��ֽ��������ܵ����ݺ�can1_rx_buff_store[type]++һֱ�ۼӣ�����CAN_BUFFER_SIZE������0�����ۼ�
uint16_t can2_rx_buff_get[CAN2_IDNUMBER]; //ÿ����һ��can1_rx_buff[type][can1_rx_buff_store[type]]��can1_rx_buff_get++�ۼӣ�ѭ���ۼ�
#endif
/* Private functions ---------------------------------------------------------*/

void can_all_init(void)
{
	CAN1_Mode_Init(CAN_Mode_Normal);
#ifdef USE_CAN2_FUN	
	CAN2_Mode_Init(CAN_Mode_Normal);
#endif
}	
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
uint8_t CAN1_Mode_Init(uint8_t mode)
{
	GPIO_InitTypeDef 			 GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9| GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); 
	
	CAN_DeInit(CAN1);
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;	
	CAN_InitStructure.CAN_RFLM=DISABLE;	
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode= mode;	 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler=7;
	CAN_Init(CAN1, &CAN_InitStructure);   
	
	//���ù����� for can1
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=~(0XF0<<5);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
#ifdef USE_CAN2_FUN		
	//���ù����� for can2
	CAN_SlaveStartBank(13);  //����can2�ӿ���ʼ�洢��
	CAN_FilterInitStructure.CAN_FilterNumber=13;	//��ʼ�洢����ʼ�����ֵ��ѡ��һ��  
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=~(0XF0<<5);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
#endif	

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVICX_init(NVIC_PPRIORITY_CAN, NVIC_SUBPRIORITY_CAN);
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����    
	
	return 0;
} 

#ifdef __cplusplus  //zkrt_test
extern "C" {
#endif //__cplusplus
void CAN1_RX0_IRQHandler(void)
{
	uint8_t i;
	uint8_t can1_rx_type;
	CanRxMsg RxMessage;
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	can1_rx_type = ((RxMessage.StdId)>>4) - 1;
	
	if(can1_rx_type > DEVICE_NUMBER-1)  
		return;
	
	for (i = 0; i < RxMessage.DLC; i++)
	{
		can1_rx_buff[can1_rx_type][can1_rx_buff_store[can1_rx_type]] = RxMessage.Data[i];
		
		can1_rx_buff_store[can1_rx_type]++;
		if (can1_rx_buff_store[can1_rx_type] == CAN_BUFFER_SIZE)
		{
			can1_rx_buff_store[can1_rx_type] = 0;
		}
	}
	
//	GPIO_ResetBits(GPIOD, GPIO_Pin_4);   //modify by yanly
	_CAN_RX_LED = 0;
	can_rx_flag = TimingDelay;				
}
#ifdef __cplusplus
}
#endif //__cplusplus

uint8_t CAN1_rx_check(uint8_t can1_rx_type)
{
//	can1_rx_type--;
	if (can1_rx_buff_store[can1_rx_type] == can1_rx_buff_get[can1_rx_type])
		return 0;
	else 
		return 1;
}


uint8_t CAN1_rx_byte(uint8_t can1_rx_type)
{
	uint8_t ch;	
//	can1_rx_type--;
	ch = can1_rx_buff[can1_rx_type][can1_rx_buff_get[can1_rx_type]];
	
	can1_rx_buff_get[can1_rx_type]++;
	if (can1_rx_buff_get[can1_rx_type] == CAN_BUFFER_SIZE)
	{
		can1_rx_buff_get[can1_rx_type] = 0;
	}

	return ch;
}

uint8_t CAN1_send_msg(uint8_t* msg,uint8_t len,uint8_t id)
{	
  uint8_t mbox;
  uint16_t i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 
  TxMessage.ExtId=0x00;	 
  TxMessage.IDE=CAN_Id_Standard;		 
  TxMessage.RTR=CAN_RTR_Data;		  
  TxMessage.DLC=len;							
  for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];				           
  mbox= CAN_Transmit(CAN1, &TxMessage);
//  printf("2222222222222222222222!\r\n");
  i=0;
  while((CAN_TransmitStatus(CAN1, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))
		i++;	
//	printf("33333333333333333333333!\r\n");	
	if(i>=0XFFF)//����1ʧ��
	{
//	printf("4444444444444444444444444!\r\n");
		CAN1_Mode_Init(CAN_Mode_Normal);
		return 1;
	}
//	printf("555555555555555555!\r\n");
//	GPIO_ResetBits(GPIOD, GPIO_Pin_5);//����CAN_TX //modify by yanly
	_CAN_TX_LED = 0;
	can_tx_flag = TimingDelay;				
	
  return 0;//����0�ɹ�
}


uint8_t CAN1_send_message_fun(uint8_t *message, uint8_t len, uint8_t id)
{
	uint8_t count;		              
	uint8_t time;
	
	time = len/8;            
//	printf("1111111111111111111111\r\n");
	for (count = 0; count < time; count++)
	{
		CAN1_send_msg(message, 8, id);
		message += 8;
		delay_ms(2);  //zkrt_notice: ��ʱ2ms  ���ٵ�
	}
	if (len%8)                          
	{
		CAN1_send_msg(message, len%8, id);
	}
	
	return 0;
}

#ifdef USE_CAN2_FUN	
/** @fungroup  can2 funciton  //add by yanly
  * @{
  */ 
uint8_t CAN2_Mode_Init(uint8_t mode)
{
	GPIO_InitTypeDef 			 GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
//	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	//ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

	//��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_CAN2); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_CAN2); 
	
	CAN_DeInit(CAN2);
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;	
	CAN_InitStructure.CAN_RFLM=DISABLE;	
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode= mode;	 
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler=7;
	CAN_Init(CAN2, &CAN_InitStructure);   
	
	//���ù�����
	//	CAN_SlaveStartBank(13);  //zkrt_notice: ��������can2�ò��ˣ�δ֪ԭ��
//	CAN_FilterInitStructure.CAN_FilterNumber=13;	  
//	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
//	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 
//	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
//	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=~(0XF0<<5);
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0xFFFF;
//	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
//	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; 
//	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVICX_init(NVIC_PPRIORITY_CAN2, NVIC_SUBPRIORITY_CAN2);
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����    
	
	return 0;
}   
#ifdef __cplusplus  //zkrt_test
extern "C" {
#endif //__cplusplus
void CAN2_RX0_IRQHandler(void)
{
	uint8_t i;
	uint8_t can2_rx_type;
	CanRxMsg RxMessage;
	
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	can2_rx_type = ((RxMessage.StdId)>>4) - 1;
	
	if(can2_rx_type > CAN2_IDNUMBER-1) 
		return;
	
	for (i = 0; i < RxMessage.DLC; i++)
	{
		can2_rx_buff[can2_rx_type][can2_rx_buff_store[can2_rx_type]] = RxMessage.Data[i];
		
		can2_rx_buff_store[can2_rx_type]++;
		if (can2_rx_buff_store[can2_rx_type] == CAN_BUFFER_SIZE)
		{
			can2_rx_buff_store[can2_rx_type] = 0;
		}
	}
//	_CAN2_RX_LED = 0;
//	can2_rx_flag = TimingDelay;				
}
#ifdef __cplusplus
}
#endif //__cplusplus

uint8_t CAN2_rx_check(uint8_t can2_rx_type)
{
	can2_rx_type--;
	if (can2_rx_buff_store[can2_rx_type] == can2_rx_buff_get[can2_rx_type])
		return 0;
	else 
	{
		if(obstacle_avoidance_rx_flag >= TimingDelay+500)  //zkrt_notice: ���ݽ���̫�죬��Ƶ�������������Ϊ��ʱ1s����
		{	
			_OBSTACLE_AVOIDANCE_RX_LED = 0;
			obstacle_avoidance_rx_flag = TimingDelay;
		}
		return 1;
	}	
}


uint8_t CAN2_rx_byte(uint8_t can2_rx_type)
{
	uint8_t ch;	
	can2_rx_type--;
	ch = can2_rx_buff[can2_rx_type][can2_rx_buff_get[can2_rx_type]];
	
	can2_rx_buff_get[can2_rx_type]++;
	if (can2_rx_buff_get[can2_rx_type] == CAN_BUFFER_SIZE)
	{
		can2_rx_buff_get[can2_rx_type] = 0;
	}

	return ch;
}

uint8_t CAN2_send_msg(uint8_t* msg,uint8_t len,uint8_t id)
{	
  uint8_t mbox;
  uint16_t i=0;
  CanTxMsg TxMessage;
  TxMessage.StdId=id;	 
  TxMessage.ExtId=0x00;	 
  TxMessage.IDE=CAN_Id_Standard;		 
  TxMessage.RTR=CAN_RTR_Data;		  
  TxMessage.DLC=len;							
  for(i=0;i<len;i++)
		TxMessage.Data[i]=msg[i];				           
  mbox= CAN_Transmit(CAN2, &TxMessage);
  i=0;
  while((CAN_TransmitStatus(CAN2, mbox)!=CAN_TxStatus_Ok)&&(i<0XFFF))
		i++;	
	if(i>=0XFFF)//����1ʧ��
	{
		CAN2_Mode_Init(CAN_Mode_Normal);
		return 1;
	}
//	_CAN2_TX_LED = 0;
//	ca2_tx_flag = TimingDelay;				
	
  return 0;//����0�ɹ�
}


uint8_t CAN2_send_message_fun(uint8_t *message, uint8_t len, uint8_t id)
{
	uint8_t count;		              
	uint8_t time;
	
	time = len/8;            
	for (count = 0; count < time; count++)
	{
		CAN2_send_msg(message, 8, id);
		message += 8;
		delay_ms(2);  //zkrt_notice: ��ʱ2ms  ���ٵ�
	}
	if (len%8)                          
	{
		CAN2_send_msg(message, len%8, id);
	}
	
	return 0;
}
#endif
/**
  * @}
  */ 
