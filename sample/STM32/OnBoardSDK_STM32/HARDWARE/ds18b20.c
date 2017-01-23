/**
  ******************************************************************************
  * @file    ds18b20.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   IIC
  *           + .. 
  *           + [2] 旧的IO口PA7为PA3  --161213 by yanly
	*																														
  *         
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************  
  */ 

#include "ds18b20.h"
#include "led.h"

//复位DS18B20
void DS18B20_Rst(uint8_t num)	   
{
	DS18B20_IO_OUT(num); 
	
  DS18B20_DQ_OUT(num)=0;
  delay_us(750); 
	
  DS18B20_DQ_OUT(num)=1;
	delay_us(15);
}


//等待DS18B20的回应
uint8_t DS18B20_Check(uint8_t num) 	   
{   
	uint8_t retry=0;
	
	DS18B20_IO_IN(num);
    while (DS18B20_DQ_IN(num)&&retry<45)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=45)return 1;
	else retry=0;
	
	//P4:等待阶段2	
	delay_us(60);
	while (!DS18B20_DQ_IN(num)&&retry<180)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=180)return 1;
	
	delay_us(480);
	
	return 0;
}
//从DS18B20读取一个位
uint8_t DS18B20_Read_Bit(uint8_t num)
{
  uint8_t bit;
	
	DS18B20_IO_OUT(num);
  DS18B20_DQ_OUT(num)=0;
	delay_us(2);
  DS18B20_DQ_OUT(num)=1; 
	DS18B20_IO_IN(num);
	delay_us(12);
	if(DS18B20_DQ_IN(num))bit=1;
  else bit=0;
  delay_us(50);
  
	return bit;
}
//从DS18B20读取一个字节
uint8_t DS18B20_Read_Byte(uint8_t num)    
{        
	uint8_t i,j,data;
	data=0;
	for (i=0;i<8;i++) 
	{
		j=DS18B20_Read_Bit(num);
		data=(j<<7)|(data>>1);
  }
	return data;
}


//写一个字节到DS18B20
void DS18B20_Write_Byte(uint8_t num, uint8_t data)     
{             
	uint8_t j;
	uint8_t testb;
 
	DS18B20_IO_OUT(num);
	for (j=0;j<8;j++) 
	{
		testb=data&0x01;
		data=data>>1;
		if (testb) 
		{
			DS18B20_DQ_OUT(num)=0;
			delay_us(2);                            
			DS18B20_DQ_OUT(num)=1;
			delay_us(60);             
		}
		else 
		{
			DS18B20_DQ_OUT(num)=0;
			delay_us(60);             
			DS18B20_DQ_OUT(num)=1;
			delay_us(2);                          
		}
	}
}
 

//开始温度转换
void DS18B20_Start(uint8_t num)// ds1820 start convert
{   						               
    DS18B20_Rst(num);	   
	  DS18B20_Check(num);	 
    DS18B20_Write_Byte(num, 0xcc);// skip rom
    DS18B20_Write_Byte(num, 0x44);// convert
} 


//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在
uint8_t DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

  //GPIOG9
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //modify by yanly for(2)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
// 	DS18B20_Rst(6); //modify by yanly
	DS18B20_Rst(DS18B20_NUM1);
	if (DS18B20_Check(DS18B20_NUM1) == 1)	//if (DS18B20_Check(6) == 1)				
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;
		return 1;		
	}
	delay_ms(10);								
	
	DS18B20_Rst(DS18B20_NUM2);   //zkrt_notice: 如果第一个温度传感器没有接或者初始化失败，就不会初始化第二个温度传感器。会不会有问题？//by yanly
	if (DS18B20_Check(DS18B20_NUM2) == 1)
	{
//		GPIO_ResetBits(GPIOC, GPIO_Pin_1);
		_ALARM_LED = 0;
		return 1;
	}
	delay_ms(10);
	
//	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	_ALARM_LED = 1;
	
	return 0;
}


//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(uint8_t num)
{
	uint8_t temp;
	uint8_t TL,TH;
	short value;
	
	DS18B20_Rst(num);
	if (DS18B20_Check(num) == 1)
	{
		return 0XFFF;
	}
	DS18B20_Write_Byte(num, 0xcc);// skip rom
	DS18B20_Write_Byte(num, 0x44);// convert
	
	DS18B20_Rst(num);
	if (DS18B20_Check(num) == 1)
	{
		return 0XFFF;
	}
	DS18B20_Write_Byte(num, 0xcc);// skip rom
	DS18B20_Write_Byte(num, 0xbe);// convert
	
	TL=DS18B20_Read_Byte(num); // LSB   
	TH=DS18B20_Read_Byte(num); // MSB   
	if(TH>7)
	{
		TH=~TH;
		TL=~TL; 
		temp=0;
	}else temp=1;
	
	value=TH; 
	value<<=8;    
	value+=TL;
	value=(double)value*0.625;
		
	if(temp)return value; 
	else return -value;    
}
















