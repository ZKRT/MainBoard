#include "flash.h"
#include "undercarriageCtrl.h"
#include "obstacleAvoid.h"
#include "dev_handle.h"

/*
除了主存储器外，还有3闪存模块存储器组织还包括3个区域：系统存储器（30K）、OTP区域（528字）、选项字节（16字）
系统存储器存放出厂时的BOOTLOADER
OTP是一次性写入区域，它包括32字1块，一共有16块，共计512字，这里可以一次性写入数据且不可擦除；还有16字用于锁定对应块。注意OTP的操作和普通FLASH的读写操作一样
用户选项字节：用于配置读保护、BOR级别、软件/硬件看门狗复位以及器件处于待机、停止状态下的复位

准确的配置FLASH的等待周期数（LATENCY），它取决于CPU的SYSCLK与电压范围，这里采用3.3V供电并且是168MHZ，查表可知6个CPU周期，LATENCY=5

FLASH编程步骤：检查BSY位，如果忙等待——PG=1——配置PSIZE——写入——等待BSY位清零——退出

扇区擦除步骤：检查LOCK，如果没解锁解锁——检查BSY位，如果忙等待——SER置一，擦扇区操作——SNB选择哪个扇区——STRT置一，开始擦除——等待BSY位置零——退出
批量擦除步骤：把上面的SER置一和SNB选扇区，替换成MER置一，其余不变

外部VPP是什么？
主存储器后面为什么空出那么大一片地址空间未知？
擦除操作后该扇区就不需要再依次查看了，直接跳到下一扇区即可
数据流缓存使能是不是应该在编程之前就要开启了？
IAP可以通过任意方式，如硬件、网络、GPRS远程升级等，效率很高。网络、GPRS等最后还是要回归到某个具体的接口，所以其实一样的效果。
*/

//字0：T1
//字1：T2

//flash_type flash_buffer ={0XFEDCBA98, TEMPTURE_LOW_EXTRA, TEMPTURE_HIGH_EXTRA, 0X76543210};
flash_type flash_buffer;

//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}  

//对扇区7进行写入
void STMFLASH_Write(void)	
{
	uint8_t i = 0;
  FLASH_Status status = FLASH_COMPLETE;
	uint32_t WriteAddr = ADDR_FLASH_SECTOR_7;
	uint32_t *flash_addr = (uint32_t *)(&flash_buffer._start_cod);
	
	FLASH_Unlock();																				//解锁 
  FLASH_DataCacheCmd(DISABLE);													//FLASH擦除或写入时，任何尝试读取的操作会导致总线错误。所以想要读取出数据，更改缓存，再写入的操作，必须禁止数据缓存。本来没什么用，放到这里备注！
	   
	status=FLASH_EraseSector(FLASH_Sector_7,VoltageRange_3);//VCC=2.7~3.6V之间!!
	
	if(status==FLASH_COMPLETE)															//如果擦除成功，才开始写！
	{
		for (i = 0; i < FLASH_USE_NUM; i++)
		{
			if(FLASH_ProgramWord(WriteAddr,  *flash_addr) != FLASH_COMPLETE)
			{
				break;	//写入异常
			}
			WriteAddr += 4;
			flash_addr++;
		}
	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(void)   	
{
	uint32_t i;
	uint32_t ReadAddr = ADDR_FLASH_SECTOR_7;
	uint32_t *flash_addr = (uint32_t *)(&flash_buffer._start_cod);
	
	for(i = 0;i < FLASH_USE_NUM; i++)
	{
		*flash_addr = STMFLASH_ReadWord(ReadAddr);
		ReadAddr += 4;
		flash_addr++;
	}
}
//将flash里的值存储到缓冲数组里，再在bsp里用这个数组给各个变量赋值
void STMFLASH_Init(void)
{
	STMFLASH_Read();
	
	if((flash_buffer._start_cod == 0XFEDCBA98)&&(flash_buffer._end_cod == 0X76543210))
	{
	}
	else
	{
		flash_buffer._start_cod = 0XFEDCBA98;
		flash_buffer._end_cod = 0X76543210;
		//tempture data
		flash_buffer._tempture_low  = GLO_TEMPTURE_LOW_INIT;
		flash_buffer._tempture_high = GLO_TEMPTURE_HIGH_INIT;
		//obstacle data
		flash_buffer.avoid_ob_enabled = OB_ENABLED_INIT;
		flash_buffer.avoid_ob_distse = OBSTACLE_ALARM_DISTANCE;
		flash_buffer.avoid_ob_velocity = OBSTACLE_AVOID_VEL_10TIMES;
		//undercarriage data
		flash_buffer.uce_autoenabled = 0;
		
		STMFLASH_Write();
	}
	
	////read flash value to global param struct
	//temp
	zkrt_devinfo.temperature_low  =  flash_buffer._tempture_low;
	zkrt_devinfo.temperature_high =  flash_buffer._tempture_high;
	//obstacle
	GuidanceObstacleData.ob_enabled = flash_buffer.avoid_ob_enabled;
	GuidanceObstacleData.ob_distance = flash_buffer.avoid_ob_distse;
	GuidanceObstacleData.ob_velocity = flash_buffer.avoid_ob_velocity;
	//undercarriage
	undercarriage_data.uce_autoenabled = flash_buffer.uce_autoenabled;
}
//擦除某个扇区
void STMFLASH_Erase(short sector)	
{
  FLASH_Status status = FLASH_COMPLETE;
	
	FLASH_Unlock();																				//解锁 
  FLASH_DataCacheCmd(DISABLE);													//FLASH擦除或写入时，任何尝试读取的操作会导致总线错误。所以想要读取出数据，更改缓存，再写入的操作，必须禁止数据缓存。本来没什么用，放到这里备注！
	   
	status=FLASH_EraseSector(sector, VoltageRange_3);//VCC=2.7~3.6V之间!!
	
	if(status==FLASH_COMPLETE)															//如果擦除成功，才开始写！
	{
	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}
/**
  * @brief  stmflash_process 管理用户配置存储区
  * @param  None
  * @retval None
  */
void stmflash_process(void)	
{
	if ((flash_buffer._tempture_low != zkrt_devinfo.temperature_low)||(flash_buffer._tempture_high != zkrt_devinfo.temperature_high))//在这里将所有的值统一更新，否则flash写入太占时间可能导致写入错误
	{
		flash_buffer._tempture_low  = zkrt_devinfo.temperature_low;				//步骤二：保存该变量到缓存里
		flash_buffer._tempture_high = zkrt_devinfo.temperature_high;
		STMFLASH_Write();											//步骤三：编写flash
	}
}
