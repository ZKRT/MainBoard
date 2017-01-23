#ifndef __IIC_H
#define __IIC_H
#include "sys.h" 

extern uint8_t  I2CPEC;

/** @defmodify modify by yanly
  * @{
//IO方向设置
#define SCL_IN()  {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=0<<8*2;}	//PB8输入模式
#define SCL_OUT() {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=1<<8*2;} //PB8输出模式

#define SDA_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<9*2;}	//PB9输入模式
#define SDA_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<9*2;} //PB9输出模式

//IO操作函数
#define IIC_SCL    PBout(8) //SCL
#define READ_SCL	 PBin(8)	//SCL状态
#define IIC_SDA    PBout(9) //SDA	 
#define READ_SDA   PBin(9)  //输入SDA
  * @}
  */ 

//IO方向设置
#define SCL_IN()  {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=0<<8*2;}	//PA8输入模式
#define SCL_OUT() {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=1<<8*2;} //PA8输出模式
#define SDA_IN()  {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=0<<9*2;}	//PC9输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(9*2));GPIOC->MODER|=1<<9*2;} //PC9输出模式	

//IO操作函数	
#define IIC_SCL    PAout(8) //SCL
#define READ_SCL	 PAin(8)	//SCL状态
#define IIC_SDA    PCout(9) //SDA	 
#define READ_SDA   PCin(9)  //输入SDA

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 

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
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
/***************************************************************/

#endif

