/**
  ******************************************************************************
  * @file    tcp_server.c 
  * @author  ZKRT
  * @version V0.0.1
  * @date    22-December-2016
  * @brief   tcp_server module
  *          + (1) 
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tcp_server_demo.h" 
#include "usart.h"
#include "led.h"
#include "malloc.h"
#include "hwTestHandle.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//TCP������������������
const u8 *tcp_server_sendbuf="Explorer STM32F407 TCP Server send data\r\n";
//TCP Server ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�пͻ���������;1,�пͻ�����������.
//bit4~0:����
u8 tcp_server_flag;	 
struct tcp_server_struct *es_global;  //LWIP�ص�����ʹ�õĽṹ���ȫ��ָ�룬ʧЧʱ��NULL
tcp_server_handle_s tcpserver_s = {0, 0, NULL, NULL, NULL,NULL, 0, {0}, 0};
tcp_server_handle_s *tcpserver_sp;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  tcp server ��ʼ�� 
  * @param  None
  * @retval 0-init fail, 1-init success
  */
u8 tcp_server_init(void)
{
	uint8_t res =0;
	err_t err;
	struct tcp_pcb *tcppcbnew = NULL;  	//����һ��TCP���������ƿ�
	struct tcp_pcb *tcppcbconn = NULL;  	//����һ��TCP���������ƿ�
	
	tcppcbnew=tcp_new();	//����һ���µ�pcb
	if(tcppcbnew)			//�����ɹ�
	{
//		tcpserver_sp->localpcb = tcppcbnew; //by yanly ����Ҫ���棬��tcp_listenʱ�Ѿ����ͷŵ���
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
		if(err==ERR_OK)	//�����
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//����tcppcb�������״̬
			tcp_accept(tcppcbconn,tcp_server_accept); 	//��ʼ��LWIP��tcp_accept�Ļص�����
			tcpserver_sp->connectpcb = tcppcbconn; //zkrt_notice: need test
			res =1;
		} 
		else
		{
			//fail			
		}	
	}
	return res;
}	
/**
  * @brief  tcp server parameter init
  * @param  None
  * @retval None
  */
void tcp_server_para_init(void)
{
	tcpserver_sp = &tcpserver_s;
}	
/**
  * @brief  tcp server ��ѭ��
  * @param  None
  * @retval None
  */
void tcp_server_prcs(void)
{
	char handle_res=0;
	switch(tcpserver_sp->server_state)
	{
		case ES_TCPSERVER_NONE: //��һ�γ�ʼ��
			printf("Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
			printf("Server Port:%d\n",TCP_SERVER_PORT);//�������˿ں�
			if(tcp_server_init() >0)
			{
				tcpserver_sp->server_state = ES_TCPSERVER_INIT;
			}
			else
			{
//				tcp_server_connection_close(tcpserver_sp->localpcb,0);//�ر�TCP Server���� //localpcb�Ѿ����ͷŵ���
				tcp_server_connection_close(tcpserver_sp->connectpcb,0);//�ر�TCP Server���� 
				tcp_server_remove_timewait();  //ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
			}	
			break;
		case ES_TCPSERVER_INIT:
			//wait tcp client connect
			break;
		case ES_TCPSERVER_ACCEPTED:
			if(tcpserver_sp->recv_state)
			{
				ZKRT_LOG(LOG_NOTICE, "recv: ");
				printf_uchar(tcpserver_sp->recvbuf, tcpserver_sp->recv_datalen);
				tcpserver_sp->recv_state = 0;
				//�������ݴ���
				handle_res = hwtest_data_handle(tcpserver_sp->recvbuf, tcpserver_sp->recv_datalen, tcpserver_sp->recvbuf, &tcpserver_sp->recv_datalen);
				if(handle_res == RETURN_SUCCESS_RES)
				{
					tcp_server_send(tcpserver_sp->pcb, tcpserver_sp, tcpserver_sp->recvbuf, tcpserver_sp->recv_datalen);		
					handle_res = 0;
				}				
//				tcp_server_send(tcpserver_sp->pcb, tcpserver_sp, tcpserver_sp->recvbuf, strlen((char *)tcpserver_sp->recvbuf));	//test yanly
			}
			break;
		case ES_TCPSERVER_CLOSING:
			printf("have a tcp conncet client closed\n");
			tcp_server_connection_close(tcpserver_sp->pcb, NULL); //�ͷŵ�ǰ�رյ�����pcb //zkrt_notice
			tcpserver_sp->server_state = ES_TCPSERVER_ACCEPTED;
#ifdef HWTEST_FUN				
			sys_led_timeout = RUN_LED_NETINIT_TIMEOUT;
			sys_led_flag = sys_led_timeout;
#endif	
		//zkrt__todo: error handle
			break;
		default:break;
	}
	lwip_periodic_handle();
}
////TCP Server ����
//void tcp_server_test(void)
//{
//	err_t err;  
//	struct tcp_pcb *tcppcbnew;  	//����һ��TCP���������ƿ�
//	struct tcp_pcb *tcppcbconn;  	//����һ��TCP���������ƿ�
//	
// 	u8 key;
//	u8 res=0;		
//	u8 t=0; 
//	u8 connflag=0;		//���ӱ��
//	
//	printf("Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
//	printf("Server Port:%d",TCP_SERVER_PORT);//�������˿ں�
//	tcppcbnew=tcp_new();	//����һ���µ�pcb
//	if(tcppcbnew)			//�����ɹ�
//	{ 
//		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
//		if(err==ERR_OK)	//�����
//		{
//			tcppcbconn=tcp_listen(tcppcbnew); 			//����tcppcb�������״̬
//			tcp_accept(tcppcbconn,tcp_server_accept); 	//��ʼ��LWIP��tcp_accept�Ļص�����
//		}
//		else 
//			res=1;  
//	}
//	else 
//		res=1;
//	
//	while(res==0)
//	{
//		key=KEY_Scan(0);
//		if(key==WKUP_PRES)
//			break;
//		if(key==KEY0_PRES)//KEY0������,��������
//		{
//			tcp_server_flag|=1<<7;//���Ҫ��������
//		}
//		if(tcp_server_flag&1<<6)//�Ƿ��յ�����?
//		{
////			LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//����һ������
////			LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//��ʾ���յ�������			
//			tcp_server_flag&=~(1<<6);//��������Ѿ���������.
//			
//			//////���ԣ�������ֱ�ӻظ���Զ�� //modify by yanly
////			tcp_write(es_global->pcb,tcp_server_recvbuf,strlen((char*)tcp_server_recvbuf),1); 
////			tcp_output(es_global->pcb);   //�����ͻ�������е����ݷ��ͳ�ȥ
//			
//			
////			es_global->p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//�����ڴ�
////			if(!es_global->p)
////				printf("pbuf alloc failed\n");
////			else
////			{
////				printf("pbuf allo ok\n");
////				pbuf_take(es_global->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
////				tcp_server_senddata(tcppcbconn,es_global); 
////				if(es_global->p!=NULL)
////					pbuf_free(es_global->p); 	//�ͷ��ڴ�	
////			}
//			//////
//		}
//		if(tcp_server_flag&1<<5)//�Ƿ�������?
//		{
//			if(connflag==0)
//			{ 
//				printf("Client IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//�ͻ���IP
//// 				LCD_ShowString(30,170,230,16,16,tbuf);
////				LCD_ShowString(30,190,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ		
//				connflag=1;//���������
//				tcp_write(es_global->pcb,"hello world",strlen("hello world"),1); //test yanly
//				tcp_output(es_global->pcb);   //�����ͻ�������е����ݷ��ͳ�ȥ
//			} 
//		}
//		else if(connflag)
//		{
////			LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//����
//			connflag=0;	//������ӶϿ���
//		}
//		lwip_periodic_handle();
//		delay_ms(2);
//		t++;
//		if(t==200)
//		{
//			t=0;
//			LED0=!LED0;
//		} 
//	}   
//	tcp_server_connection_close(tcppcbnew,0);//�ر�TCP Server����
//	tcp_server_connection_close(tcppcbconn,0);//�ر�TCP Server���� 
//	tcp_server_remove_timewait(); 
//	memset(tcppcbnew,0,sizeof(struct tcp_pcb));
//	memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
//} 

//lwIP tcp_accept()�Ļص�����
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
	err_t ret_err;
	tcp_server_handle_s *es; 
 	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
	es = tcpserver_sp;
 	if(es!=NULL)
	{
		es->server_state = ES_TCPSERVER_ACCEPTED; //��������
		es->pcb=newpcb;
		es->send_p=NULL;
		es->remote_ip = newpcb->remote_ip;
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//��ʼ��tcp_recv()�Ļص�����
		tcp_err(newpcb,tcp_server_error); 	//��ʼ��tcp_err()�ص�����
		tcp_poll(newpcb,tcp_server_poll,1);	//��ʼ��tcp_poll�ص�����
		tcp_sent(newpcb,tcp_server_sent);  	//��ʼ�����ͻص�����
		ZKRT_LOG(LOG_NOTICE, "connet ip =%d.%d.%d.%d\n", (int)(newpcb->remote_ip.addr&0xff), (int)(newpcb->remote_ip.addr>>8)&0xff, (int)((newpcb->remote_ip.addr>>16)&0xff), (int)((newpcb->remote_ip.addr>>24)&0xff));
		ret_err=ERR_OK;
#ifdef HWTEST_FUN				
			sys_led_timeout = RUN_LEN_CLIENT_CONC_TIMEOUT;
			sys_led_flag = sys_led_timeout;
#endif	
	}
	else 
		ret_err=ERR_MEM;
	return ret_err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  tcp_server_handle_s *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(tcp_server_handle_s*)arg;
	es->pcb = tpcb; //��ǰpcbָ����ջص����tpcb
	if(p==NULL) //�ӿͻ��˽��յ�������
	{
		es->server_state = ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
		//zkrt__todo: ������Ҫ���ú���tcp_server_connection_close(tpcb, NULL)����ǰpcb�رգ�����
		es->send_p = p;		
		ret_err = ERR_OK;
	}
	else if(err!=ERR_OK)	//�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
	{
		if(p)
			pbuf_free(p);	//�ͷŽ���pbuf
		ret_err=err;
		ZKRT_LOG(LOG_NOTICE, "err flag: %d\n", err);
	}
	else if(es->server_state==ES_TCPSERVER_ACCEPTED) 	//��������״̬
	{
		if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
		{
			memset(es->recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
			{
				//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
					memcpy(es->recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
				else 
					memcpy(es->recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) 
					break; //����TCP�ͻ��˽�������,����	
			}
			es->recv_datalen = data_len; //add by yanly
//			//test by yanly
//			tcp_write(es_global->pcb,tcp_server_recvbuf,strlen((char*)tcp_server_recvbuf),1); 
//			tcp_output(es_global->pcb);   //�����ͻ�������е����ݷ��ͳ�ȥ
			
			es->recv_state =1;	//��ǽ��յ�������
			es->remote_ip = tpcb->remote_ip;
			ZKRT_LOG(LOG_NOTICE, "recv ip =%d.%d.%d.%d\n", (int)(tpcb->remote_ip.addr&0xff), (int)(tpcb->remote_ip.addr>>8)&0xff, (int)((tpcb->remote_ip.addr>>16)&0xff), (int)((tpcb->remote_ip.addr>>24)&0xff));
//			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
//			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
//			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
//			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
			pbuf_free(p);  	//�ͷ��ڴ�
			ret_err=ERR_OK;
		}
	}
	else	//�������ر���
	{
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		es->send_p=NULL;
		pbuf_free(p);	//�ͷ��ڴ�
		ret_err=ERR_OK;
	}
	return ret_err;
}
//lwIP tcp_err�����Ļص�����
void tcp_server_error(void *arg,err_t err) //���ô˻ص�ʱ����Ӧ��pcb�Ѿ����ͷŵ���
{  
	LWIP_UNUSED_ARG(err);  
	ZKRT_LOG(LOG_NOTICE, "tcp error:%d\r\n",err);
//	if(arg!=NULL)
//		mem_free(arg);//�ͷ��ڴ�
} 
//lwIP tcp_poll�Ļص�����
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
	if(es!=NULL)
	{
		if(tcp_server_flag&(1<<7))	//�ж��Ƿ�������Ҫ����
		{
			es->send_p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//�����ڴ�
			pbuf_take(es->send_p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
			tcp_server_senddata(tpcb,es); 		//��ѯ��ʱ����Ҫ���͵����� //zkrt__todo: ���ͳɹ��Ÿı���
			tcp_server_flag&=~(1<<7);  			//������ݷ��ͱ�־λ
			if(es->send_p!=NULL)
				pbuf_free(es->send_p); 	//�ͷ��ڴ�	
		}
		else if(es->recv_state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
		{
			tcp_server_connection_close(tpcb,es);//�ر�����
		}
		ret_err=ERR_OK;
	}
	else
	{
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT; 
	}
	return ret_err;
} 
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->send_p)
		tcp_server_senddata(tpcb,es);//��������
	return ERR_OK;
} 
//err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_server_struct *es;
//	LWIP_UNUSED_ARG(len); 
//	es = (struct tcp_server_struct *) arg;
//	if(es->p)tcp_server_senddata(tpcb,es);//��������
//	return ERR_OK;
//} 
//�˺���������������
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	struct pbuf *ptr;
	u16 plen;
	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->send_p&&(es->send_p->len<=tcp_sndbuf(tpcb)))
	{
		ptr=es->send_p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1); //��Ҫ���͵����ݼ��뷢�ͻ��������
		if(wr_err==ERR_OK)
		{ 
			plen=ptr->len;
			es->send_p=ptr->next;			//ָ����һ��pbuf
			if(es->send_p)
				pbuf_ref(es->send_p);	//pbuf��ref��һ
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
		}
		else if(wr_err==ERR_MEM)
			es->send_p=ptr;
		tcp_output(tpcb);   //�����ͻ�������е����ݷ��ͳ�ȥ
	}
} 
//void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	struct pbuf *ptr;
//	u16 plen;
//	err_t wr_err=ERR_OK;
//	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
//	{
//		ptr=es->p;
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1); //��Ҫ���͵����ݼ��뷢�ͻ��������
//		if(wr_err==ERR_OK)
//		{ 
//			plen=ptr->len;
//			es->p=ptr->next;			//ָ����һ��pbuf
//			if(es->p)
//				pbuf_ref(es->p);	//pbuf��ref��һ
//			pbuf_free(ptr);
//			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
//		}
//		else if(wr_err==ERR_MEM)
//			es->p=ptr;
//		tcp_output(tpcb);   //�����ͻ�������е����ݷ��ͳ�ȥ
//	}
//} 
/**
  * @brief  tcp server send data user api
  * @param  tpcb
  * @param  user_arg
  * @param  sendbuf
  * @param  buf_len
  * @retval err_t, ERR_OK-send success, else-send fail
  */
err_t tcp_server_send(struct tcp_pcb *tpcb, void *user_arg, u8 *sendbuf, u16 buf_len)
{
	err_t ret = ERR_OK;
	tcp_server_handle_s *es = (tcp_server_handle_s *)user_arg;
	es->send_p=pbuf_alloc(PBUF_TRANSPORT, buf_len,PBUF_RAM);	//�����ڴ� 
	pbuf_take(es->send_p, sendbuf, buf_len);	//��buf�����ݿ�����es->p_tx��
	tcp_server_senddata(tpcb,user_arg);//��buf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
//	if((ret ==ERR_OK)&&(es->send_p))
//	{
//		pbuf_free(es->send_p);	//�ͷ��ڴ�	
//		es->send_p = NULL;
//		return ret;
//	}	
	return ret;
}
//�ر�tcp����
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
//	tcpserver_sp->server_state = ES_TCPSERVER_NONE;
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//�� tcp.c���� 
extern struct tcp_pcb *tcp_active_pcbs;			//�� tcp.c���� 
extern struct tcp_pcb *tcp_tw_pcbs;				//�� tcp.c����  
//ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	while(tcp_active_pcbs!=NULL)
	{
		lwip_periodic_handle();//������ѯ
		delay_ms(10);//�ȴ�tcp_active_pcbsΪ��  
	}
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//����еȴ�״̬��pcbs
	{
		tcp_pcb_purge(pcb); 
		tcp_tw_pcbs=pcb->next;
		pcb2=pcb;
		pcb=pcb->next;
		memp_free(MEMP_TCP_PCB,pcb2);	
	}
}
#ifdef HWTEST_FUN			
/**
  * @brief  tcp_server_hwt_led_control  Ӳ�����Ե�ϵͳ�ƿ���
  * @retval none
  */
void tcp_server_hwt_led_control(void)
{
	u8 link_state = lwipdev.linkstatus;
	
	if(link_state == LINK_UP)
	{
		if(tcpserver_sp->server_state == ES_TCPSERVER_INIT)
		{
			sys_led_timeout = RUN_LED_NETINIT_TIMEOUT;
		}
		if(sys_led_flag < RUNLED_TIMEOUT_OFFSET)
		{
			sys_led_flag = sys_led_timeout;
		}
		
		if(sys_led_flag == RUNLED_TIMEOUT_OFFSET)
		{
			sys_led_flag = sys_led_timeout;
			_RUN_LED = ! _RUN_LED;
		}
	}	
	

}
#endif
/**
  * @}
  */ 

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

