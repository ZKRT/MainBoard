#include "usb_usr_process.h"
#include "exfuns.h"
#include "sys.h"

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
u8 AppState; //Ӧ��״̬

//�û�����������
//����ֵ:0,����
//       1,������
u8 USH_User_App(void)
{ 
	u32 total,free;
	u8 res=0;
	
	switch(AppState)
	{
		case USH_USR_FS_INIT:
			printf("��ʼִ���û�����!!!\r\n");
		  AppState = USH_USR_FS_TEST;
			printf("�豸���ӳɹ�!.\n");	
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
//USB��ѭ��
void usb_user_prcs(void)
{        
	USBH_Process(&USB_OTG_Core, &USB_Host);//����ʵ��USB����ͨ�ŵĺ���״̬������Ҫ��ѭ�����ã�����Ƶ��Խ��Խ�ã��Ա㼰ʱ�����������\
	ע�⣺�˺�������ʶ��׶���Ҫ����Ƶ�����ã�ʶ��֮��ʣ�µĲ�����U�̶�д����������USB�жϴ���	
}















