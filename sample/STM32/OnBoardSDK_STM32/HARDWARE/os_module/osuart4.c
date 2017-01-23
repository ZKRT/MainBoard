#include <stdio.h>
#include <string.h>
#include "osqtmr.h"
#include "osuart4.h"
#include "hw_usart.h"
#include "includes.h"
#include "gl_flash.h"
 
extern OS_EVENT *Rs485_mbox;			//mail box
//OS_EVENT *Rs485_mbox;			//mail box 
 
static  volatile  u8               _u1WrTxRegEn;
static  volatile  u8               _u1RxEn;        //rx enable flag
static  volatile  u8               _u1TxEn;        //tx enable flag
static  volatile  u8               _u1rxErrflag;      //uart line status error flag
static  volatile  u8               _u1TxCharTmr;    /*uart tx timer,soft wotchdog*/ 
static  volatile  u8               _u1RxCharTmr;    /*rx char internal time*/
static  volatile  u16               _u1FrameTmr;     /*rx frame internal time*/
static  volatile  u8               _u1CharTmrRestart;
static  volatile  u16              _u1FrameTmrRestart;   
static  volatile  u16              _u1txCnt;
static  volatile  u16              _u1rxCnt;
static  volatile  scommTxBuf_t       *_pu1TxPtr;
static  volatile  scommRcvBuf_t       *_pu1RxPtr;
static  volatile  scommRcvBuf_t       _u1RcvBuf;
static  volatile  u8               _u1RcvArea[UART1_RX_BUF_SIZE];
static  volatile  uRxBufStatus_t   _u1RcvStates;
static	volatile	u8							 _u1Txing;
volatile  u8 							 _u1com4_state;				//uart state in 3 seconds
volatile  u8 							 _u1com4_state_tmrcnt;	//timer count

static void Usart1TaskInit(void);
static void Usart1Config(void);
//static void Usart1Config(dev_info_st usart1_t);
static  void  _hwuart1_txIntrSer(void);
static  void  _hwuart1_rxIntrSer(void);
static  void  _hwuart1_1msTask(void);
static  void  _hwuart1_ScanRxProcess(void);
static  void  Usart1RstVariable(void);

void RS485_DE(u8 rs485_t)
{
	if(TX_485 == rs485_t)
		GPIO_SetBits(GPIOD,GPIO_Pin_0);
	if(RX_485 == rs485_t)
		GPIO_ResetBits(GPIOD,GPIO_Pin_0);
}

static void Usart1TaskInit(void)
{
	Usart1RstVariable();
	t_systmr_insertQuickTask(_hwuart1_1msTask, 1, OSTMR_PERIODIC);            
  t_systmr_insertQuickTask(_hwuart1_ScanRxProcess, 10, OSTMR_PERIODIC);   
}
//static void Usart1Config(void)
//{
//	USART_InitTypeDef USART_InitStructure;
//	
//	USART_InitStructure.USART_BaudRate = 9600;
//  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//  USART_InitStructure.USART_StopBits = USART_StopBits_1;
//  USART_InitStructure.USART_Parity = USART_Parity_No;
//  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//  STM_EVAL_COMInit(COM4, &USART_InitStructure);																									//charge com need to change
//	
//  /* Enable the EVAL_COM4 Transmoit interrupt: this interrupt is generated when the 
//     EVAL_COM4 transmit data register is empty */  
////  USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE);
//	USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); 												//charge com need to change

//  /* Enable the EVAL_COM4 Receive interrupt: this interrupt is generated  
//     when the EVAL_COM4 receive data register is not empty */
//  USART_ITConfig(EVAL_COM4, USART_IT_RXNE, ENABLE);	//charge com need to change
//}

static void Usart1Config()
{
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = baudrate_map[dev_info.rs485.baudrate];
  USART_InitStructure.USART_WordLength = databit_map[dev_info.rs485.databit];
  USART_InitStructure.USART_StopBits = stopbit_map[dev_info.rs485.stopbit];
  USART_InitStructure.USART_Parity = parity_map[dev_info.rs485.parity];
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM4, &USART_InitStructure);																									//charge com need to change
	
  /* Enable the EVAL_COM4 Transmoit interrupt: this interrupt is generated when the 
     EVAL_COM4 transmit data register is empty */  
//  USART_ITConfig(EVAL_COM2, USART_IT_TXE, ENABLE);
	USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); 												//charge com need to change

  /* Enable the EVAL_COM4 Receive interrupt: this interrupt is generated  
     when the EVAL_COM4 receive data register is not empty */
//  USART_ITConfig(EVAL_COM4, USART_IT_RXNE, ENABLE);	//charge com need to change
	if(dev_info.rs485.enalbed & 0x01)	
		USART_ITConfig(EVAL_COM4, USART_IT_RXNE, ENABLE);
	else
		USART_ITConfig(EVAL_COM4, USART_IT_RXNE, DISABLE); 
}
void usart4_config_chg(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE);
	USART_ITConfig(EVAL_COM4, USART_IT_RXNE, DISABLE);
	USART_InitStructure.USART_BaudRate = baudrate_map[dev_info.rs485.baudrate];
  USART_InitStructure.USART_WordLength = databit_map[dev_info.rs485.databit];
  USART_InitStructure.USART_StopBits = stopbit_map[dev_info.rs485.stopbit];
  USART_InitStructure.USART_Parity = parity_map[dev_info.rs485.parity];
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  STM_EVAL_COMInit(COM4, &USART_InitStructure);
  USART_ITConfig(EVAL_COM4, USART_IT_RXNE, ENABLE);
	if(dev_info.rs485.enalbed & 0x01)	
		USART_ITConfig(EVAL_COM4, USART_IT_RXNE, ENABLE);
	else
		USART_ITConfig(EVAL_COM4, USART_IT_RXNE, DISABLE); 
}
/******interupt fun handler***********/
void _UART4_IRQHandler(void)
{
  if (USART_GetITStatus(EVAL_COM4, USART_IT_RXNE) != RESET)	//charge com need to change
  {
    /* received data */
    //USART_GetInputString();
		_hwuart1_rxIntrSer();
  }

  /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
  if (USART_GetFlagStatus(EVAL_COM4, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(EVAL_COM4);
		_u1rxErrflag = TRUE;
  }

  if(USART_GetITStatus(EVAL_COM4, USART_IT_TXE) != RESET)
  {   
    /* Write one byte to the transmit data register */
    //USART_SendBufferData();
		//if(_u1WrTxRegEn == TRUE)
		{	
			_hwuart1_txIntrSer();
		}
  }
		//add by yanly160705
	_u1com4_state = UART_CHANNEL_BUSY;
  _u1com4_state_tmrcnt = UART_CHANNEL_BUSY_TIMEROUT;
}
static void Usart1RstVariable(void)
{
//  _u1TxEn = DISABLE;              /*disable tx*/
//  _u1RxEn = DISABLE;              /*disable rx*/
//  _u1Mode = SCOMM_MODE_NONE;       /*none mode*/

  _u1rxErrflag = 0;
  _u1txCnt = 0;
  _u1rxCnt = 0;
  _u1Txing = FALSE;
//	
  memset((u8 *)_u1RcvArea, 0x00, sizeof(_u1RcvArea));
  memset((u8 *)&_u1RcvBuf, 0x00, sizeof(_u1RcvBuf));
  _u1RcvBuf.pscommRcvContent = (u8 *)_u1RcvArea;
  _pu1RxPtr = &_u1RcvBuf;
  
  _u1RcvStates = URX_BUF_IDLE;
  _pu1TxPtr = NULL;
  _u1TxCharTmr = 0;
  _u1RxCharTmr = 0;
//  _u1FrameTmr = 0;
//  _u1WrTxRegEn = FALSE;
	USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); //add yan141111
  _u1CharTmrRestart = UART0_CHAR_TMR_RESTART;     //default set
	_u1com4_state = UART_CHANNEL_IDLE;
	_u1com4_state_tmrcnt = 0;
//  _u1FrameTmrRestart = UART0_FRM_TMR_RESTART;     //default set
}

static  void  _hwuart1_rxIntrSer(void)
{
  if(_u1RcvStates == URX_BUF_IDLE || _u1RcvStates == URX_BUF_BUSY) //and (_u1RxEn == ENABLE)) 
	{
	  if(_u1RcvStates == URX_BUF_IDLE)
	  {
	    _u1RcvStates = URX_BUF_BUSY;
	    _u1rxCnt = 0;
	  }	
    _pu1RxPtr->pscommRcvContent[_u1rxCnt] = USART_ReceiveData(EVAL_COM4);
		_u1rxCnt++;
		_u1RxCharTmr = _u1CharTmrRestart;  //一直循坏扫描这个值，值为0时表示串口帧接收结束
	}	
	else
	{//clear rx fifo
		(void)USART_ReceiveData(EVAL_COM4);
	}
}

static  void  _hwuart1_txIntrSer(void)
{
  if(_pu1TxPtr != NULL)
  {
    if(_u1txCnt < _pu1TxPtr->scommTxLen) 
    {
			/* Write one byte to the transmit data register */
			USART_SendData(EVAL_COM4, _pu1TxPtr->pscommTxContent[_u1txCnt]);
			_u1txCnt++;
      _u1TxCharTmr = _u1CharTmrRestart;
    }
    else
    {
//      _u1WrTxRegEn = FALSE;   
			USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); //add yan141111
      _u1Txing = FALSE;
      
//			_u1RxEn = ENABLE;      
//      if(_u1Mode == SCOMM_MODE_MASTER)
//      {/*provision to rx */   
//        _u1rxErrflag = FALSE;
//        _u1FrameTmr = _u1FrameTmrRestart;  /*主机,发送完成,则启动等待计时器等待接收完*/
//      }
    }
  }
	else
	{
		USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); //add yan141111
	}
}

static  void  _hwuart1_1msTask(void)
{
  if(_u1TxCharTmr != 0)
  {//tx timer timeout
    _u1TxCharTmr--;
    if(_u1TxCharTmr == 0 && _u1Txing == TRUE)  //use delay to judge sent data(one frame) is over
    {
      _u1Txing     = FALSE;
//      _u1WrTxRegEn = FALSE;
			USART_ITConfig(EVAL_COM4, USART_IT_TXE, DISABLE); //add yan141111
      _u1txCnt     = 0;
      _pu1TxPtr    = NULL;
    }
  }
  
  if(_u1RxCharTmr != 0)
  {
    _u1RxCharTmr--;
    if(_u1RxCharTmr == 0)  //use delay to judge receive data(one frame) is over
    {
      if(_u1RcvStates == URX_BUF_BUSY)
      {
        _u1RcvStates = URX_BUF_COMPL;
      }  
    }
  }

//  if(_u1FrameTmr != 0)  //this flag use to control the time of switch master or slave mode, think about use or not in future!!!!
//  {
//    _u1FrameTmr--;
//    if(_u1FrameTmr == 0)
//    {
//      if(_u1Mode == SCOMM_MODE_MASTER)
//      {//master...
//        _u1RxEn = DISABLE;
//        _u1TxEn = ENABLE;
//      }
//      else if(_u1Mode == SCOMM_MODE_SLAVE) 
//      {//slave..
//        _u1RxEn = ENABLE;
//        _u1TxEn = DISABLE;  
//      }      
//    }
//  }  
}


static  void  _hwuart1_ScanRxProcess(void)
{
  if(_u1rxErrflag)
  {//err...clear all variable
    _u1RcvStates = URX_BUF_IDLE;    
    _u1RxCharTmr = TIMEOUT;
    _u1rxCnt = 0;   

//    _u1RxEn = ENABLE;
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {
//      _u1FrameTmr = _u1FrameTmrRestart;
//    }
//    else
//    {
//      _u1FrameTmr = 0;     /*restart frame timer*/
//    }     
    
    _u1rxErrflag = FALSE;   
  }  
  
  if(_u1RcvStates == URX_BUF_COMPL)
  {
    /*rcv data complete*/
    _pu1RxPtr->scommRcvLen = _u1rxCnt;
    _u1RcvStates = URX_BUF_READY;
		OSMboxPost(Rs485_mbox,(void *)_pu1RxPtr); 	//ucos application: send msg to mail box use
//    _u1TxEn = ENABLE;
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {
//      _u1FrameTmr = 0;
//    }
//    else
//    {
//      _u1FrameTmr = _u1FrameTmrRestart;     /*restart frame timer*/
//    }
  }
}

scommReturn_t  t_hwuart4_ReceiveMessage(scommRcvBuf_t *rxMsg)
{
  volatile  scommReturn_t    __rcv = SCOMM_RET_NONE; 
  
  if(rxMsg != NULL)
  {
		if(_u1Txing == FALSE)
		{	
//	    if(_u1RxEn == TRUE)
//	    {
	      if(_u1RcvStates == URX_BUF_READY)
	      {
	        /*copy uart hardware data to upon layer*/
	        rxMsg->scommRcvLen = _pu1RxPtr->scommRcvLen;
	        memcpy((u8 *)(rxMsg->pscommRcvContent), (u8 *)(_pu1RxPtr->pscommRcvContent), _pu1RxPtr->scommRcvLen);

	        _pu1RxPtr->scommRcvLen = 0;   //reset value
	        _u1RcvStates = URX_BUF_IDLE; 
	        __rcv = SCOMM_RET_OK;
	      }
	      else
	      {
	        __rcv = SCOMM_RET_NOREADY;
	      }
//	    }
//	    else
//	    {
//	      __rcv = SCOMM_RET_TIMEOUT;
//	    }
		}
		else
		{
			__rcv = SCOMM_RET_TXING;
		}	
  }
  else
  {
    __rcv = SCOMM_RET_ERR_PARAM;
  }
  
  return (__rcv);
}
scommReturn_t  t_hwuart4_SendMessage(scommTxBuf_t*  txMsg)
{
//  if(_u1TxEn == ENABLE)
//  {
//    if(_u1Mode == SCOMM_MODE_MASTER)
//    {/*master.. provision to rx */
//    }
//    else
//    {/*slave,tx denote process complete*/
//      _u1FrameTmr = TIMEOUT;  
//    }

    while(u1_hwuart4_txing());
   
    if((txMsg != NULL) && (txMsg->scommTxLen > 0))
    {
      _u1TxCharTmr = _u1CharTmrRestart;
      _pu1TxPtr = txMsg;    /*get tx data pointer*/
      _u1txCnt = 0;//1;   
//      _u1WrTxRegEn = TRUE;    
			USART_ITConfig(EVAL_COM4, USART_IT_TXE, ENABLE); //add yan141111
			_u1Txing = TRUE;

//			/* Write first byte to the transmit data register */
//			USART_SendData(EVAL_COM4, _pu1TxPtr->pscommTxContent[0]);//[_u1txCnt-1]);
      return(SCOMM_RET_OK);
    }
    else
    {
      return(SCOMM_RET_ERR_PARAM);
    }
//  }
//  else
//  {
//    return(SCOMM_RET_TIMEOUT);
//  }
}

u8  u1_hwuart4_txing(void)
{
	return ((USART_GetITStatus(EVAL_COM4, USART_IT_TXE) == SET) ||(_u1Txing == TRUE));
}

void os_usart4(void)
{
	Usart1TaskInit();
	Usart1Config();
}
void os_reset_rcvstate_u4(void)			//mail box use
{
	_pu1RxPtr->scommRcvLen = 0;   //reset value
	_u1RcvStates = URX_BUF_IDLE; 
}	
u8 os_get_uart_chls_u4(void)
{
//	if((_u1Txing == FALSE)&&(_u1RcvStates == URX_BUF_IDLE))
//	{
//		return UART_CHANNEL_IDLE;
//	}
//	return UART_CHANNEL_BUSY;
	return _u1com4_state;
}
