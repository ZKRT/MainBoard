
#ifndef   _OSTMR0_H_
#define   _OSTMR0_H_

#include "sys.h"
#include "port.h"
#define   NUM_OF_HW_TMR_SEPARATOR    2


/*
  Input Args  : Null
  Output Args : Null
  Purpose     : enable hardware timer 0 
*/
extern void hwtmr0_enable(void);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : disable hardware timer 0 
*/
extern void hwtmr0_disable(void);

/*
  Input Args  : timer interrupt timeout period
  Output Args : Null
  Purpose     : reset the timer interrupt timeout period
*/
//extern void hwtmr0_timePeriodSetting(u4_t timerPeriod);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is called by preripheral (hwperip.c) for intialzation
*/
extern void hwtmr0_reg_init(void);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is for intialzation
*/
extern void hwtmr0_init(void);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is called by hwvic.h for interrupt handler
*/
//extern void hwtmr0_irqHandler();
//extern void TIM_NVIC_Configuration(void); //add


/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is only called by _ostmr_init.c to setup the interrupt function handler
*/
extern bool_t b_hwtmr0_setting( vfp_t fast[], vfp_t slow[], vfp_t sec[]);

#endif
