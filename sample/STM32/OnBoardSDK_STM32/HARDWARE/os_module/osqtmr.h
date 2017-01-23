/*

File Name:    
Author:       
Date:         
Purpose:      OS timer header files for application purpose only
              

*/

#ifndef   _OSQTMR_H_
#define   _OSQTMR_H_

#include "ostmr.h"



/*
  Input Args  : Null
  Output Args : Null
  Purpose     : os timer initialization function, call it before once before ostmr use
*/
extern bool_t b_systmr_init(void);


/*
  Input Args  : value of the timer in 100ms unit
  Output Args : return an timer ID to callee
  Purpose     : Create a timer
*/
extern ostmrID_t t_systmr_create(ostmr_t );

/*
  Input Args  : timer ID, that timer should be deleted when it is called
  Output Args : deleted when true return, otherwise return false
  Purpose     : Delete a timer
*/
extern bool_t b_systmr_deleteQuickTask(ostmrID_t *);

/*
  Input Args  : timer ID
  Output Args : finished when true return, otherwise false
  Purpose     : Check timer timeout
*/
extern bool_t b_systmr_quickFinished(ostmrID_t *);

/*
  Input Args  : timer ID and value in 100ms
  Output Args : NULL
  Purpose     : Reset a new timer value
*/
extern void systmr_set( ostmrID_t *, ostmr_t );

/*
  Input Args  : timer ID
  Output Args : return 100ms unit value
  Purpose     : Get the remain value before timeout
*/
extern ostmr_t t_systmr_getValue( ostmrID_t *);


/*
  Input Args  : timer value in 100ms 
  Output Args : NULL
  Purpose     : wait unit the timer timeout
  example     : systmr_quickWait(2), the program should wait 2ms before go on process (ISR is not disable)
*/
extern void systmr_quickWait(ostmr_t );


/*
  Input Args  : 1.  function handler
                2.  timer value in 100ms
                3.  timer task mode (refer to timer mode)                
  Output Args : NULL
  Purpose     : background timer task is generated and function handler is called when system timer timeout
*/
extern ostmrID_t t_systmr_insertQuickTask(vfp_t, ostmr_t, ostmrMode_t);//ostmr_t <1000 ms, unit: ms 


/*
  Input Args  : 1.  function handler
                2.  timer value in 100ms
                3.  timer task mode (refer to timer mode)                
  Output Args : delete success return true, otherwise false.
  Purpose     : Delete timer task
*/
extern bool_t b_systmr_deleteTask(ostmrID_t *);

/*
  Input Args  : 1.  Timer taks ID
                2.  change to specified status                
  Output Args : NULL
  Purpose     : Change task status
*/
extern void systmr_taskStatusChange(ostmrID_t *, ostmrStatus_t);

/*
  Input Args  : NONE               
  Output Args : NONE
  Purpose     : start hardware timer2
*/
extern void systmr_enable(void);
/*
  Input Args  : NONE               
  Output Args : NONE
  Purpose     : stop hardware timer2
*/
extern void systmr_disable(void);
/*
  Input Args  : NONE               
  Output Args : NONE
  Purpose     : release timer2 task
*/
extern void systmr_releaseTask(void);

#endif
