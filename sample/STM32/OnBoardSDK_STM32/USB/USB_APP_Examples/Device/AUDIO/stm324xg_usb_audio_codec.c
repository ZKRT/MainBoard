#include "stm324xg_usb_audio_codec.h"
#include "usbd_audio_core.h"
#include "wm8978.h"
#include "i2s.h"
#include "malloc.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//USB�����ײ�ӿں��� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
u8 volume=0;								//��ǰ����  

vu8 audiostatus=0;							//bit0:0,��ͣ����;1,��������   
vu8 i2splaybuf=0;							//�������ŵ���Ƶ֡������
vu8 i2ssavebuf=0;							//��ǰ���浽����Ƶ������ 
#define AUDIO_BUF_NUM		100				//���ڲ��õ���USB�첽ʱ�Ӳ���
											//STM32 IIS���ٶȺ�USB���͹������ݵ��ٶȴ��ڲ���,������48Khz��,ʵ
											//��IIS�ǵ���48Khz(47.991Khz)��,���Ե����͹�����������,���STM32����
											//�ٶȿ�,������дλ��׷�ϲ���λ��(i2ssavebuf==i2splaybuf)ʱ,�ͻ����
											//���.���þ������AUDIO_BUF_NUMֵ,���Ծ������ٻ������. 
								
u8 *i2sbuf[AUDIO_BUF_NUM]; 					//��Ƶ����֡,ռ���ڴ���=AUDIO_BUF_NUM*AUDIO_OUT_PACKET �ֽ�
  
//��Ƶ����I2S DMA����ص�����
void audio_i2s_dma_callback(void) 
{      
	if((i2splaybuf==i2ssavebuf)&&audiostatus==0)
	{ 
		I2S_Play_Stop();
	}else
	{
		i2splaybuf++;
		if(i2splaybuf>(AUDIO_BUF_NUM-1))i2splaybuf=0;
		if(DMA1_Stream4->CR&(1<<19))
		{	 
			DMA_MemoryTargetConfig(DMA1_Stream4,(u32)i2sbuf[i2splaybuf],DMA_Memory_0);
			//DMA1_Stream4->M0AR=(u32)i2sbuf[i2splaybuf];//ָ����һ��buf 
		}
		else 
		{   
			DMA_MemoryTargetConfig(DMA1_Stream4,(u32)i2sbuf[i2splaybuf],DMA_Memory_1);
			//DMA1_Stream4->M1AR=(u32)i2sbuf[i2splaybuf];//ָ����һ��buf 
		} 
	}
}  

//������Ƶ�ӿ�
//OutputDevice:����豸ѡ��,δ�õ�.
//Volume:������С,0~100
//AudioFreq:��Ƶ������
uint32_t EVAL_AUDIO_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{   
	u16 t=0;
	for(t=0;t<AUDIO_BUF_NUM;t++)		//�ڴ����� 
	{
		i2sbuf[t]=mymalloc(SRAMIN,AUDIO_OUT_PACKET);
	}
	if(i2sbuf[AUDIO_BUF_NUM-1]==NULL)	//�ڴ�����ʧ��
	{
		printf("Malloc Error!\r\n");
		for(t=0;t<AUDIO_BUF_NUM;t++)myfree(SRAMIN,i2sbuf[t]); 
		return 1;
	} 
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);					//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����
	I2S2_SampleRate_Set(AudioFreq);		//���ò�����
	EVAL_AUDIO_VolumeCtl(Volume);		//��������
	I2S2_TX_DMA_Init(i2sbuf[0],i2sbuf[1],AUDIO_OUT_PACKET/2); 
 	i2s_tx_callback=audio_i2s_dma_callback;		//�ص�����ָwav_i2s_dma_callback
	I2S_Play_Start();							//����DMA  
	printf("EVAL_AUDIO_Init:%d,%d\r\n",Volume,AudioFreq);
	return 0; 
}
 
//��ʼ������Ƶ����
//pBuffer:��Ƶ�������׵�ַָ��
//Size:��������С(��λ:�ֽ�)
uint32_t EVAL_AUDIO_Play(uint16_t* pBuffer, uint32_t Size)
{  
	printf("EVAL_AUDIO_Play:%x,%d\r\n",pBuffer,Size);
	return 0;
}
 
//��ͣ/�ָ���Ƶ������
//Cmd:0,��ͣ����;1,�ָ�����
//Addr:��Ƶ�����������׵�ַ
//Size:��Ƶ��������С(��λ:harf word,Ҳ����2���ֽ� 
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint32_t EVAL_AUDIO_PauseResume(uint32_t Cmd, uint32_t Addr, uint32_t Size)
{    
	u16 i;
	u8 *p=(u8*)Addr;
	if(Cmd==AUDIO_PAUSE)
	{
 		audiostatus=0; 
	}else
	{
		audiostatus=1;
		i2ssavebuf++;
		if(i2ssavebuf>(AUDIO_BUF_NUM-1))i2ssavebuf=0;
		for(i=0;i<AUDIO_OUT_PACKET;i++)
		{
			i2sbuf[i2ssavebuf][i]=p[i];
		}
		I2S_Play_Start();				//����DMA  
	} 
	return 0;
}
 
//ֹͣ����
//Option:���Ʋ���,1/2,���:CODEC_PDWN_HW����
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint32_t EVAL_AUDIO_Stop(uint32_t Option)
{ 
	printf("EVAL_AUDIO_Stop:%d\r\n",Option);
	audiostatus=0;
	return 0;
} 
//�������� 
//Volume:0~100
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint32_t EVAL_AUDIO_VolumeCtl(uint8_t Volume)
{ 
	volume=Volume; 
	WM8978_HPvol_Set(volume*0.63,volume*0.63);
	WM8978_SPKvol_Set(volume*0.63);
	return 0;
} 
//��������
//Cmd:0,����
//    1,����
//����ֵ:0,����
//    ����,�������
uint32_t EVAL_AUDIO_Mute(uint32_t Cmd)
{  
	if(Cmd==AUDIO_MUTE_ON)
	{
		WM8978_HPvol_Set(0,0);
		WM8978_SPKvol_Set(0);
	}else
	{
		WM8978_HPvol_Set(volume*0.63,volume*0.63);
		WM8978_SPKvol_Set(volume*0.63);
	}
	return 0;
}  
//������Ƶ������
//Addr:��Ƶ�����������׵�ַ
//Size:��Ƶ��������С(��λ:harf word,Ҳ����2���ֽ�)
void Audio_MAL_Play(uint32_t Addr, uint32_t Size)
{  
	u16 i;
	u8 t=i2ssavebuf;
	u8 *p=(u8*)Addr;
	u8 curplay=i2splaybuf;	//��ǰ���ڲ��ŵĻ���֡���
	if(curplay)curplay--;
	else curplay=AUDIO_BUF_NUM-1; 
	audiostatus=1;
	t++;
	if(t>(AUDIO_BUF_NUM-1))t=0; 
	if(t==curplay)			//д���������˵�ǰ���ڲ��ŵ�֡,������һ֡
	{
		t++;
		if(t>(AUDIO_BUF_NUM-1))t=0;   
		printf("bad position:%d\r\n",t);
	}
	i2ssavebuf=t;
	for(i=0;i<Size*2;i++)
	{
		i2sbuf[i2ssavebuf][i]=p[i];
	}
	I2S_Play_Start();		//����DMA  
}






















