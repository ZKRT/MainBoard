#ifndef __IIC_H
#define __IIC_H
#include "sys.h" 

extern uint8_t  I2CPEC;

/** @defmodify modify by yanly
  * @{
//IO��������
#define SCL_IN()  {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=0<<8*2;}	//PB8����ģʽ
#define SCL_OUT() {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=1<<8*2;} //PB8���ģʽ

#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9���ģʽ

//IO��������
#define IIC_SCL    PBout(8) //SCL
#define READ_SCL	 PBin(8)	//SCL״̬
#define IIC_SDA    PBout(9) //SDA	 
#define READ_SDA   PBin(9)  //����SDA
  * @}
  */ 

//IO��������
#define SCL_IN()  {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=0<<8*2;}	//PA8����ģʽ
#define SCL_OUT() {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=1<<8*2;} //PA8���ģʽ
#define SDA_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PC9����ģʽ
#define SDA_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} //PC9���ģʽ	

//IO��������	
#define IIC_SCL    PAout(8) //SCL
#define READ_SCL	 PAin(8)	//SCL״̬
#define IIC_SDA    PCout(9) //SDA	 
#define READ_SDA   PCin(9)  //����SDA

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 

uint8_t VI2C_Set_ClkLow(void);
uint8_t VI2C_Set_ClkHigh(void);

uint8_t VI2C_Start(void);
uint8_t VI2C_WriteAdd(uint8_t w);
uint8_t VI2C_ReadACK(void);
uint8_t  VI2C_WriteData(uint8_t data);
uint8_t VI2C_ReadData(uint8_t *data,uint8_t A_N);
uint8_t VI2C_Writebytes(uint8_t *data, uint8_t len,uint8_t stopflag);
uint8_t VI2C_Readcmd(uint8_t cmd,uint8_t *data, uint8_t len);
uint8_t VI2C_Readbytes(uint8_t *data,uint8_t len);
void VI2C_Stop(void);
uint8_t GetVI2CPEC(void);
void Wait(uint16_t t);
void VI2C_Recover(void);

uint8_t VI2C_ReadBattery(uint8_t cmd,uint8_t len,uint8_t *buff);
uint8_t SMBUS_WriteBattery(uint8_t *data, uint8_t len);

void wait_keep_test(void);
void wait_keephf_test(void);
void scl_set_low_test(void);


/***************************************************************/  //add by yanly from yuanzi iic 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
/***************************************************************/

#endif

