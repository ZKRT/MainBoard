#include "bat.h"
#include "iic.h"												
#include "mavlink_msg_smartbat_text.h"	
#include "heart.h"
#include "djiapp.h"

uint16_t Betterydata[14]={0};
uint16_t dji_Betterydata[16]={0};
uint32_t Device_ID=0;				 
uint8_t result_value[3]={0,0,3};



void sendToMobile(uint8_t *data, uint8_t len);


//读温度
uint16_t readtemperature(void)
{
	uint16_t temperature=0;
		if(VI2C_ReadBattery(0x08,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				temperature=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				temperature=0xFFFF;
			}
		}
		else 
		{
			return 8;
		}

		return  temperature;
}

//读总电压
uint16_t readvoltage(void)
{
	uint16_t voltage=0;
		if(VI2C_ReadBattery(0x09,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				voltage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				voltage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 1;
		}
		return  voltage;
}

//读充放电电流
uint16_t readcurrent(void)
{
	uint16_t current=0;
		if(VI2C_ReadBattery(0x0a,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				current=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				current=0xFFFF;//±íê??áμ?μ?êy?Yê?′í?óμ?
			}
		}
		else 
		{
			return 2;
		}
		
		if(current>45000)  //当电池是放电电流时，负数，取反+1
		{
			current=0xffff-current+1;
		}

		return  current;
}

//读剩余容量百分比
uint16_t readpercent(void)
{
	uint16_t percent=0;
		if(VI2C_ReadBattery(0x0d,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				percent=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				percent=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 4;
		}
		return  percent;
}


//读剩余容量
uint16_t readrelativeelectric(void)
{
	uint16_t relativeelectric=0;
		if(VI2C_ReadBattery(0x0f,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				relativeelectric=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				relativeelectric=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 3;
		}
		return  relativeelectric;
}

//读满充容量
uint16_t readelectric(void)
{
	uint16_t electric=0;
		if(VI2C_ReadBattery(0x10,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				electric=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				electric=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 6;
		}
		return  electric;
}


//读循环次数
uint16_t readcyclecount(void)
{
	uint16_t cyclecount=0;
		if(VI2C_ReadBattery(0x17,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				cyclecount=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				cyclecount=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 5;
		}
		return  cyclecount;
}

//读设计容量
uint16_t readdesigncapacity(void)
{
	uint16_t readdesigncapacity=0;
		if(VI2C_ReadBattery(0x18,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				readdesigncapacity=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				readdesigncapacity=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 7;
		}
		return  readdesigncapacity;
}

//读S1电压
uint16_t reads1votlage(void)
{
	uint16_t s1votlage=0;
		if(VI2C_ReadBattery(0x3c,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s1votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s1votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 9;
		}
		return  s1votlage;
	}

	//读S2电压
uint16_t reads2votlage(void)
{
	uint16_t s2votlage=0;
		if(VI2C_ReadBattery(0x3d,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s2votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s2votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 10;
		}
		return  s2votlage;
	}

//读S3电压
uint16_t reads3votlage(void)
{
	uint16_t s3votlage=0;
		if(VI2C_ReadBattery(0x3e,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s3votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s3votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 11;
		}
		return  s3votlage;
	}

	//读S4电压
uint16_t reads4votlage(void)
{
	uint16_t s4votlage=0;
		if(VI2C_ReadBattery(0x3f,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s4votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s4votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 12;
		}
		return  s4votlage;
	}

	//读S5电压
uint16_t reads5votlage(void)
{
	uint16_t s5votlage=0;
		if(VI2C_ReadBattery(0x40,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s5votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s5votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 13;
		}
		return  s5votlage;
	}

	
	//读S6电压
uint16_t reads6votlage(void)
{
	uint16_t s6votlage=0;
		if(VI2C_ReadBattery(0x41,2,result_value)==0)
		{
			if(result_value[2]==0)
			{
				s6votlage=result_value[1]<<8 | result_value[0];
			}
			else
			{
				I2CPEC=0;
				s6votlage=0xFFFF;//表示读到的数据是错误的
			}
		}
		else 
		{
			return 14;
		}
		return  s6votlage;
}

void bat_value_roll(void)
{
	Betterydata[0]=readvoltage();		 			//0X09，第1个——1
	Betterydata[1]=readcurrent();		 			//0X0A，第2个——2
	Betterydata[2]=readrelativeelectric();//0X0F，第3个——4
	Betterydata[3]=readpercent();					//0X0D，第4个——3
	Betterydata[4]=readcyclecount();			//0X17，第5个——6
	Betterydata[5]=readelectric();				//0X10，第6个——5
	Betterydata[6]=readdesigncapacity();	//0X18，第7个——7
	Betterydata[7]=readtemperature();			//0X08，第8个——0
	Betterydata[8]=reads1votlage();				//V1，第9个——8，以下以此类推
	Betterydata[9]=reads2votlage();				//V2
	Betterydata[10]=reads3votlage();			//V3
	Betterydata[11]=reads4votlage();			//V4
	Betterydata[12]=reads5votlage();			//V5
	Betterydata[13]=reads6votlage();			//V6
	
	if ((Betterydata[0] != 1)||(Betterydata[1] != 2)||(Betterydata[2] != 3)||(Betterydata[3] != 4)||(Betterydata[4] != 5)||(Betterydata[5] != 6)||(Betterydata[6] != 7)||
		(Betterydata[7] != 8)||(Betterydata[8] != 9)||(Betterydata[9] != 10)||(Betterydata[10] != 11)||(Betterydata[11] != 12)||(Betterydata[12] != 13)||(Betterydata[13] != 14))//更新心跳
	{
		msg_smartbat_buffer[24] |= 0X04;	

	}
	else
	{
		msg_smartbat_buffer[24] &= 0XFB;	 
	
	}
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

void dji_bat_value_roll(void)
{

	dji_Betterydata[0]=readvoltage();		 			//0X09，第1个——1
	dji_Betterydata[1]=readcurrent();		 			//0X0A，第2个——2
	dji_Betterydata[2]=readrelativeelectric();//0X0F，第3个——4
	dji_Betterydata[3]=readpercent();					//0X0D，第4个——3
	dji_Betterydata[4]=readcyclecount();			//0X17，第5个——6
	dji_Betterydata[5]=readelectric();				//0X10，第6个——5
	dji_Betterydata[6]=readdesigncapacity();	//0X18，第7个——7
	dji_Betterydata[7]=readtemperature();			//0X08，第8个——0
	dji_Betterydata[8]=reads1votlage();				//V1，第9个——8，以下以此类推
	dji_Betterydata[9]=reads2votlage();				//V2
	dji_Betterydata[10]=reads3votlage();			//V3
	dji_Betterydata[11]=reads4votlage();			//V4
	dji_Betterydata[12]=reads5votlage();			//V5
	dji_Betterydata[13]=reads6votlage();			//V6
//	dji_Betterydata[14]=Device_ID;
//	dji_Betterydata[15]=Device_ID>>0xf;
	
	if ((dji_Betterydata[0] != 1)||(dji_Betterydata[1] != 2)||(dji_Betterydata[2] != 3)||(dji_Betterydata[3] != 4)||(dji_Betterydata[4] != 5)||(dji_Betterydata[5] != 6)||(dji_Betterydata[6] != 7)||
		(dji_Betterydata[7] != 8)||(dji_Betterydata[8] != 9)||(dji_Betterydata[9] != 10)||(dji_Betterydata[10] != 11)||(dji_Betterydata[11] != 12)||(dji_Betterydata[12] != 13)||(dji_Betterydata[13] != 14))//更新心跳
	{
		msg_smartbat_dji_buffer[24] |= 0X04;
	}
	else
	{
		msg_smartbat_dji_buffer[24] &= 0XFB;
				memset(dji_Betterydata, 0, 14);
		dji_Betterydata[3] = 100;
		dji_Betterydata[7] = 2731;
	}
	
}

/***********************************************************************
		          中科瑞泰消防无人机函数定义
		
		函数 名称：  
	       函数 功能：
		函数 入参：
		函数返回值：
		说     明：

************************************************************************/

//该函数一共32个字节长度，包括1个word类型的ID和14个半word类型的数据
void dji_bat_value_send(void)
{

	sendToMobile((uint8_t*)&dji_Betterydata,32);

}






//该函数一共32个字节长度，包括1个word类型的ID和14个半word类型的数据
void bat_value_send(void)
{
	mavlink_msg_smartbat_text_send(MAVLINK_COMM_0, Device_ID, Betterydata[0], Betterydata[1], Betterydata[2], Betterydata[3], Betterydata[4], Betterydata[5], Betterydata[6], Betterydata[7], Betterydata[8], Betterydata[9], Betterydata[10], Betterydata[11], Betterydata[12], Betterydata[13]);
}
