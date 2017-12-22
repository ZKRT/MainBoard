/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    2014/10/31
  * @brief   achieve multi timer task, base 5ms
  ******************************************************************************/

#include "ostmr0.h"
#include "ostmr.h"
/*
  Macros
*/
#define NUM_OF_TASK     10

/* 
  Local types
*/




/*
  Exported variables
*/

/*
  Local variables
*/
static u16 volatile _fastTaskCnt[NUM_OF_HW_TMR_SEPARATOR], _slowTaskCnt[NUM_OF_HW_TMR_SEPARATOR], _secTaskCnt[NUM_OF_HW_TMR_SEPARATOR];
static sysTimer_t		_fastTasks[NUM_OF_HW_TMR_SEPARATOR][NUM_OF_TASK/2];
static sysTimer_t		_slowTasks[NUM_OF_HW_TMR_SEPARATOR][NUM_OF_TASK/2];
static sysTimer_t		_secTasks[NUM_OF_HW_TMR_SEPARATOR][NUM_OF_TASK/2];

static vfp_t _fastTaskHandler[NUM_OF_HW_TMR_SEPARATOR], _slowTaskHandler[NUM_OF_HW_TMR_SEPARATOR], _secTaskHandler[NUM_OF_HW_TMR_SEPARATOR];

static void ostmr_dummp(void)
{

}

static ostmrStatus_t t_ostmr_status(ostmrID_t *taskID)
{
  u32        __taskodd,__tasknum;
    
  if(taskID->number < NUM_OF_TASK)
  {
    if(taskID->number & 0x00000001)
    {
      __taskodd = 1;
      __tasknum = (taskID->number-1)/2;
    }        
    else
    {
      __taskodd = 0;
      __tasknum = taskID->number/2;
    }
  }
  else
  {
    return (OSTMR_STATUS_ERR);
  } 
  
  switch(taskID->speed)
  {
    case OSTMR_FAST:          
      return (_fastTasks[__taskodd][__tasknum].status);
    
    case OSTMR_SLOW:
      return (_slowTasks[__taskodd][__tasknum].status);
    
    case OSTMR_SEC:
      return (_secTasks[__taskodd][__tasknum].status);
    
    default:
    return (OSTMR_STATUS_ERR);           
  }  
   
}


static void ostmr_fastTaskHandler0(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_fastTasks[0][__i].status == OSTMR_ACTIVE)
    {
    
      _fastTasks[0][__i].currentValue--; 
      if(_fastTasks[0][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_fastTasks[0][__i].timertask))();
        if(_fastTasks[0][__i].mode == OSTMR_PERIODIC)
        {
          _fastTasks[0][__i].currentValue = _fastTasks[0][__i].value;
        }
        else if(_fastTasks[0][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _fastTasks[0][__i].status = OSTMR_INACTIVE;
        }
        else if(_fastTasks[0][__i].mode == OSTMR_STOP_WATCH)
        {
          _fastTasks[0][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

static void ostmr_fastTaskHandler1(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_fastTasks[1][__i].status == OSTMR_ACTIVE)
    {
      _fastTasks[1][__i].currentValue--; 
      if(_fastTasks[1][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_fastTasks[1][__i].timertask))();
        if(_fastTasks[1][__i].mode == OSTMR_PERIODIC)
        {
          _fastTasks[1][__i].currentValue = _fastTasks[1][__i].value;
        }
        else if(_fastTasks[1][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _fastTasks[1][__i].status = OSTMR_INACTIVE;
        }
        else if(_fastTasks[1][__i].mode == OSTMR_STOP_WATCH)
        {
          _fastTasks[1][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

static void ostmr_slowTaskHandler0(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_slowTasks[0][__i].status == OSTMR_ACTIVE)
    {
      _slowTasks[0][__i].currentValue--; 
      if(_slowTasks[0][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_slowTasks[0][__i].timertask))();
        if(_slowTasks[0][__i].mode == OSTMR_PERIODIC)
        {
          _slowTasks[0][__i].currentValue = _slowTasks[0][__i].value;
        }
        else if(_slowTasks[0][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _slowTasks[0][__i].status = OSTMR_INACTIVE;
        }
        else if(_slowTasks[0][__i].mode == OSTMR_STOP_WATCH)
        {
          _slowTasks[0][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

static void ostmr_slowTaskHandler1(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_slowTasks[1][__i].status == OSTMR_ACTIVE)
    {
      _slowTasks[1][__i].currentValue--; 
      if(_slowTasks[1][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_slowTasks[1][__i].timertask))();
        if(_slowTasks[1][__i].mode == OSTMR_PERIODIC)
        {
          _slowTasks[1][__i].currentValue = _slowTasks[1][__i].value;
        }
        else if(_slowTasks[1][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _slowTasks[1][__i].status = OSTMR_INACTIVE;
        }
        else if(_slowTasks[1][__i].mode == OSTMR_STOP_WATCH)
        {
          _slowTasks[1][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

static void ostmr_secTaskHandler0(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_secTasks[0][__i].status == OSTMR_ACTIVE)
    {
      _secTasks[0][__i].currentValue--; 
      if(_secTasks[0][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_secTasks[0][__i].timertask))();
        if(_secTasks[0][__i].mode == OSTMR_PERIODIC)
        {
          _secTasks[0][__i].currentValue = _secTasks[0][__i].value;
        }
        else if(_secTasks[0][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _secTasks[0][__i].status = OSTMR_INACTIVE;
        }
        else if(_secTasks[0][__i].mode == OSTMR_STOP_WATCH)
        {
          _secTasks[0][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

static void ostmr_secTaskHandler1(void)
{
  u32 __i;
  
  for(__i=0; __i<(NUM_OF_TASK/2); __i++)
  {
    if(_secTasks[1][__i].status == OSTMR_ACTIVE)
    {
      _secTasks[1][__i].currentValue--; 
      if(_secTasks[1][__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_secTasks[1][__i].timertask))();
        if(_secTasks[1][__i].mode == OSTMR_PERIODIC)
        {
          _secTasks[1][__i].currentValue = _secTasks[1][__i].value;
        }
        else if(_secTasks[1][__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _secTasks[1][__i].status = OSTMR_INACTIVE;
        }
        else if(_secTasks[1][__i].mode == OSTMR_STOP_WATCH)
        {
          _secTasks[1][__i].status = OSTMR_FINISHED;
        }
      }
    }    
  }
}

ostmrID_t t_ostmr_insertTask(vfp_t taskHandler, ostmr_t value, ostmrMode_t mode)
{

  ostmrID_t  __tmpTmrID;
  ostmr_t    __tmpValue;
  u32       __cnt,__tasknum,__ajustValue;
  sysTimer_t *__ptimer;
  u16       *__ptaskCnt;
  
  __tmpValue = value%10;
  if(__tmpValue > 0 or value<10)
  {
    goto insertErr;
  }
  else
  {
    
    /* assign to fastTask */
    if(value < 100)
    {      
      if(_fastTaskCnt[0] == _fastTaskCnt[1] and _fastTaskCnt[0] <5)
      {
        __tasknum = 0;        
      }
      else if(_fastTaskCnt[0] > _fastTaskCnt[1])
      {
        __tasknum = 1;
      }
      else if(_fastTaskCnt[0] < _fastTaskCnt[1])
      {
        __tasknum = 0;
      }
      else
      {
        goto insertErr;
      }
      __ptaskCnt = (u16 *) &_fastTaskCnt[__tasknum];
      __ptimer = &_fastTasks[__tasknum][0];
      __ajustValue = 1;
    }
    /* assign to slowTask */
    else if(value < 1000)
    {
      if(_slowTaskCnt[0] == _slowTaskCnt[1] and _slowTaskCnt[0] <5)
      {
        __tasknum = 0;        
      }
      else if(_slowTaskCnt[0] > _slowTaskCnt[1])
      {
        __tasknum = 1;
      }
      else if(_slowTaskCnt[0] < _slowTaskCnt[1])
      {
        __tasknum = 0;
      }
      else
      {
        goto insertErr;
      }
      __ptaskCnt = (u16 *) &_slowTaskCnt[__tasknum];
      __ptimer = &_slowTasks[__tasknum][0];    
      __ajustValue = 10;
    }
    /* assign to secTask */
    else
    {
      if(_secTaskCnt[0] == _secTaskCnt[1] and _secTaskCnt[0] <5)
      {
        __tasknum = 0;        
      }
      else if(_secTaskCnt[0] > _secTaskCnt[1])
      {
        __tasknum = 1;
      }
      else if(_secTaskCnt[0] < _secTaskCnt[1])
      {
        __tasknum = 0;
      }
      else
      {
        goto insertErr;
      }
      __ptaskCnt = (u16 *) &_secTaskCnt[__tasknum];
      __ptimer = &_secTasks[__tasknum][0];     
      __ajustValue = 100;
    }
    for(__cnt=0; __cnt<(NUM_OF_TASK/2); __cnt++)
    {
      if(__ptimer->status == OSTMR_INACTIVE)
      {
        hwtmr0_disable();
        __ptimer->status       = OSTMR_ACTIVE;
        __ptimer->mode         = mode;
        __ptimer->value        = (value/(10*__ajustValue));
        __ptimer->currentValue = (value/(10*__ajustValue));       
        __ptimer->timertask    = taskHandler; 
        *__ptaskCnt = *__ptaskCnt +1;       
        hwtmr0_enable();
        return (__ptimer->id);
      }
      else
      {
        __ptimer++;
      }
    }
  }
  
insertErr: 
  __tmpTmrID.speed = OSTMR_SPEED_ERR;
  __tmpTmrID.number = 0xffff;
  return ( __tmpTmrID); 
}

bool_t b_ostmr_deleteTask(ostmrID_t *taskID)
{  
  u32        __taskodd,__tasknum;
    
  if(taskID->number < NUM_OF_TASK)
  {
    if(taskID->number & 0x00000001)
    {
      __taskodd = 1;
      __tasknum = (taskID->number-1)/2;
    }        
    else
    {
      __taskodd = 0;
      __tasknum = taskID->number/2;
    }
  }
  else
  {
    return (FALSE);
  }  
  
  switch(taskID->speed)
  {
    case OSTMR_FAST:
    {           
      if(_fastTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
      {
        hwtmr0_disable();
        _fastTasks[__taskodd][__tasknum].status =  OSTMR_INACTIVE;
        _fastTasks[__taskodd][__tasknum].value =0;
        _fastTasks[__taskodd][__tasknum].timertask = NULL;
        _fastTaskCnt[__taskodd]--;
        hwtmr0_enable();
      }
      else
      {
        return (FALSE);
      }
    }
    break;
    
    case OSTMR_SLOW:
    {
      if(_slowTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
      {
        hwtmr0_disable();
        _slowTasks[__taskodd][__tasknum].status =  OSTMR_INACTIVE;
        _slowTasks[__taskodd][__tasknum].value =0;
        _slowTasks[__taskodd][__tasknum].timertask = NULL;
        _slowTaskCnt[__taskodd]--;
        hwtmr0_enable();
      }
      else
      {
        return (FALSE);
      }   
    }
    break;
    
    case OSTMR_SEC:
    {
      if(_secTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
      {
        hwtmr0_disable();
        _secTasks[__taskodd][__tasknum].status =  OSTMR_INACTIVE;
        _secTasks[__taskodd][__tasknum].value =0;
        _secTasks[__taskodd][__tasknum].timertask = NULL;
        _secTaskCnt[__taskodd]--;
        hwtmr0_enable();
      }
      else
      {
        return (FALSE);
      }   
    }
    break;
    
    default:
    return (FALSE);           
  }
  return (TRUE);
}



bool_t b_ostmr_init(void)
{
  u32 __i, __j;  
  hwtmr0_init();
  for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _fastTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _fastTasks[__j][__i].status       = OSTMR_INACTIVE;
      _fastTasks[__j][__i].mode         = OSTMR_NONE;
      _fastTasks[__j][__i].value        = 0;
      _fastTasks[__j][__i].currentValue = 0;
      _fastTasks[__j][__i].timertask    = NULL;    
      _fastTasks[__j][__i].id.speed  = OSTMR_FAST;
      _fastTasks[__j][__i].id.number = __i*2+__j;
    }  
  }
     
  for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _slowTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _slowTasks[__j][__i].status       = OSTMR_INACTIVE;
      _slowTasks[__j][__i].mode         = OSTMR_NONE;
      _slowTasks[__j][__i].value        = 0;
      _slowTasks[__j][__i].currentValue = 0;
      _slowTasks[__j][__i].timertask    = NULL;     
      _slowTasks[__j][__i].id.speed  = OSTMR_SLOW;
      _slowTasks[__j][__i].id.number = __i*2+__j;
    }  
  }

  for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _secTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _secTasks[__j][__i].status       = OSTMR_INACTIVE;
      _secTasks[__j][__i].mode         = OSTMR_NONE;
      _secTasks[__j][__i].value        = 0;
      _secTasks[__j][__i].currentValue = 0;
      _secTasks[__j][__i].timertask    = NULL;    
      _secTasks[__j][__i].id.speed  = OSTMR_SEC;
      _secTasks[__j][__i].id.number = __i*2+__j;
    }  
  }


  _fastTaskHandler[0] = ostmr_fastTaskHandler0;
  _fastTaskHandler[1] = ostmr_fastTaskHandler1;
  
  _slowTaskHandler[0] = ostmr_slowTaskHandler0;
  _slowTaskHandler[1] = ostmr_slowTaskHandler1;
  
  _secTaskHandler[0] = ostmr_secTaskHandler0;
  _secTaskHandler[1] = ostmr_secTaskHandler1;
  
  if(b_hwtmr0_setting( _fastTaskHandler, _slowTaskHandler, _secTaskHandler))
  {
    hwtmr0_enable();
    return (TRUE);
  }
  else
  {
    hwtmr0_disable();
    return (FALSE);
  }
  
}

/* 
  unit in 100ms, if value less than 100ms, it automatic change to 100ms
  for example t_ostmr_create(1) = 100ms
*/

ostmrID_t t_ostmr_create(ostmr_t value)
{     
  value *= 100;
  return (t_ostmr_insertTask(ostmr_dummp, value, OSTMR_STOP_WATCH));
}

bool_t b_ostmr_delete(ostmrID_t *tmrID)
{
  return(b_ostmr_deleteTask(tmrID));
}

bool_t b_ostmr_finished(ostmrID_t *tmrID)
{
	if(t_ostmr_status(tmrID) == OSTMR_FINISHED)
		return TRUE;
	else
		return FALSE;
}

void ostmr_set( ostmrID_t *taskID, ostmr_t value)
{
  u32        __taskodd,__tasknum;
    
  if(taskID->number < NUM_OF_TASK)
  {
    hwtmr0_disable();
    if(taskID->number & 0x00000001)
    {
      __taskodd = 1;
      __tasknum = (taskID->number-1)/2;
    }        
    else
    {
      __taskodd = 0;
      __tasknum = taskID->number/2;
    }            
    switch(taskID->speed)
    {
      case OSTMR_FAST:
      {
        _fastTasks[__taskodd][__tasknum].status = OSTMR_ACTIVE;
        _fastTasks[__taskodd][__tasknum].value = value;
      }
      break;
      
      case OSTMR_SLOW:
      {
        _slowTasks[__taskodd][__tasknum].status = OSTMR_ACTIVE;
        _slowTasks[__taskodd][__tasknum].value = value;
      }
      break;
      
      case OSTMR_SEC:
      {
        _secTasks[__taskodd][__tasknum].status = OSTMR_ACTIVE;
        _secTasks[__taskodd][__tasknum].value = value;
      }
      break;
      
      default:
      break;       
    }         
    hwtmr0_enable();
  }
}


ostmr_t t_ostmr_getValue( ostmrID_t *taskID)
{
  u32        __taskodd,__tasknum;
    
  if(taskID->number < NUM_OF_TASK)
  {
    hwtmr0_disable();    
    if(taskID->number & 0x00000001)
    {
      __taskodd = 1;
      __tasknum = (taskID->number-1)/2;
    }        
    else
    {
      __taskodd = 0;
      __tasknum = taskID->number/2;
    }       
    switch(taskID->speed)
    {
      case OSTMR_FAST:
      {        
        return ( _fastTasks[__taskodd][__tasknum].value);
      }
//      break;
      
      case OSTMR_SLOW:
      {        
        return (_slowTasks[__taskodd][__tasknum].value);
      }
 //     break;
      
      case OSTMR_SEC:
      {       
        return (_secTasks[__taskodd][__tasknum].value);
      }
     // break;
      
      default:
        return (0);
     // break;       
    }         
    //hwtmr0_enable();
  }
  else
  {
    return (0);
  }
}


void ostmr_wait(ostmr_t value)
{
  ostmrID_t __tmpTmrID;
  value *= 100;
  __tmpTmrID = t_ostmr_insertTask(ostmr_dummp, value, OSTMR_STOP_WATCH);
  while(!b_ostmr_finished(&__tmpTmrID));  /* wait until it timer finish */
  b_ostmr_delete(&__tmpTmrID);  
}



void ostmr_taskStatusChange(ostmrID_t *taskID, ostmrStatus_t status)
{
  u32        __taskodd,__tasknum;
    
  if(taskID->number < NUM_OF_TASK)
  {
    hwtmr0_disable(); 
    if(taskID->number & 0x00000001)
    {
      __taskodd = 1;
      __tasknum = (taskID->number-1)/2;
    }        
    else
    {
      __taskodd = 0;
      __tasknum = taskID->number/2;
    }   
    switch(taskID->speed)
    {
      case OSTMR_FAST:
      {        
        if(_fastTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
        {
          _fastTasks[__taskodd][__tasknum].status = status;
        }
      }
      break;
      
      case OSTMR_SLOW:
      {       
        if(_slowTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
        {
          _slowTasks[__taskodd][__tasknum].status = status;
        }        
      }
      break;
      
      case OSTMR_SEC:
      { 
        if(_secTasks[__taskodd][__tasknum].status != OSTMR_INACTIVE)
        {      
          _secTasks[__taskodd][__tasknum].status = status;
        }
      }
      break;
      
      default:
      break;       
    }         
    hwtmr0_enable();    
  }
}

void ostmr_disable(void)
{
  hwtmr0_disable();
}

void ostmr_enable(void)
{
  hwtmr0_enable();
}

void ostmr_releaseTask(void)
{
  u32 __i, __j;  
  for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _fastTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _fastTasks[__j][__i].status       = OSTMR_INACTIVE;
      _fastTasks[__j][__i].mode         = OSTMR_NONE;
      _fastTasks[__j][__i].value        = 0;
      _fastTasks[__j][__i].currentValue = 0;
      _fastTasks[__j][__i].timertask    = NULL;    
      _fastTasks[__j][__i].id.speed  = OSTMR_FAST;
      _fastTasks[__j][__i].id.number = __i*2+__j;
    }  
  }
     
  for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _slowTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _slowTasks[__j][__i].status       = OSTMR_INACTIVE;
      _slowTasks[__j][__i].mode         = OSTMR_NONE;
      _slowTasks[__j][__i].value        = 0;
      _slowTasks[__j][__i].currentValue = 0;
      _slowTasks[__j][__i].timertask    = NULL;     
      _slowTasks[__j][__i].id.speed  = OSTMR_SLOW;
      _slowTasks[__j][__i].id.number = __i*2+__j;
    }  
  }
	
	for(__j=0; __j<NUM_OF_HW_TMR_SEPARATOR; __j++)
  {
    _secTaskCnt[__j]  = 0;
    for(__i =0; __i<(NUM_OF_TASK/2); __i++)
    {
      _secTasks[__j][__i].status       = OSTMR_INACTIVE;
      _secTasks[__j][__i].mode         = OSTMR_NONE;
      _secTasks[__j][__i].value        = 0;
      _secTasks[__j][__i].currentValue = 0;
      _secTasks[__j][__i].timertask    = NULL;    
      _secTasks[__j][__i].id.speed  = OSTMR_SEC;
      _secTasks[__j][__i].id.number = __i*2+__j;
    }  
  }
}
