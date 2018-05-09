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
#include "hw_config.h"
#include "bsp.h"
#include "main.h"
#include "BspUsart.h"

#ifdef CPLUSPLUS_HANDLE_ZK
extern "C" {
#endif

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
#include "osusart.h"
#ifdef USE_LWIP_FUN
#include "lwip_comm.h"
#endif
#ifdef USE_USB_FUN
#include "exfuns.h"
#include "usb_usr_process.h"
#endif

#ifdef CPLUSPLUS_HANDLE_ZK
}
#endif

void BSPinit() {
    SystickConfig();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    b_ostmr_init();                                         //fast timer
    b_systmr_init();                                        //quick fast timer
#ifdef USE_LWIP_FUN
    lwip_timer_init();                                      //专为网络使用的定时器时钟
#endif
    led_init();
#ifdef USE_EXTRAM_FUN
    FSMC_SRAM_Init();
    mymem_init(SRAMIN);                                       //初始化内部内存池
    mymem_init(SRAMEX);                                       //初始化外部内存池
#endif
    IIC_Init();
    STMFLASH_Init();
#ifdef USE_DJI_FUN
    Usart_DJI_Config();
#endif
    os_usart_init();  //usart os task init
    usart_config();
    can_all_init();
#ifdef _TEMPTURE_IO_
    DS18B20_Init();
#endif
#ifdef _TEMPTURE_ADC_
//  ADC1_Init();     //zkrt_notice : HWV4.0没有ADC检测，此处暂时直接屏蔽，不用关宏定义
#endif
#ifdef USE_LWIP_FUN
    lwip_comm_init();
#endif
#ifdef USE_USB_FUN
    exfuns_init();                                             //为fatfs相关变量申请内存
    USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks); //初始化USB主机
#endif
}
