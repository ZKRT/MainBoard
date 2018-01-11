#include "osusart.h"
#include "osusartX.h"

/********************************************************************************
串口接收函数
********************************************************************************/
scommReturn_t  t_osscomm_ReceiveMessage(uint8_t  *rxData, uint16_t *rxLen, USART_TypeDef* COM)
{
  scommRcvBuf_t    __temp;
  scommReturn_t    __rt;
  uint8_t port;
  __temp.pscommRcvContent = rxData;

	port = whatis_arrynum_of_USART(COM);
	
	__rt = (scommReturn_t)t_hwuartX_ReceiveMessage(&__temp, port, COM);
	
  *rxLen = __temp.scommRcvLen;
  return (__rt);
}
/********************************************************************************
串口发送函数
********************************************************************************/
scommReturn_t  t_osscomm_sendMessage(uint8_t  *txData, uint16_t txLen, USART_TypeDef* COM)
{
	scommTxBuf_t   __temp;
  scommReturn_t  __rt;
  uint8_t port;
  __temp.scommTxLen = txLen;
  __temp.pscommTxContent = txData;
	
	port = whatis_arrynum_of_USART(COM);
	
//	if(port == RS485_COM)
//		RS485_DE(TX_485);
	
	__rt = t_hwuartX_SendMessage(&__temp, port, COM);
  
  while(u1_hwuartX_txing(port, COM));
	
//	if(port == RS485_COM)
//	{
//		RS485_DE(RX_485);
//	}
  return __rt;
}
//初始化
void os_usart_init(void)
{
	os_usartX();
}
