#ifndef _IWDG_H
#define _IWDG_H
#include "sys.h"

void IWDG_Init(uint8_t prer,uint16_t rlr);//IWDG��ʼ��
void IWDG_Feed(void);  //ι������
#endif
