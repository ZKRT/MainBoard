#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIM4_Int_Init(void);
void lwip_timer_init(void);
extern uint32_t test_systick_compare;
#endif

