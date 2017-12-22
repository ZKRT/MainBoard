#include "flash.h"
#include "undercarriageCtrl.h"
#include "obstacleAvoid.h"
#include "dev_handle.h"

/*
�������洢���⣬����3����ģ��洢����֯������3������ϵͳ�洢����30K����OTP����528�֣���ѡ���ֽڣ�16�֣�
ϵͳ�洢����ų���ʱ��BOOTLOADER
OTP��һ����д������������32��1�飬һ����16�飬����512�֣��������һ����д�������Ҳ��ɲ���������16������������Ӧ�顣ע��OTP�Ĳ�������ͨFLASH�Ķ�д����һ��
�û�ѡ���ֽڣ��������ö�������BOR�������/Ӳ�����Ź���λ�Լ��������ڴ�����ֹͣ״̬�µĸ�λ

׼ȷ������FLASH�ĵȴ���������LATENCY������ȡ����CPU��SYSCLK���ѹ��Χ���������3.3V���粢����168MHZ������֪6��CPU���ڣ�LATENCY=5

FLASH��̲��裺���BSYλ�����æ�ȴ�����PG=1��������PSIZE����д�롪���ȴ�BSYλ���㡪���˳�

�����������裺���LOCK�����û���������������BSYλ�����æ�ȴ�����SER��һ����������������SNBѡ���ĸ���������STRT��һ����ʼ���������ȴ�BSYλ���㡪���˳�
�����������裺�������SER��һ��SNBѡ�������滻��MER��һ�����಻��

�ⲿVPP��ʲô��
���洢������Ϊʲô�ճ���ô��һƬ��ַ�ռ�δ֪��
����������������Ͳ���Ҫ�����β鿴�ˣ�ֱ��������һ��������
����������ʹ���ǲ���Ӧ���ڱ��֮ǰ��Ҫ�����ˣ�
IAP����ͨ�����ⷽʽ����Ӳ�������硢GPRSԶ�������ȣ�Ч�ʺܸߡ����硢GPRS�������Ҫ�ع鵽ĳ������Ľӿڣ�������ʵһ����Ч����
*/

//��0��T1
//��1��T2

//flash_type flash_buffer ={0XFEDCBA98, TEMPTURE_LOW_EXTRA, TEMPTURE_HIGH_EXTRA, 0X76543210};
flash_type flash_buffer;

//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}  

//������7����д��
void STMFLASH_Write(void)	
{
	uint8_t i = 0;
  FLASH_Status status = FLASH_COMPLETE;
	uint32_t WriteAddr = ADDR_FLASH_SECTOR_7;
	uint32_t *flash_addr = (uint32_t *)(&flash_buffer._start_cod);
	
	FLASH_Unlock();																				//���� 
  FLASH_DataCacheCmd(DISABLE);													//FLASH������д��ʱ���κγ��Զ�ȡ�Ĳ����ᵼ�����ߴ���������Ҫ��ȡ�����ݣ����Ļ��棬��д��Ĳ����������ֹ���ݻ��档����ûʲô�ã��ŵ����ﱸע��
	   
	status=FLASH_EraseSector(FLASH_Sector_7,VoltageRange_3);//VCC=2.7~3.6V֮��!!
	
	if(status==FLASH_COMPLETE)															//��������ɹ����ſ�ʼд��
	{
		for (i = 0; i < FLASH_USE_NUM; i++)
		{
			if(FLASH_ProgramWord(WriteAddr,  *flash_addr) != FLASH_COMPLETE)
			{
				break;	//д���쳣
			}
			WriteAddr += 4;
			flash_addr++;
		}
	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
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
//��flash���ֵ�洢���������������bsp����������������������ֵ
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
//����ĳ������
void STMFLASH_Erase(short sector)	
{
  FLASH_Status status = FLASH_COMPLETE;
	
	FLASH_Unlock();																				//���� 
  FLASH_DataCacheCmd(DISABLE);													//FLASH������д��ʱ���κγ��Զ�ȡ�Ĳ����ᵼ�����ߴ���������Ҫ��ȡ�����ݣ����Ļ��棬��д��Ĳ����������ֹ���ݻ��档����ûʲô�ã��ŵ����ﱸע��
	   
	status=FLASH_EraseSector(sector, VoltageRange_3);//VCC=2.7~3.6V֮��!!
	
	if(status==FLASH_COMPLETE)															//��������ɹ����ſ�ʼд��
	{
	}
	
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
}
/**
  * @brief  stmflash_process �����û����ô洢��
  * @param  None
  * @retval None
  */
void stmflash_process(void)	
{
	if ((flash_buffer._tempture_low != zkrt_devinfo.temperature_low)||(flash_buffer._tempture_high != zkrt_devinfo.temperature_high))//�����ｫ���е�ֵͳһ���£�����flashд��̫ռʱ����ܵ���д�����
	{
		flash_buffer._tempture_low  = zkrt_devinfo.temperature_low;				//�����������ñ�����������
		flash_buffer._tempture_high = zkrt_devinfo.temperature_high;
		STMFLASH_Write();											//����������дflash
	}
}
