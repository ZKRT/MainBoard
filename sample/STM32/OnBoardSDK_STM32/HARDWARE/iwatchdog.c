#include "iwatchdog.h"
#include "stm32f4xx_iwdg.h"

//初始化独立看门狗
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	
	IWDG_SetPrescaler(prer); 

	IWDG_SetReload(rlr);   

	IWDG_ReloadCounter(); 
	
	IWDG_Enable();      
}

//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
