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

#define TCP_SERVER_RX_BUFSIZE	200		//����tcp server���������ݳ���
#define TCP_SERVER_PORT			15010	//����tcp server�Ķ˿�

/* Exported types ------------------------------------------------------------*/

//tcp����������״̬
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		//�������ӿ�δ��ʼ��
	ES_TCPSERVER_INIT,        //�������ӿ��ѳ�ʼ�����ȴ��ͻ�������
	ES_TCPSERVER_ACCEPTED,		//�пͻ����������� 
	ES_TCPSERVER_CLOSING,		//�����ر�����
}; 
////LWIP�ص�����ʹ�õĽṹ��
//struct tcp_server_struct
//{
//	u8 state;               //��ǰ����״
//	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
//	struct pbuf *p;         //ָ�����/�����pbuf
//}; 

typedef struct tcp_server_struct {
	u8 server_state; //see tcp_server_states
	u8 recv_state;  //0-����δ���գ�1-���յ�����
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct tcp_pcb *localpcb; //��ʱ���õ� //by yanly
	struct tcp_pcb *connectpcb; //listenʱָ���pcb��������������������ر�ʱ��Ҫ�ͷš�//by yanly
	struct pbuf *send_p;  //ָ������͵�pbuf���û�Ҫ���͵����ݻ�ת�����浽��buf�� \
												//�û���ֱ�ӵ��ú���tcp_client_senddata���ͣ����δ������ȫ�ɹ����ڷ��ͻص���Ҳ�����tcp_client_senddata��������
	ip_addr_t remote_ip; //��ǰ��������е�Զ��IP��ַ
	u8 recvbuf[TCP_SERVER_RX_BUFSIZE];
	u16 recv_datalen;
}tcp_server_handle_s;

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void tcp_server_test(void);//TCP Server���Ժ���
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
