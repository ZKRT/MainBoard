#ifndef __FLASH_H__
#define __FLASH_H__
#include "sys.h"   

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 								//STM32 FLASH的起始地址，扇区0的起始地址
#define STM32_FLASH_END  0x080FFFFF									//STM32 FLASH的结束地址，扇区11的结束地址


//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//扇区1起始地址, 16 Kbytes， 累计32K  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//扇区2起始地址, 16 Kbytes， 累计48K  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//扇区3起始地址, 16 Kbytes， 累计64K
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//扇区4起始地址, 64 Kbytes， 累计128K
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//扇区5起始地址, 128 Kbytes，累计256K
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//扇区6起始地址, 128 Kbytes，累计384K
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//扇区7起始地址, 128 Kbytes，累计512K。如果是E容量512K的，使用这个扇区作为基地址即可
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//扇区11起始地址,128 Kbytes  

//flash sector size
#define SIZE_FLASH_SECTOR_0     ((uint32_t)(ADDR_FLASH_SECTOR_1-ADDR_FLASH_SECTOR_0)
#define SIZE_FLASH_SECTOR_1     ((uint32_t)ADDR_FLASH_SECTOR_2-ADDR_FLASH_SECTOR_1)
#define SIZE_FLASH_SECTOR_2     ((uint32_t)ADDR_FLASH_SECTOR_3-ADDR_FLASH_SECTOR_2)
#define SIZE_FLASH_SECTOR_3     ((uint32_t)ADDR_FLASH_SECTOR_4-ADDR_FLASH_SECTOR_3)
#define SIZE_FLASH_SECTOR_4     ((uint32_t)ADDR_FLASH_SECTOR_5-ADDR_FLASH_SECTOR_4)
#define SIZE_FLASH_SECTOR_5     ((uint32_t)ADDR_FLASH_SECTOR_6-ADDR_FLASH_SECTOR_5)
#define SIZE_FLASH_SECTOR_6     ((uint32_t)ADDR_FLASH_SECTOR_7-ADDR_FLASH_SECTOR_6)
#define SIZE_FLASH_SECTOR_7     ((uint32_t)ADDR_FLASH_SECTOR_8-ADDR_FLASH_SECTOR_7)
#define SIZE_FLASH_SECTOR_8     ((uint32_t)ADDR_FLASH_SECTOR_9-ADDR_FLASH_SECTOR_8)
#define SIZE_FLASH_SECTOR_9     ((uint32_t)ADDR_FLASH_SECTOR_10-ADDR_FLASH_SECTOR_9)
#define SIZE_FLASH_SECTOR_10    ((uint32_t)ADDR_FLASH_SECTOR_11-ADDR_FLASH_SECTOR_10)
#define SIZE_FLASH_SECTOR_11    SIZE_FLASH_SECTOR_10

#pragma pack(push, 1)
typedef struct _flash_type	
{
	uint32_t _start_cod;
	short _tempture_low;
	short _tempture_high;
	uint32_t _end_cod;
}flash_type;
#pragma pack(pop)

extern flash_type flash_buffer;

#define FLASH_USE_NUM    (sizeof(flash_buffer)/4 + ((sizeof(flash_buffer)%4 == 0)?0:1))

uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//读出字  
void STMFLASH_Write(void);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(void);   		//从指定地址开始读出指定长度的数据
void STMFLASH_Init(void);
void STMFLASH_Erase(short sector);			
void stmflash_process(void);
#endif


