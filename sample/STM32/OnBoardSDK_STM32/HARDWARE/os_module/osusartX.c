/**
  ******************************************************************************
  * @file    osusartX.c 
  * @author  
  * @version 
  * @date    
  * @brief   USART  串口收发均在后台中断中中完成
  ******************************************************************************
  * @COPYRIGHT
  */ 

/* Includes ------------------------------------------------------------------*/
#include "osqtmr.h"
#include "osusartX.h"

typedef struct
{
	volatile  uint8_t               _u1WrTxRegEn;
	volatile  uint8_t               _u1RxEn;        //rx enable flag
	volatile  uint8_t               _u1TxEn;        //tx enable flag
	volatile  uint8_t               _u1rxErrflag;      //uart line status error flag
	volatile  uint8_t               _u1TxCharTmr;    /*uart tx timer,soft wotchdog*/ 
	volatile  uint8_t               _u1RxCharTmr;    /*rx char internal time*/
	volatile  uint16_t               _u1FrameTmr;     /*rx frame internal time*/
	volatile  uint8_t               _u1CharTmrRestart;
	volatile  uint16_t              _u1FrameTmrRestart;   
	volatile  uint16_t              _u1txCnt;
	volatile  uint16_t              _u1rxCnt;
	//static  volatile  scommmode_t      _u1Mode;
	volatile  scommTxBuf_t       *_pu1TxPtr;
	volatile  scommRcvBuf_t       *_pu1RxPtr;
	volatile  scommRcvBuf_t       _u1RcvBuf;
	volatile  uint8_t               _u1RcvArea[USART_RX_DATA_SIZE];
	volatile  uRxBufStatus_t   _u1RcvStates;
	volatile	uint8_t							 _u1Txing;
	//volatile  uint8_t 							 _u1com1_state;				//uart state in 3 seconds
	//volatile  uint8_t 							 _u1com1_state_tmrcnt;	//timer count	

}osusartx_st;	

static osusartx_st osusartX_state[USART_MAX_INDEX];
USART_TypeDef* UsartInstance[USART_MAX_INDEX] = {USART2, USART3, UART4, USART6}; //zkrt_notice: 根据芯片自定义
const uint32_t uart_baudrate[USART_MAX_INDEX] ={115200, 115200, 115200, 2400} ;//zkrt_notice: 根据芯片自定义

//task init
static void UsartXTaskInit(void);

//tx inter handle
static  void  _hwuartX_txIntrSer(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM);
//rx inter handle
static  void  _hwuartX_rxIntrSer(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM);

//all timer task
static  void  _hwuartX_1msTask(void);
static  void  _hwuartX_ScanRxProcess(void);

//parameter reset init
static  void  UsartXRstVariable(void);
static  void  UsartXRstVariable_num(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM);

/**
  * @brief  找出USART TYPE 对应的osusartX_state 数组索引号码
  * @param  None
  * @retval None
  */
uint8_t whatis_arrynum_of_USART(USART_TypeDef* COM)
{
	uint8_t i;
	for(i=0; i< USART_MAX_INDEX; i++)
	{
		if(UsartInstance[i] == COM)
			return i;
	}
	return 0xff;
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void UsartXTaskInit(void)
{
	UsartXRstVariable();
	t_systmr_insertQuickTask(_hwuartX_1msTask, 1, OSTMR_PERIODIC);   
	t_systmr_insertQuickTask(_hwuartX_ScanRxProcess, 10, OSTMR_PERIODIC);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static  void  UsartXRstVariable_num(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM)
{
	
  ustate->_u1rxErrflag = 0;
  ustate->_u1txCnt = 0;
  ustate->_u1rxCnt = 0;
  ustate->_u1Txing = FALSE;
	
  memset((uint8_t *)ustate->_u1RcvArea, 0x00, sizeof(ustate->_u1RcvArea));
  memset((uint8_t *)&ustate->_u1RcvBuf, 0x00, sizeof(ustate->_u1RcvBuf));
  ustate->_u1RcvBuf.pscommRcvContent = (uint8_t *)ustate->_u1RcvArea;
  ustate->_pu1RxPtr = &ustate->_u1RcvBuf;
  
  ustate->_u1RcvStates = URX_BUF_IDLE;
  ustate->_pu1TxPtr = NULL;
  ustate->_u1TxCharTmr = 0;
  ustate->_u1RxCharTmr = 0;
//  ustate->_u1FrameTmr = 0;
//  ustate->_u1WrTxRegEn = FALSE;
	
	USART_ITConfig(UsartInstance[ustate_item], USART_IT_TXE, DISABLE);
	
	if(uart_baudrate[ustate_item] >= 4800)
		ustate->_u1CharTmrRestart = UART0_CHAR_TMR_RESTART;
	else
		ustate->_u1CharTmrRestart = UART0_CHAR_TMR_RESTART+3;
//	ustate->_u1com1_state = UART_CHANNEL_IDLE;
//	ustate->_u1com1_state_tmrcnt = 0;
//  ustate->_u1FrameTmrRestart = UART0_FRM_TMR_RESTART;     //default set	
}	
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void UsartXRstVariable(void)
{
	int i;
	for(i=0; i< USART_MAX_INDEX; i++)
	{
		UsartXRstVariable_num(i, &osusartX_state[i], UsartInstance[i]);
	}
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static  void  _hwuartX_rxIntrSer(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM)
{
  if(ustate->_u1RcvStates == URX_BUF_IDLE || ustate->_u1RcvStates == URX_BUF_BUSY) //and (_u1RxEn == ENABLE)) 
	{
	  if(ustate->_u1RcvStates == URX_BUF_IDLE)
	  {
	    ustate->_u1RcvStates = URX_BUF_BUSY;
	    ustate->_u1rxCnt = 0;
	  }	
    ustate->_pu1RxPtr->pscommRcvContent[ustate->_u1rxCnt] = USART_ReceiveData(UsartInstance[ustate_item]);
		if(ustate->_u1rxCnt >=sizeof(ustate->_u1RcvArea))  //add de bug by yanly
			ustate->_u1rxCnt=0;
		else
			ustate->_u1rxCnt++;
		ustate->_u1RxCharTmr = ustate->_u1CharTmrRestart;  //一直循坏扫描这个值，值为0时表示串口帧接收结束
	}	
	else
	{//clear rx fifo
		(void)USART_ReceiveData(UsartInstance[ustate_item]);
	}
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static  void  _hwuartX_txIntrSer(uint8_t ustate_item, osusartx_st * ustate, USART_TypeDef* COM)
{
  if(ustate->_pu1TxPtr != NULL)
  {
    if(ustate->_u1txCnt < ustate->_pu1TxPtr->scommTxLen) 
    {
			/* Write one byte to the transmit data register */
			USART_SendData(UsartInstance[ustate_item], ustate->_pu1TxPtr->pscommTxContent[ustate->_u1txCnt]);
			ustate->_u1txCnt++;
      ustate->_u1TxCharTmr = ustate->_u1CharTmrRestart;
    }
    else
    {
//      _u1WrTxRegEn = FALSE;   
			USART_ITConfig(UsartInstance[ustate_item], USART_IT_TXE, DISABLE); //add yan141111
      ustate->_u1Txing = FALSE;
      
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
		USART_ITConfig(UsartInstance[ustate_item], USART_IT_TXE, DISABLE);
	}
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static  void  _hwuartX_1msTask(void)
{
	int i;
	for(i=0; i<USART_MAX_INDEX; i++)
	{
		if(osusartX_state[i]._u1TxCharTmr != 0)
		{//tx timer timeout
			osusartX_state[i]._u1TxCharTmr--;
			if(osusartX_state[i]._u1TxCharTmr == 0 && osusartX_state[i]._u1Txing == TRUE)  //use delay to judge sent data(one frame) is over
			{
				osusartX_state[i]._u1Txing     = FALSE;
	//      _u1WrTxRegEn = FALSE;
				USART_ITConfig(UsartInstance[i], USART_IT_TXE, DISABLE); //add yan141111 
				osusartX_state[i]._u1txCnt     = 0;
				osusartX_state[i]._pu1TxPtr    = NULL;
			}
		}
		
		if(osusartX_state[i]._u1RxCharTmr != 0)
		{
			osusartX_state[i]._u1RxCharTmr--;
			if(osusartX_state[i]._u1RxCharTmr == 0)  //use delay to judge receive data(one frame) is over
			{
				if(osusartX_state[i]._u1RcvStates == URX_BUF_BUSY)
				{
					osusartX_state[i]._u1RcvStates = URX_BUF_COMPL;
				}  
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
/**
  * @brief  
  * @param  None
  * @retval None
  */
static  void  _hwuartX_ScanRxProcess(void)
{
	int i;
	for(i=0; i<USART_MAX_INDEX; i++)
  {
		if(osusartX_state[i]._u1rxErrflag)
		{//err...clear all variable
			osusartX_state[i]._u1RcvStates = URX_BUF_IDLE;    
			osusartX_state[i]._u1RxCharTmr = TIMEOUT;
			osusartX_state[i]._u1rxCnt = 0;   

	//    _u1RxEn = ENABLE;
	//    if(_u1Mode == SCOMM_MODE_MASTER)
	//    {
	//      _u1FrameTmr = _u1FrameTmrRestart;
	//    }
	//    else
	//    {
	//      _u1FrameTmr = 0;     /*restart frame timer*/
	//    }     
			
			osusartX_state[i]._u1rxErrflag = FALSE;   
		}  
		
		if(osusartX_state[i]._u1RcvStates == URX_BUF_COMPL)
		{
			/*rcv data complete*/
			osusartX_state[i]._pu1RxPtr->scommRcvLen = osusartX_state[i]._u1rxCnt;
			osusartX_state[i]._u1RcvStates = URX_BUF_READY;
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
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
scommReturn_t  t_hwuartX_ReceiveMessage(scommRcvBuf_t *rxMsg, uint8_t ustate_item, USART_TypeDef* COM)
{
  volatile  scommReturn_t    __rcv = SCOMM_RET_NONE; 
  
  if(rxMsg != NULL)
  {
		if(osusartX_state[ustate_item]._u1Txing == FALSE)
		{	
//	    if(_u1RxEn == TRUE)
//	    {
	      if(osusartX_state[ustate_item]._u1RcvStates == URX_BUF_READY)
	      {
	        /*copy uart hardware data to upon layer*/
	        rxMsg->scommRcvLen = osusartX_state[ustate_item]._pu1RxPtr->scommRcvLen;
	        memcpy((uint8_t *)(rxMsg->pscommRcvContent), (uint8_t *)(osusartX_state[ustate_item]._pu1RxPtr->pscommRcvContent), osusartX_state[ustate_item]._pu1RxPtr->scommRcvLen);

	        osusartX_state[ustate_item]._pu1RxPtr->scommRcvLen = 0;   //reset value
	        osusartX_state[ustate_item]._u1RcvStates = URX_BUF_IDLE; 
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
/**
  * @brief  
  * @param  None
  * @retval None
  */
uint8_t  u1_hwuartX_txing(uint8_t ustate_item, USART_TypeDef* COM)
{
	return ((USART_GetITStatus(UsartInstance[ustate_item], USART_IT_TXE) == SET) ||(osusartX_state[ustate_item]._u1Txing == TRUE));
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
scommReturn_t  t_hwuartX_SendMessage(scommTxBuf_t*  txMsg, uint8_t ustate_item, USART_TypeDef* COM)
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

    while(u1_hwuartX_txing(ustate_item, COM));
   
    if((txMsg != NULL) && (txMsg->scommTxLen > 0))
    {
      osusartX_state[ustate_item]._u1TxCharTmr = osusartX_state[ustate_item]._u1CharTmrRestart;
      osusartX_state[ustate_item]._pu1TxPtr = txMsg;    /*get tx data pointer*/
      osusartX_state[ustate_item]._u1txCnt = 0;//1;   
//      _u1WrTxRegEn = TRUE;    
			USART_ITConfig(UsartInstance[ustate_item], USART_IT_TXE, ENABLE); //add yan141111
			osusartX_state[ustate_item]._u1Txing = TRUE;

//			/* Write first byte to the transmit data register */
//			USART_SendData(USART1, _pu1TxPtr->pscommTxContent[0]);//[_u1txCnt-1]);
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
/**
  * @brief  
  * @param  None
  * @retval None
  */
void os_usartX(void)
{
	UsartXTaskInit();
//	Usart1Config();
}
/**
  * @brief  interupt fun handler
  * @param  None
  * @retval None
  */
void _USARTX_IRQHandler(USART_TypeDef* COM)
{
	uint8_t num = whatis_arrynum_of_USART(COM);
  if (USART_GetITStatus(COM, USART_IT_RXNE) != RESET)
  {
    /* received data */
    //USART_GetInputString();
		_hwuartX_rxIntrSer(num, &osusartX_state[num], COM);   //zkrt_must_notice
  }
  /* If overrun condition occurs, clear the ORE flag 
                              and recover communication */
  if (USART_GetFlagStatus(COM, USART_FLAG_ORE) != RESET)
  {
    (void)USART_ReceiveData(COM);
		USART_ClearFlag(COM, USART_FLAG_ORE);
		osusartX_state[num]._u1rxErrflag = TRUE;
  }

  if(USART_GetITStatus(COM, USART_IT_TXE) != RESET)
  {
    /* Write one byte to the transmit data register */
    //USART_SendBufferData();
		//if(_u1WrTxRegEn == TRUE)
		{	
			_hwuartX_txIntrSer(num, &osusartX_state[num], COM);   //zkrt_must_notice
		}
  }
//	_u1com1_state = UART_CHANNEL_BUSY;
//  _u1com1_state_tmrcnt = UART_CHANNEL_BUSY_TIMEROUT;
}

/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void) //zkrt_notice: 根据芯片自定义
{
	_USARTX_IRQHandler(USART2);
}
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void) //zkrt_notice: 根据芯片自定义
{
	_USARTX_IRQHandler(USART3);
}
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void) //zkrt_notice: 根据芯片自定义
{
	_USARTX_IRQHandler(UART4);
}
/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void) //zkrt_notice: 根据芯片自定义
{
	_USARTX_IRQHandler(USART6);
}