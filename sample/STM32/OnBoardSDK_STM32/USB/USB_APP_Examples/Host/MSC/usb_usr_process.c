#include "usb_usr_process.h"
#include "exfuns.h"
#include "sys.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
u8 AppState; //应用状态

//用户测试主程序
//返回值:0,正常
//       1,有问题
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	
	switch(AppState)
	{
		case USH_USR_FS_INIT:
			printf("开始执行用户程序!!!\r\n");
		  AppState = USH_USR_FS_TEST;
			printf("设备连接成功!.\n");	
			res=exf_getfree((u8 *)"2:",&total,&free);
			if(res==0)
			{
				printf("FATFS OK!\n");	
				printf("U Disk Total Size:     %d MB\n", total>>10);	 
				printf("U Disk  Free Size:     %d MB\n", free>>10); 	    
			} 
			break;
		case USH_USR_FS_TEST:
			//do something //zkrt_todo
			break;
		default:break;
	}
	return res;
} 
//USB主循环
void usb_user_prcs(void)
{        
	USBH_Process(&USB_OTG_Core, &USB_Host);//用于实现USB主机通信的核心状态处理，需要被循环调用，而且频率越快越好，以便及时处理各种事务。\
	注意：此函数仅在识别阶段需要反复频繁调用，识别之后，剩下的操作（U盘读写）都可以由USB中断处理。	
}















