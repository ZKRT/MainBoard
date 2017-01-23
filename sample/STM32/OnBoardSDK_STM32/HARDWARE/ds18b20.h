#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   

//IO��������
#define DS18B20_IO_IN(num)  {GPIOA->MODER&=~(3<<(num*2));GPIOA->MODER|=0<<(num*2);}	//PA67����ģʽ --�Ѹ�ΪPA3��PA6
#define DS18B20_IO_OUT(num) {GPIOA->MODER&=~(3<<(num*2));GPIOA->MODER|=1<<(num*2);} //PA67���ģʽ --�Ѹ�ΪPA3��PA6
 
//IO��������											   
#define	DS18B20_DQ_OUT(num) PAout(num) //���ݶ˿�	PA67 --�Ѹ�ΪPA3��PA6
#define	DS18B20_DQ_IN(num)  PAin(num)  //���ݶ˿�	PA67 --�Ѹ�ΪPA3��PA6

//DS18B20 PIN NUMBER MAPPING
#define	DS18B20_NUM1				3  //PIN3
#define	DS18B20_NUM2        6  //PIN6
   	
uint8_t DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(uint8_t num);	//��ȡ�¶�
void DS18B20_Start(uint8_t num);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t num, uint8_t data);//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(uint8_t num);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(uint8_t num);		//����һ��λ
uint8_t DS18B20_Check(uint8_t num);			//����Ƿ����DS18B20
void DS18B20_Rst(uint8_t num);			//��λDS18B20    
#endif















