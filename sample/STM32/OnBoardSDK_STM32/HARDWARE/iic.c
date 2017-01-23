/**
  ******************************************************************************
  * @file    ii.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   IIC
  *           + .. 
  *           + [2] �ɵ�IIC2��ΪIIC3����SDA����PB9��ΪPC9��SCL����PB8��ΪPA8;  
	*																														--161213 by yanly
  *         
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */ 

#include "iic.h"
#include "led.h"

uint8_t  I2CPEC=0;

uint8_t const Pectable[]=
{
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
};

#define WAIT_KEEP      122	
#define WAIT_KEEPHF    75		
#define WAIT_TURN			 252000

//��ʼ��IIC��ѡ��©
void IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;

//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	IIC_SCL=1;
	IIC_SDA=1;
}


/****************************************************
�������ƣ�Wait(uint t)
�������ܣ��ȴ���ʱ
���������
���������
#if CPUHZ==16  
#elif CPUHZ==8
#define Wait(a) 
#endif
*****************************************************/

void Wait(uint16_t t)
{
  while(t--);
}

/****************************************************
�������ƣ�GetVI2CPEC
�������ܣ���ȡ��ǰPEC
���������
���������
*****************************************************/
uint8_t GetI2CPEC(void)
{
  return I2CPEC;
}


/****************************************************
�������ƣ�VI2C_Set_ClkLow
�������ܣ���ʱ��������
���������
���������0:���쳣  1����ʱ
*****************************************************/
uint8_t VI2C_Set_ClkLow(void)
{
	uint32_t t=WAIT_TURN;

//	SCL_IN();
	do
	{
		if(READ_SCL)
		{
//			SCL_OUT();
			IIC_SCL = 0;
			return 0;
		}
		break;
	}while(t--);
	
//	SCL_OUT();
	
	return 1;
}
/****************************************************
�������ƣ�VI2C_Set_ClkHigh
�������ܣ���ʱ��������
���������
���������0:���쳣  1����ʱ
*****************************************************/
uint8_t VI2C_Set_ClkHigh(void)
{
	uint32_t t=WAIT_TURN;
	
//	SCL_OUT();
	IIC_SCL = 1;
	
//	SCL_IN();
	do
	{
		if(READ_SCL)  
		{
		 return 0;
		}
	}while(t--);
	
//	SCL_OUT();
	
	return 1;    
}
/****************************************************
�������ƣ�VI2C_Recover
�������ܣ��ָ���������
���������
���������
*****************************************************/
void VI2C_Recover(void)
{
  uint8_t i;
  for(i=0;i<10;i++)
  {
    VI2C_Set_ClkLow();
    VI2C_Set_ClkHigh();
  }
  for(i=0;i<8;i++)
  {
     VI2C_Stop();
  }
}
/****************************************************
�������ƣ�VI2C_Start
�������ܣ����������ź�
���������
���������0:���쳣  1����ʱ
*****************************************************/
uint8_t VI2C_Start(void)
{
//	SDA_OUT();
	
	IIC_SCL = 1;														
	IIC_SDA = 1;
	if(VI2C_Set_ClkLow())
		return 1;
	Wait(WAIT_KEEP);
	if(VI2C_Set_ClkHigh())//IIC_SCL = 1;    
		return 1;
	Wait(WAIT_KEEPHF);
	IIC_SDA = 0;                            
	Wait(WAIT_KEEPHF);
	VI2C_Set_ClkLow();                     
	Wait(WAIT_KEEP);
																				
	return 0;
}
/****************************************************
�������ƣ�VI2C_Stop
�������ܣ����ͽ�������
���������
���������
*****************************************************/
void VI2C_Stop(void)
{
//	SDA_OUT();

	IIC_SDA = 1;                      
	VI2C_Set_ClkHigh();
	
	Wait(WAIT_KEEPHF);
	VI2C_Set_ClkLow();                  
	Wait(WAIT_KEEPHF);
	IIC_SDA = 0;												
	Wait(WAIT_KEEPHF);
	VI2C_Set_ClkHigh();                  
	Wait(WAIT_KEEP);
	IIC_SDA = 1;                       
	Wait(WAIT_KEEP);   
}
/****************************************************
�������ƣ�VI2C_ReadACK
�������ܣ���ȡӦ���ź�
���������
���������0:Ӧ��  1����Ӧ��
*****************************************************/
uint8_t VI2C_ReadACK(void)
{
	uint8_t ret=1;
	
//	SDA_OUT();
	VI2C_Set_ClkLow();
	IIC_SDA = 1;
	Wait(WAIT_KEEP);
	VI2C_Set_ClkHigh();
	Wait(WAIT_KEEP);
	
//	SDA_IN();
	if(READ_SDA==0)	
		ret=0;
   
	return ret;
}
/****************************************************
�������ƣ�VI2C_WriteAdd
�������ܣ�д��ַ
���������w   1:д��ַ   0������ַ
���������0:Ӧ��  1����Ӧ��
*****************************************************/
uint8_t VI2C_WriteAdd(uint8_t w)
{
  uint8_t data,i;
	
//	SDA_OUT();
	if(w)
  {
    data=0x16;                     
    I2CPEC=Pectable[0^0x16];				
  }													 				
  else
  {
    data=0x17;										
    I2CPEC=Pectable[I2CPEC^0x17];	
  }
  
	for(i=0;i<8;i++)									
  {
    IIC_SCL = 0;										
    
		if(data&0x80)
			IIC_SDA = 1;
		else
			IIC_SDA = 0;
		
		Wait(WAIT_KEEP);
		VI2C_Set_ClkHigh();							
		Wait(WAIT_KEEPHF);
    data=data<<1;
  }
	
	return VI2C_ReadACK();						
}
/****************************************************
�������ƣ�VI2C_WriteData
�������ܣ�д����
���������data :д�������
���������0:Ӧ��  1����Ӧ��
*****************************************************/
uint8_t  VI2C_WriteData(uint8_t data)
{	
	uint8_t i;
	
//	SDA_OUT();
	I2CPEC=Pectable[I2CPEC^data];	
	for(i=0;i<8;i++)									
	{
		VI2C_Set_ClkLow();							
		if(data&0x80)
			IIC_SDA = 1;
		else
			IIC_SDA = 0;
		
		Wait(WAIT_KEEP);
		VI2C_Set_ClkHigh();							
		Wait(WAIT_KEEPHF);
		data=data<<1;
	}

	return VI2C_ReadACK();						
}
/****************************************************
�������ƣ�VI2C_ReadData
�������ܣ�������
���������A_N:  1��Ӧ����ֽ�  0����Ӧ����ֽ�(���豸Ӧ��ͬ)
�����������ȡ1byte
*****************************************************/
uint8_t VI2C_ReadData(uint8_t *data,uint8_t A_N)
{
	uint8_t ret, i;
	
	for(i=0;i<8;i++)							
	{
		ret=ret<<1;									
		if(VI2C_Set_ClkLow())				
			return 1;
//		SDA_OUT();
		IIC_SDA = 1;
		Wait(WAIT_KEEP);

		if(VI2C_Set_ClkHigh())			
			return 1;
		Wait(WAIT_KEEPHF);
		
//		SDA_IN();
		if(READ_SDA)								
			ret=ret|1;
		else
			ret=ret&0xfe;
	}
	
//	SDA_OUT();
	if(VI2C_Set_ClkLow())					
		return 1;
	if(A_N)												
		IIC_SDA = 0;
	else													
		IIC_SDA = 1;
	Wait(WAIT_KEEP);							
	if(VI2C_Set_ClkHigh())				
		return 1;
	Wait(WAIT_KEEP);
	*data=ret;										
	I2CPEC=Pectable[I2CPEC^ret];
	return 0;
}
/****************************************************
�������ƣ�VI2C_Writebytes
�������ܣ�д���ַ�����
���������data:����ָ��  len;д�����ݳ���  stopflag 0:��ֹͣλ   1����ֹͣλ
���������0�����쳣  ���� :ʧ��

*****************************************************/
uint8_t VI2C_Writebytes(uint8_t *data, uint8_t len,uint8_t stopflag)//
{
	unsigned char i,err=0;

	if(VI2C_Start())							
	{
		err=1;
		return err;
	}
	if(VI2C_WriteAdd(1))					
	{
		err|=2;
		return err;
	}
    
	for(i=0;i<len;i++)						
	{
		if(VI2C_WriteData(data[i]))	
		{
			err|=4;
			return err;
		}
	}

	if(stopflag)									
			VI2C_Stop();
	return err;
}
/****************************************************
�������ƣ�SMBUS_WriteBattery
�������ܣ�д���ַ�����
���������data:����ָ��  len;д�����ݳ��� 
���������0�����쳣  ���� :ʧ��
*****************************************************/
uint8_t SMBUS_WriteBattery(uint8_t *data, uint8_t len)
{
  uint8_t ret;
  ret=VI2C_Writebytes(data,  len,0);
  if(ret==0)
  {

    if(VI2C_WriteData(I2CPEC))//��PEC
      ret=4;
  }
   VI2C_Stop();
  return ret;
}
/****************************************************
�������ƣ�VI2C_Readbytes
�������ܣ�д���ַ�����
���������data:����ָ��  len;�����ݳ��� 
���������0�����쳣  ���� :ʧ��


*****************************************************/
uint8_t VI2C_Readbytes(uint8_t *data,uint8_t len)
{
  unsigned char i,err=0;

	if(VI2C_Start())											//��ʼ�ź�
	{
		err=1;
		return err;
	}
	if(VI2C_WriteAdd(0))									//�豸��ַ����
	{
		err|=2;
		return err;
	}
	for(i = 0; i < len; i++)						  //��ȡ3��
	{ 
		if(VI2C_ReadData(data+i,len-1-i))		//��ȡresult_value[0]��Ӧ2��result[1]��Ӧ1��result[2]��Ӧ0
		return 4;
	}
	VI2C_Stop();													//ֹͣ�ź�

	return err;
}
/****************************************************
�������ƣ�VI2C_Readcmd
�������ܣ�д���ַ�����
���������cmd������ data:����ָ��  len;�����ݳ��� 
���������0�����쳣  ���� :ʧ��
*****************************************************/
uint8_t VI2C_Readcmd(uint8_t cmd,uint8_t *data, uint8_t len)
{
    unsigned char i,err=0;
    if(VI2C_Start())
    {
      err=1;//ʱ�ӿ���ʧ��
      return err;
    }
    if(VI2C_WriteAdd(1))
    {
      err|=2;//�豸��Ӧ��
      return err;
    }
    VI2C_WriteData(cmd);
    VI2C_Start();
    VI2C_WriteAdd(0);
    for(i = 0; i < len; i++)
    {
      if(VI2C_ReadData(data+i,len-1-i))
          return 4;
    }
    
    VI2C_Stop();
    return err;
}
/****************************************************
��������:VI2C_ReadBattery
��������:��������ݣ����ж�У���Ƿ���ȷ
�������:cmd:����   buff:����ָ��  len�������ݳ���
�������:0:���쳣  ���� :ʧ��


*****************************************************/
uint8_t VI2C_ReadBattery(uint8_t cmd,uint8_t len,uint8_t *buff)
{
  uint8_t ret;
  uint8_t tempbuff[2];
  tempbuff[0]=cmd;									
  
	ret= VI2C_Writebytes(tempbuff,1,0);
  if(ret==0)
  {
    ret=VI2C_Readbytes(buff, len+1); 
    buff[len]=I2CPEC;
  }
  return ret;
}

void wait_keep_test(void)
{
	while (1)
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;	//modify by yanly
		Wait(WAIT_KEEP);
//		GPIO_SetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 1;
		Wait(WAIT_KEEP);
	}
}

void wait_keephf_test(void)
{
	while (1)
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;	//modify by yanly
		Wait(WAIT_KEEPHF);
//		GPIO_SetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 1;	
		Wait(WAIT_KEEPHF);
	}
}

void scl_set_low_test(void)
{
	uint32_t t=WAIT_TURN;//7000 20ms

	while (1)
	{
		t = WAIT_TURN;
		do
		{
			if(READ_SCL)
			{
			}
		}while(t--);
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1); //PC1, ALM LED ? note by yanly
		_ALARM_LED = 0;
		t = WAIT_TURN;
		do
		{
			if(READ_SCL)
			{
			}
		}while(t--);
//		GPIO_SetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 1;
	}
}

/***************************************************************/  //add by yanly from yuanzi iic
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
//	delay_us(12);
delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
// 	delay_us(12);
delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
//	delay_us(12);
delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
//	delay_us(2); //modify by yanly 170112
	IIC_SDA=1;delay_us(2);	   
//	delay_us(6);	   
	IIC_SCL=1;delay_us(2);	 
//	delay_us(6);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
delay_us(2);   //��TEA5767��������ʱ���Ǳ����
//		delay_us(12);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
//		delay_us(6); 
		IIC_SCL=0;	
		delay_us(2);
//		delay_us(12);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
delay_us(2);
//        delay_us(10);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
delay_us(1); 
//		delay_us(12); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
/***************************************************************/
