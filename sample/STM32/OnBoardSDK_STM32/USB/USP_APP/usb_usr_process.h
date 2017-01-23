/**
  ******************************************************************************
  * @file    usb_process.h
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
**/	


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_USR_PROCESS__
#define __USB_USR_PROCESS__


#include "usbh_usr.h" 

extern USBH_HOST  USB_Host;
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;


void usb_user_prcs(void);
	
#endif //__USB_PROCESS__
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

