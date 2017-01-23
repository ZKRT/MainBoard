#ifndef __FLASH_H__
#define __FLASH_H__
#include "sys.h"   

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 								//STM32 FLASH����ʼ��ַ������0����ʼ��ַ
#define STM32_FLASH_END  0x080FFFFF									//STM32 FLASH�Ľ�����ַ������11�Ľ�����ַ


//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes�� �ۼ�32K  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes�� �ۼ�48K  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes�� �ۼ�64K
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes�� �ۼ�128K
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes���ۼ�256K
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes���ۼ�384K
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes���ۼ�512K�������E����512K�ģ�ʹ�����������Ϊ����ַ����
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  

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

uint32_t STMFLASH_ReadWord(uint32_t faddr);		  	//������  
void STMFLASH_Write(void);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(void);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void STMFLASH_Init(void);
void STMFLASH_Erase(short sector);			
void stmflash_process(void);
#endif


