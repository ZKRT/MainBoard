#include "osusart.h"
#include "includes.h"
#include "ostmr.h"
#include "sys_ctrl.h"

OS_EVENT *Plc_com_mbox;
OS_EVENT *Rs232_mbox;
OS_EVENT *Rs485_mbox;
OS_EVENT *UART_mutex[3];

const u32 baudrate_map[] = {1200, 2400, 4800, 7200, 9600, 14400, 19200, 38400, 57600, 115200};
const u16 databit_map[] = {USART_WordLength_8b, USART_WordLength_8b, USART_WordLength_9b};
const u16 parity_map[] = {USART_Parity_No, USART_Parity_Odd, USART_Parity_Even};
const u16 stopbit_map[] = {USART_StopBits_1, USART_StopBits_2};

extern volatile  u8 							 _u1com1_state;
extern volatile  u8 							 _u1com1_state_tmrcnt;
extern volatile  u8 							 _u1com4_state;
extern volatile  u8 							 _u1com4_state_tmrcnt;
extern volatile  u8 							 _u1com5_state;
extern volatile  u8 							 _u1com5_state_tmrcnt;
//extern OS_EVENT *UART_mutex[3];
//OS_EVENT *UART_mutex[3];
//u8 plc_com_sendb[USART_TX_DATA_SIZE];		//global uart send bufffer
//u8 rs232_sendb[USART_TX_DATA_SIZE];
//u8 rs485_sendb[USART_TX_DATA_SIZE];

//------------------------------------------------------------------------------------//
static u8  u1_hwuart_Txing(u8 uPort);
static  void  _hwuart_check_com_state(void);

/********************************************************************************
串口接收函数
********************************************************************************/
scommReturn_t  t_osscomm_ReceiveMessage(u8  *rxData, u16 *rxLen, u8 port)  //upper layer usart receive function  
{
  scommRcvBuf_t    __temp;
  scommReturn_t    __rt;
  
  __temp.pscommRcvContent = rxData;
	
	switch(port)
	{
		case USART1_COM:
			__rt = (scommReturn_t)t_hwuart1_ReceiveMessage(&__temp);
		break;
		case UART4_COM:
			__rt = (scommReturn_t)t_hwuart4_ReceiveMessage(&__temp);
		break;
		case UART5_COM:
			__rt = (scommReturn_t)t_hwuart5_ReceiveMessage(&__temp);
		break;		
    default: return SCOMM_RET_PORT_ERR;
	}	
  *rxLen = __temp.scommRcvLen;
  return (__rt);
}
/********************************************************************************
串口发送函数
********************************************************************************/
scommReturn_t  t_osscomm_sendMessage(u8  *txData, u16 txLen, u8 port)
{
	scommTxBuf_t   __temp;
  scommReturn_t  __rt;
  
  __temp.scommTxLen = txLen;
  __temp.pscommTxContent = txData;
	
	if(port == RS485_COM)
		RS485_DE(TX_485);
	
	switch(port)
	{
		case USART1_COM:
			__rt = (scommReturn_t)t_hwuart1_SendMessage(&__temp);
			break;
		case UART4_COM:
			__rt = (scommReturn_t)t_hwuart4_SendMessage(&__temp);
			break;
		case UART5_COM:
			__rt = (scommReturn_t)t_hwuart5_SendMessage(&__temp);
			break;
    default: return SCOMM_RET_PORT_ERR;
	}	
  while(u1_hwuart_Txing(port));
	
	if(port == RS485_COM)
	{
		OSTimeDlyHMSM(0,0,0,15); 
		RS485_DE(RX_485);
	}
  return __rt;
}
scommReturn_t  t_osscomm_sendMessage_mutex(u8  *txData, u16 txLen, u8 port)
{
	scommTxBuf_t   __temp;
  scommReturn_t  __rt;
	u8 err;
  
  __temp.scommTxLen = txLen;
  __temp.pscommTxContent = txData;
	
	if(port >3)
		return SCOMM_RET_PORT_ERR;
	
	if(port == RS485_COM)
		RS485_DE(TX_485);
	
	OSMutexPend(UART_mutex[port], 0, &err);
	
	switch(port)
	{
		case UART5_COM:
			__rt = (scommReturn_t)t_hwuart5_SendMessage(&__temp);
			rs_led_process(UART5_COM);
			break;
		case UART4_COM:
			__rt = (scommReturn_t)t_hwuart4_SendMessage(&__temp);
			rs_led_process(UART4_COM);
			break;
		case USART1_COM:
			__rt = (scommReturn_t)t_hwuart1_SendMessage(&__temp);
			break;
    default: return SCOMM_RET_PORT_ERR;
	}	
  while(u1_hwuart_Txing(port));
	
	OSMutexPost(UART_mutex[port]);
	
	if(port == RS485_COM)
	{
		OSTimeDlyHMSM(0,0,0,15); 
		RS485_DE(RX_485);
	}
  return __rt;
}
void printf_test(u8 *txData, u16 txLen)
{
#ifdef PRINTF_TEST
	t_osscomm_sendMessage_mutex(txData, txLen, RS232_COM);
#endif
}
void printf_test_v2(u8 *txData)
{
#ifdef PRINTF_TEST
	u16 txLen;
	txLen = (u16)strlen((char*)txData);
	t_osscomm_sendMessage_mutex(txData, txLen, RS232_COM);
	t_osscomm_sendMessage_mutex((u8*)"\r\n", sizeof("\r\n"), RS232_COM);
#endif
}
//------------------------------------------------------------------------------------//
//发送等待
static u8  u1_hwuart_Txing(u8 uPort)
{
  switch(uPort)
  {
    case USART1_COM:
      return (u1_hwuart1_txing());
		case UART4_COM:
			return (u1_hwuart4_txing()); 
		case UART5_COM:
			return (u1_hwuart5_txing()); 
    default:
      return FALSE;
  }
}
void os_usart_init(void)
{
	u8 err;
	
	Plc_com_mbox = OSMboxCreate((void*)0);
	Rs232_mbox = OSMboxCreate((void*)0);
	Rs485_mbox = OSMboxCreate((void*)0);
	
	UART_mutex[PLC_COM] = OSMutexCreate(PLC_COM_MUTEX_PRIO, &err); 
	UART_mutex[RS232_COM] = OSMutexCreate(RS232_MUTEX_PRIO, &err); 
	UART_mutex[RS485_COM] = OSMutexCreate(RS485_MUTEX_PRIO, &err);
	
	os_usart1();
	os_usart4();
	os_usart5();
	t_ostmr_insertTask(_hwuart_check_com_state, 1000, OSTMR_PERIODIC);
}
void os_reset_uart_rcvstate(u8 uPort)
{
  switch(uPort)
  {
    case USART1_COM:
      os_reset_rcvstate_u1();
			break;
		case UART4_COM:
      os_reset_rcvstate_u4();
			break;
		case UART5_COM:
      os_reset_rcvstate_u5();
			break;
    default:
      break;
  }
}
u8 os_get_uart_channel_state(u8 uPort)
{
	u8 state = UART_CHANNEL_IDLE;
	 switch(uPort)
  {
    case USART1_COM:
      state = os_get_uart_chls_u1();
			break;
		case UART4_COM:
      state = os_get_uart_chls_u4();
			break;
		case UART5_COM:
      state = os_get_uart_chls_u5();
			break;
    default:
      break;
  }
	return state;
}
void usart_config_chg(u8 uPort)
{
	switch(uPort)
  {
		case UART4_COM:
			usart4_config_chg();
			break;
		case UART5_COM:
			usart5_config_chg();
			break;
    default:
      break;
  }
}
static  void  _hwuart_check_com_state(void)
{
	if(_u1com1_state!= UART_CHANNEL_IDLE)
	{
		if(_u1com1_state_tmrcnt)
		{
			_u1com1_state_tmrcnt--;
		}
		else
		{
			_u1com1_state = UART_CHANNEL_IDLE;
		}	
	}
	if(_u1com4_state!= UART_CHANNEL_IDLE)
	{
		if(_u1com4_state_tmrcnt)
		{
			_u1com4_state_tmrcnt--;
		}
		else
		{
			_u1com4_state = UART_CHANNEL_IDLE;
		}	
	}
	if(_u1com5_state!= UART_CHANNEL_IDLE)
	{
		if(_u1com5_state_tmrcnt)
		{
			_u1com5_state_tmrcnt--;
		}
		else
		{
			_u1com5_state = UART_CHANNEL_IDLE;
		}	
	}
}
