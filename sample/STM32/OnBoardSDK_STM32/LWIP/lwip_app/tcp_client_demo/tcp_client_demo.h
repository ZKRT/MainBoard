/**
  ******************************************************************************
  * @file    tcp_client.h 
  * @author  ZKRT
  * @version V0.0.1
  * @date    13-December-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TCP_CLIENT_DEMO_H
#define __TCP_CLIENT_DEMO_H
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm.h"

/* Exported constants --------------------------------------------------------*/ 

#define TCP_CLIENT_RX_BUFSIZE	200		//����tcp client���������ݳ���
#define	TCP_CLIENT_PORT			15010	//����tcp clientҪ���ӵ�Զ�˶˿�

/* Exported types ------------------------------------------------------------*/

//tcp����������״̬
enum tcp_client_states
{
	ES_TCPCLIENT_NONE = 0,		//û������
	ES_TCPCLIENT_INIT,
	ES_TCPCLIENT_CONNECTED,		//���ӵ��������� 
	ES_TCPCLIENT_CLOSING,		//�ر�����
}; 
//LWIP�ص�����ʹ�õĽṹ��
struct tcp_client_struct
{
	u8 state;               //��ǰ����״
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *p;         //ָ�����/�����pbuf
};  

typedef struct{
	u8 connect_state; //see tcp_client_states
	u8 recv_state;  //0-����δ���գ�1-���յ�����
	struct tcp_pcb *pcb;    //ָ��ǰ��pcb
	struct pbuf *send_p;  //ָ������͵�pbuf���û�Ҫ���͵����ݻ�ת�����浽��buf�� \
												//�û���ֱ�ӵ��ú���tcp_client_senddata���ͣ����δ������ȫ�ɹ����ڷ��ͻص���Ҳ�����tcp_client_senddata��������
	u8 recvbuf[TCP_CLIENT_RX_BUFSIZE];
}tcp_client_handle_s;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void tcp_client_set_remoteip(void);
void tcp_client_test(void);//TCP Client���Ժ���
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);
void tcp_client_error(void *arg,err_t err);
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
err_t tcp_client_senddata(struct tcp_pcb *tpcb, void *user_arg);
err_t tcp_client_send(struct tcp_pcb *tpcb, void *user_arg, u8 *sendbuf, u16 buf_len);
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es );
void tcp_client_para_init(void);
void tcp_client_prcs(void);

#endif /* __TCP_CLIENT_DEMO_H */
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/





















