#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   

//IO方向设置
#define DS18B20_IO_IN(num)  {GPIOA->MODER&=~(3<<(num*2));GPIOA->MODER|=0<<(num*2);}	//PA67输入模式 --已改为PA3和PA6
#define DS18B20_IO_OUT(num) {GPIOA->MODER&=~(3<<(num*2));GPIOA->MODER|=1<<(num*2);} //PA67输出模式 --已改为PA3和PA6
 
//IO操作函数											   
#define	DS18B20_DQ_OUT(num) PAout(num) //数据端口	PA67 --已改为PA3和PA6
#define	DS18B20_DQ_IN(num)  PAin(num)  //数据端口	PA67 --已改为PA3和PA6

//DS18B20 PIN NUMBER MAPPING
#define	DS18B20_NUM1				3  //PIN3
#define	DS18B20_NUM2        6  //PIN6
   	
uint8_t DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(uint8_t num);	//获取温度
void DS18B20_Start(uint8_t num);		//开始温度转换
void DS18B20_Write_Byte(uint8_t num, uint8_t data);//写入一个字节
uint8_t DS18B20_Read_Byte(uint8_t num);		//读出一个字节
uint8_t DS18B20_Read_Bit(uint8_t num);		//读出一个位
uint8_t DS18B20_Check(uint8_t num);			//检测是否存在DS18B20
void DS18B20_Rst(uint8_t num);			//复位DS18B20    
#endif















