/**
  ******************************************************************************
  * @file    
  * @author  yanly
  * @version 
  * @date    2014/10/31
  * @brief   achieve multi timer task,System timer task for application,base 1ms
  ******************************************************************************/

#include "ostmr1.h"
#include "osqtmr.h"
#include "ostmr.h"


/*
  Macros
*/
#define NUM_OF_QTASK     10


/*
  Local types
*/







/*
  Exported variables
*/

/*
  Local variables
*/
static	u16	volatile	_quickQTaskCnt;
static	u16	volatile	_fastQTaskCnt;
static sysTimer_t		  _quickQTasks[NUM_OF_QTASK];
static sysTimer_t		  _fastQTasks[NUM_OF_QTASK];

static vfp_t _quickQTaskHandler, _fastQTaskHandler;

static void systmr_dummp(void)
{

}

static ostmrStatus_t t_systmr_status(ostmrID_t *taskID)
{
	if(taskID->number >= NUM_OF_QTASK)
  {
    return (OSTMR_STATUS_ERR);
  }  
   
  switch(taskID->speed)
  {
    case OSTMR_QUICK:          
      return (_quickQTasks[taskID->number].status);
    
    case OSTMR_FAST:    
    	return (_fastQTasks[taskID->number].status);
//    break;
    	
    default:
      return (OSTMR_STATUS_ERR);           
  }  
   
}


static void systmr_quickQTaskHandler(void)
{
  u8 __i;

  for(__i=0; __i<NUM_OF_QTASK; __i++)
  {
    if(_quickQTasks[__i].status == OSTMR_ACTIVE)
    {
      _quickQTasks[__i].currentValue--; 
      if(_quickQTasks[__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_quickQTasks[__i].timertask))();
        if(_quickQTasks[__i].mode == OSTMR_PERIODIC)
        {
          _quickQTasks[__i].currentValue = _quickQTasks[__i].value;
        }
        else if(_quickQTasks[__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _quickQTasks[__i].status = OSTMR_INACTIVE;
        }
        else if(_quickQTasks[__i].mode == OSTMR_STOP_WATCH)
        {
          _quickQTasks[__i].status = OSTMR_FINISHED;
        }
      }
    }
  }    
}

static void systmr_fastQTaskHandler(void)
{
  u8 __i;

  for(__i=0; __i<NUM_OF_QTASK; __i++)
  {
    if(_fastQTasks[__i].status == OSTMR_ACTIVE)
    {
      _fastQTasks[__i].currentValue--; 
      if(_fastQTasks[__i].currentValue == TIMEOUT)     
      {
        /* run task */
        (*(_fastQTasks[__i].timertask))();
        if(_fastQTasks[__i].mode == OSTMR_PERIODIC)
        {
          _fastQTasks[__i].currentValue = _fastQTasks[__i].value;
        }
        else if(_fastQTasks[__i].mode == OSTMR_ATOMIC)
        {
          /* delete time task because of atomic timer task */
          _fastQTasks[__i].status = OSTMR_INACTIVE;
        }
        else if(_fastQTasks[__i].mode == OSTMR_STOP_WATCH)
        {
          _fastQTasks[__i].status = OSTMR_FINISHED;
        }
      }
    }
  }    
}


ostmrID_t t_systmr_insertQuickTask(vfp_t taskHandler, ostmr_t value, ostmrMode_t mode)
{
  u32 __cnt;
  ostmrID_t  __tmpTmrID;
  sysTimer_t *__ptimer;
  volatile  u16 *__ptaskCnt;
  u16 __ajustValue;
  
  if(value == 0 or value >= 1000)
  {
    goto insertErr;
  }
  else if(_quickQTaskCnt > NUM_OF_QTASK)
  {
    goto insertErr;
  }
  else if(value > 10 and value%10 > 0)
  {
  	goto insertErr;
  }
  else
  {
  	if(value < 10)
  	{
  		__ptaskCnt = &_quickQTaskCnt;
  		__ptimer = &_quickQTasks[0]; 
  		__ajustValue = 1;
  	}
  	else
  	{
  		__ptaskCnt = &_fastQTaskCnt;
  		__ptimer = &_fastQTasks[0]; 
  		__ajustValue = 10;  	
  	}		
    /* assign to qTask */
    for(__cnt=0; __cnt<NUM_OF_QTASK; __cnt++)
    {
      if(__ptimer->status == OSTMR_INACTIVE)
      {
        hwtmr2_disable();
        __ptimer->status       = OSTMR_ACTIVE;
        __ptimer->mode         = mode;
        __ptimer->value        = value/__ajustValue;
        __ptimer->currentValue = value/__ajustValue;       
        __ptimer->timertask    = taskHandler; 
        *__ptaskCnt = *__ptaskCnt + 1;                          /* increment of quick counter */       
        hwtmr2_enable();
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

bool_t b_systmr_deleteQuickTask(ostmrID_t *taskID)
{  
  if(taskID->number >= NUM_OF_QTASK)
  {
    return (FALSE_P);  
  }
  
  switch(taskID->speed)
  {
    case OSTMR_QUICK:
    {           
      if(_quickQTasks[taskID->number].status != OSTMR_INACTIVE)
      {
        hwtmr2_disable();
        _quickQTasks[taskID->number].status =  OSTMR_INACTIVE;
        _quickQTasks[taskID->number].value =0;
        _quickQTasks[taskID->number].timertask = NULL;
        _quickQTaskCnt--;
        hwtmr2_enable();
      }
      else
      {
        return (FALSE_P);
      }
    }
    break;
        
    case OSTMR_FAST:
      if(_fastQTasks[taskID->number].status != OSTMR_INACTIVE)
      {
        hwtmr2_disable();
        _fastQTasks[taskID->number].status =  OSTMR_INACTIVE;
        _fastQTasks[taskID->number].value =0;
        _fastQTasks[taskID->number].timertask = NULL;
        _fastQTaskCnt--;
        hwtmr2_enable();
      }
      else
      {
        return (FALSE_P);
      }    	
    break;
    	    
    default:
    return (FALSE_P);           
  }
  return (TRUE_P);
}



bool_t b_systmr_init(void)
{
  u32 __i;  
  hwtmr2_init();
  _quickQTaskCnt= 0;
  _fastQTaskCnt= 0;
  for(__i =0; __i<(NUM_OF_QTASK); __i++)
  {
    _quickQTasks[__i].status       = OSTMR_INACTIVE;
    _quickQTasks[__i].mode         = OSTMR_NONE;
    _quickQTasks[__i].value        = 0;
    _quickQTasks[__i].currentValue = 0;
    _quickQTasks[__i].timertask    = NULL;    
    _quickQTasks[__i].id.speed     = OSTMR_QUICK;
    _quickQTasks[__i].id.number    = __i;
  }  
 
  for(__i =0; __i<(NUM_OF_QTASK); __i++)
  {
    _fastQTasks[__i].status       = OSTMR_INACTIVE;
    _fastQTasks[__i].mode         = OSTMR_NONE;
    _fastQTasks[__i].value        = 0;
    _fastQTasks[__i].currentValue = 0;
    _fastQTasks[__i].timertask    = NULL;    
    _fastQTasks[__i].id.speed     = OSTMR_QUICK;
    _fastQTasks[__i].id.number    = __i;
  }  
  
  _quickQTaskHandler = systmr_quickQTaskHandler;
  _fastQTaskHandler = systmr_fastQTaskHandler;
  
  
  if(b_hwtmr2_setting( _quickQTaskHandler, _fastQTaskHandler))
  {
    hwtmr2_enable();
    return (TRUE_P);
  }
  else
  {
    hwtmr2_disable();
    return (FALSE_P);
  }
  
}

/* 
  unit in 100ms, if value less than 100ms, it automatic change to 100ms
  for example t_ostmr_create(1) = 100ms
*/


bool_t b_systmr_quickFinished(ostmrID_t *tmrID)
{
  if(t_systmr_status(tmrID) == OSTMR_FINISHED)
		return TRUE_P;
	else
		return FALSE_P;
}

void systmr_set( ostmrID_t *taskID, ostmr_t value)
{
  if(taskID->number < NUM_OF_QTASK)
  {
    hwtmr2_disable();
           
    switch(taskID->speed)
    {
      case OSTMR_QUICK:
      {
        _quickQTasks[taskID->number].status = OSTMR_ACTIVE;
        _quickQTasks[taskID->number].value = value;
      }
      break;
      
      case OSTMR_FAST:
      {
        _fastQTasks[taskID->number].status = OSTMR_ACTIVE;
        _fastQTasks[taskID->number].value = value;
      }
      break;  
                
      default:
      break;       
    }         
    hwtmr2_enable();
  }
}


ostmr_t t_systmr_getValue( ostmrID_t *taskID)
{   
  if(taskID->number < NUM_OF_QTASK)
  {
    hwtmr2_disable();    
    switch(taskID->speed)
    {
      case OSTMR_QUICK:
      {        
        return ( _quickQTasks[taskID->number].value);
      }
      //break;
      
      case OSTMR_FAST:
      {        
        return ( _fastQTasks[taskID->number].value);
      }
      //break;  
                
      default:
        return (0);
      //break;       
    }         
//    hwtmr2_enable();
  }
  else
  {
    return (0);
  }
}


void systmr_quickWait(ostmr_t value)
{
  ostmrID_t __tmpTmrID;

  if(value < 10) /* it must be less than 10ms */
  {
    __tmpTmrID = t_systmr_insertQuickTask(systmr_dummp, value, OSTMR_STOP_WATCH);
    while(!b_systmr_quickFinished(&__tmpTmrID));  /* wait until it timer finish */
    b_systmr_deleteQuickTask(&__tmpTmrID);
  }  
}



void systmr_taskStatusChange(ostmrID_t *taskID, ostmrStatus_t status)
{    
  if(taskID->number < NUM_OF_QTASK)
  {
    hwtmr2_disable(); 
  
    switch(taskID->speed)
    {
      case OSTMR_QUICK:
      {        
        if(_quickQTasks[taskID->number].status != OSTMR_INACTIVE)
        {
          _quickQTasks[taskID->number].status = status;
        }
      }
      break;
      
      case OSTMR_FAST:
      {        
        if(_fastQTasks[taskID->number].status != OSTMR_INACTIVE)
        {
          _fastQTasks[taskID->number].status = status;
        }
      }
      break;
      
      default:
      break;       
    }         
    hwtmr2_enable();    
  }
}

void systmr_enable(void)
{
  hwtmr2_enable(); 
}

void systmr_disable(void)
{
  hwtmr2_disable(); 
}

void systmr_releaseTask(void)
{
  u32 __i;  
  _quickQTaskCnt= 0;
  _fastQTaskCnt= 0;
  for(__i =0; __i<(NUM_OF_QTASK); __i++)
  {
    _quickQTasks[__i].status       = OSTMR_INACTIVE;
    _quickQTasks[__i].mode         = OSTMR_NONE;
    _quickQTasks[__i].value        = 0;
    _quickQTasks[__i].currentValue = 0;
    _quickQTasks[__i].timertask    = NULL;    
    _quickQTasks[__i].id.speed     = OSTMR_QUICK;
    _quickQTasks[__i].id.number    = __i;
  }  
 
  for(__i =0; __i<(NUM_OF_QTASK); __i++)
  {
    _fastQTasks[__i].status       = OSTMR_INACTIVE;
    _fastQTasks[__i].mode         = OSTMR_NONE;
    _fastQTasks[__i].value        = 0;
    _fastQTasks[__i].currentValue = 0;
    _fastQTasks[__i].timertask    = NULL;    
    _fastQTasks[__i].id.speed     = OSTMR_QUICK;
    _fastQTasks[__i].id.number    = __i;
  }  
}

