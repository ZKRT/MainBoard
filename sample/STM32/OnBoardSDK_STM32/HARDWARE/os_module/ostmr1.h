/*

File Name:    
Author:       
Date:         
Purpose:      OSTMR1 header file

*/

#ifndef   _OSTMR1_H_
#define   _OSTMR1_H_

#include "sys.h"
#include "port.h"
/*
  Input Args  : Null
  Output Args : Null
  Purpose     : enable hardware timer 2 
*/
extern void hwtmr2_enable(void);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : disable hardware timer 2 
*/
extern void hwtmr2_disable(void);


/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is for intialzation
*/
extern void hwtmr2_init(void);

/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is called by hwvic.h for interrupt handler
*/
//extern void hwtmr2_irqHandler(void);


/*
  Input Args  : Null
  Output Args : Null
  Purpose     : it is only called by _ostmr_init.c to setup the interrupt function handler
*/
extern bool_t b_hwtmr2_setting( vfp_t,  vfp_t);

#endif
