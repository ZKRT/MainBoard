#include "iwatchdog.h"
#include "stm32f4xx_iwdg.h"

//��ʼ���������Ź�
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	
	IWDG_SetPrescaler(prer); 

	IWDG_SetReload(rlr);   

	IWDG_ReloadCounter(); 
	
	IWDG_Enable();      
}

//ι�������Ź�
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
