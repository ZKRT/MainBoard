/*

File Name:    _OSTMR_H_
Author:       
Date:         
Purpose:      OS timer header files for application purpose only
              

*/

#ifndef   _OSTMR_H_
#define   _OSTMR_H_

#include "sys.h"
#include "port.h"

typedef u32 ostmr_t;

typedef enum
{
  OSTMR_INACTIVE = 0,
  OSTMR_ACTIVE,
  OSTMR_PAUSED,
  OSTMR_FINISHED,
  OSTMR_STATUS_ERR
} ostmrStatus_t;


typedef enum
{
  OSTMR_QUICK,    /* 1ms speed time on systmr only */
  OSTMR_FAST,		  /* 10ms speed time */
  OSTMR_SLOW,		  /* 10 times per second (=100ms) */
  OSTMR_SEC,		  /* once per second */
  OSTMR_SPEED_ERR = 0xFFFF,
} ostmrSpeed_t;//enum16(ostmrSpeed);


typedef struct 
{
  volatile ostmrSpeed_t  speed;
  volatile u16          number;
}ostmrID_t;



typedef enum
{
  OSTMR_NONE,
  OSTMR_ATOMIC,       /* only call once and automatic delete  */
  OSTMR_PERIODIC,     /* periodic timer task                  */
  OSTMR_STOP_WATCH,   /* it will stop when timer timeout      */
} ostmrMode_t;



typedef struct
{  
  volatile ostmrStatus_t	  status;  
  volatile ostmrMode_t     mode;
  volatile ostmrID_t       id;
  volatile ostmr_t		      value;
  volatile ostmr_t		      currentValue;
  vfp_t           timertask;
}sysTimer_t;


#define ostmr_taskFinish(x) ostmr_taskStatusChange(x, OSTMR_FINISHED)
#define ostmr_taskRestart(x) ostmr_taskStatusChange(x, OSTMR_ACTIVE)


/*
  Input Args  : Null
  Output Args : Null
  Purpose     : os timer initialization function, call it before once before ostmr use
*/
extern bool_t b_ostmr_init(void);


/*
  Input Args  : value of the timer in 100ms unit
  Output Args : return an timer ID to callee
  Purpose     : Create a timer
*/
extern ostmrID_t t_ostmr_create(ostmr_t );

/*
  Input Args  : timer ID, that timer should be deleted when it is called
  Output Args : deleted when true return, otherwise return false
  Purpose     : Delete a timer
*/
extern bool_t b_ostmr_delete(ostmrID_t *);

/*
  Input Args  : timer ID
  Output Args : finished when true return, otherwise false
  Purpose     : Check timer timeout
*/
extern bool_t b_ostmr_finished(ostmrID_t *);

/*
  Input Args  : timer ID and value in 100ms
  Output Args : NULL
  Purpose     : Reset a new timer value
*/
extern void ostmr_set( ostmrID_t *, ostmr_t );

/*
  Input Args  : timer ID
  Output Args : return 100ms unit value
  Purpose     : Get the remain value before timeout
*/
extern ostmr_t t_ostmr_getValue( ostmrID_t *);


/*
  Input Args  : timer value in 100ms 
  Output Args : NULL
  Purpose     : wait unit the timer timeout
  example     : ostmr_wait(2), the program should wait 200ms before go on process (ISR is not disable)
*/
extern void ostmr_wait(ostmr_t );


/*
  Input Args  : 1.  function handler
                2.  timer value in 100ms
                3.  timer task mode (refer to timer mode)                
  Output Args : NULL
  Purpose     : background timer task is generated and function handler is called when system timer timeout
*/
extern ostmrID_t t_ostmr_insertTask(vfp_t, ostmr_t, ostmrMode_t);  //unit: ms


/*
  Input Args  : 1.  function handler
                2.  timer value in 100ms
                3.  timer task mode (refer to timer mode)                
  Output Args : delete success return true, otherwise false.
  Purpose     : Delete timer task
*/
extern bool_t b_ostmr_deleteTask(ostmrID_t *);

/*
  Input Args  : 1.  Timer taks ID
                2.  change to specified status                
  Output Args : NULL
  Purpose     : Change task status
*/
extern void ostmr_taskStatusChange(ostmrID_t *, ostmrStatus_t);

/*
  Input Args  : NONE             
  Output Args : NONE
  Purpose     : stop hardware timer0
*/
extern void ostmr_disable(void);

/*
  Input Args  : NONE             
  Output Args : NONE
  Purpose     : start hardware timer0
*/
extern void ostmr_enable(void);
/*
  Input Args  : NONE             
  Output Args : NONE
  Purpose     : release timer0 task
*/
extern void ostmr_releaseTask(void);

#endif
