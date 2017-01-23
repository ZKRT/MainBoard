/**
  ******************************************************************************
  * @file    tcp_server.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    22-December-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm.h"

/* Exported constants --------------------------------------------------------*/ 

#define TCP_SERVER_RX_BUFSIZE	200		//定义tcp server最大接收数据长度
#define TCP_SERVER_PORT			15010	//定义tcp server的端口

/* Exported types ------------------------------------------------------------*/

//tcp服务器连接状态
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//服务器接口未初始化
	ES_TCPSERVER_INIT,        //服务器接口已初始化，等待客户端连接
	ES_TCPSERVER_ACCEPTED,		//有客户端连接上了 
	ES_TCPSERVER_CLOSING,		//即将关闭连接
}; 
////LWIP回调函数使用的结构体
//struct tcp_server_struct
//{
//	u8 state;               //当前连接状
//	struct tcp_pcb *pcb;    //指向当前的pcb
//	struct pbuf *p;         //指向接收/或传输的pbuf
//}; 

typedef struct tcp_server_struct {
	u8 server_state; //see tcp_server_states
	u8 recv_state;  //0-数据未接收，1-接收到数据
	struct tcp_pcb *pcb;    //指向当前的pcb
	struct tcp_pcb *localpcb; //暂时不用的 //by yanly
	struct tcp_pcb *connectpcb; //listen时指向的pcb，保存在这里，当服务器关闭时，要释放。//by yanly
	struct pbuf *send_p;  //指向待发送的pbuf，用户要发送的数据会转换缓存到此buf。 \
												//用户可直接调用函数tcp_client_senddata发送，如果未发送完全成功，在发送回调里也会继续tcp_client_senddata继续发送
	ip_addr_t remote_ip; //当前传输过程中的远程IP地址
	u8 recvbuf[TCP_SERVER_RX_BUFSIZE];
	u16 recv_datalen;
}tcp_server_handle_s;

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void tcp_server_test(void);//TCP Server测试函数
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_server_error(void *arg,err_t err);
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);
void tcp_server_para_init(void);
void tcp_server_prcs(void);
err_t tcp_server_send(struct tcp_pcb *tpcb, void *user_arg, u8 *sendbuf, u16 buf_len);
#ifdef HWTEST_FUN				
void tcp_server_hwt_led_control(void);
#endif
//
extern tcp_server_handle_s *tcpserver_sp;

#endif /* __TCP_SERVER_DEMO_H */

/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
