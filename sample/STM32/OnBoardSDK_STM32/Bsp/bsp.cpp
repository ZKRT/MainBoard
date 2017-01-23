/*! @file bsp.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  Helper functions for board STM32F4Discovery
 *
 *  Copyright 2016 DJI. All right reserved.
 *
 * */

#include "bsp.h"
#include "main.h"
#include "BspUsart.h"
#ifdef __cplusplus
extern "C"
{
#include "sys.h"
#include "adc.h"
#include "usart.h"
#include "timer_zkrt.h"
#include "led.h"
#include "iic.h"
#include "flash.h"
#include "can.h"
#include "ds18b20.h"
#include "osqtmr.h"
#include "ostmr.h"
#include "sram.h"
#include "malloc.h"
#ifdef USE_LWIP_FUN			
#include "lwip_comm.h"
#endif
#ifdef USE_USB_FUN	 	
#include "exfuns.h"
#include "usb_usr_process.h"
#endif
}
#endif //__cplusplus

void BSPinit()
{
//  RCC_Configuration();                                  //配置成内部时钟 
	SystickConfig();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	b_ostmr_init();                                         //fast timer
	b_systmr_init();                                        //quick fast timer
#ifdef USE_LWIP_FUN		
	lwip_timer_init();                                      //专为网络使用的定时器时钟
#endif
	led_init();
	FSMC_SRAM_Init();
	mymem_init(SRAMIN);		                                  //初始化内部内存池
	mymem_init(SRAMEX);		                                  //初始化外部内存池
	IIC_Init();
	STMFLASH_Init();
#ifdef USE_DJI_FUN	
	Usart_DJI_Config();
#endif	
	uart_init();
	can_all_init();
#ifdef _TEMPTURE_IO_
	DS18B20_Init();																		
#endif	
#ifdef _TEMPTURE_ADC_	
	ADC1_Init();
#endif
#ifdef USE_LWIP_FUN	
  lwip_comm_init();
#endif
#ifdef USE_USB_FUN
 	exfuns_init();			                                   //为fatfs相关变量申请内存 
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  //初始化USB主机
#endif
}
