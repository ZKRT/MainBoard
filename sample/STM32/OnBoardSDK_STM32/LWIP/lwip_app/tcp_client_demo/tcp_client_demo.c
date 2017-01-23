#include "tcp_client_demo.h" 
//#include "delay.h"
#include "usart.h"
#include "led.h"
//#include "key.h"
//#include "lcd.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h" 
#include "sys.h"
 
extern u32 lwip_localtime;
//TCP Client�������ݻ�����
//u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	
//TCP������������������
const u8 *tcp_client_sendbuf="Explorer STM32F407 TCP Client send data\r\n";
tcp_client_handle_s tcpc_s = {0, 0, NULL, NULL, {0}};

//TCP Client ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�������Ϸ�����;1,�����Ϸ�������.
//bit4~0:����
u8 tcp_client_flag;	 

//����Զ��IP��ַ
void tcp_client_set_remoteip(void)
{
	//ǰ����IP���ֺ�DHCP�õ���IPһ��
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
	lwipdev.remoteip[3]= 105; 
}
/**
  * @brief  tcp client ��ʼ�� 
  * @param  None
  * @retval 0-init fail, 1-init success
  */
u8 tcp_client_init(void)
{
	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
	
	tcppcb=tcp_new();	//����һ���µ�pcb
	
	if(tcppcb)			//�����ɹ�
	{
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
		tcpc_s.connect_state = ES_TCPCLIENT_INIT;
		tcpc_s.pcb = tcppcb;
		//todo: ��ʼ��ʱ�����涨ʱ������������
		tcp_client_init_timeout = lwip_localtime;
		return 1;
 	}
	return 0;
}	
void tcp_client_para_init(void)
{}	
void tcp_client_prcs(void)
{
//	if((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0xff))
//		return;
	switch(tcpc_s.connect_state)
	{
		case ES_TCPCLIENT_NONE: //��һ�γ�ʼ��
			tcp_client_set_remoteip();//��ѡ��IP
			printf("Local IP:%d.%d.%d.%d\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
			printf("Remote IP:%d.%d.%d.%d\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//Զ��IP
			printf("Remote Port:%d\n",TCP_CLIENT_PORT);//�ͻ��˶˿ں�
			tcp_client_init(); 
			break;
		case ES_TCPCLIENT_INIT:
			//zkrt_todo: 10s��δ�����ϣ���������
			if(lwip_localtime - tcp_client_init_timeout > 10000)
			{
				tcp_client_init_timeout = lwip_localtime;
				tcp_client_connection_close(tcpc_s.pcb,0);//�ر����ӣ������������  //modify by yanly
				//zkrt_tips: �費��Ҫ��lwip_periodic_handle();
				tcp_client_init(); //���³�ʼ��
			}
			break;
		case ES_TCPCLIENT_CONNECTED:
			if(tcpc_s.recv_state)
			{
				printf("recv: %s\n", tcpc_s.recvbuf);
				tcpc_s.recv_state = 0;
				//todo: �������ݴ���
				//test yanly
				tcp_client_send(tcpc_s.pcb, &tcpc_s, tcpc_s.recvbuf, strlen((char *)tcpc_s.recvbuf));
			}
			break;
		case ES_TCPCLIENT_CLOSING:
			printf("tcp client closing\n");
		 tcpc_s.connect_state = ES_TCPCLIENT_INIT;
		//zkrt_todo: error handle
			break;
		default:break;
	}	
	lwip_periodic_handle();
}
//TCP Client ����
//void tcp_client_test(void)
//{
// 	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
//	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
// 	u8 key;
//	u8 res=0;		
//	u8 t=0; 
//	u8 connflag=0;		//���ӱ��
//	
//	tcp_client_set_remoteip();//��ѡ��IP

//	printf("Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//������IP
//	printf("Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//Զ��IP
//	printf("Remote Port:%d",TCP_CLIENT_PORT);//�ͻ��˶˿ں�
//	tcppcb=tcp_new();	//����һ���µ�pcb
//	
//	if(tcppcb)			//�����ɹ�
//	{
//		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
//		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
// 	}
//	else 
//		res=1;
//	while(res==0)
//	{
//		key=KEY_Scan(0);
//		if(key==WKUP_PRES)break;
//		if(key==KEY0_PRES)//KEY0������,��������
//		{
//			tcp_client_flag|=1<<7;//���Ҫ��������
//		}
//		if(tcp_client_flag&1<<6)//�Ƿ��յ�����?
//		{
//			//LCD_ShowString(30,230,lcddev.width-30,lcddev.height-230,16,tcp_client_recvbuf);//��ʾ���յ�������		
//			printf("recv: %s\n", tcp_client_recvbuf);
//			tcp_client_flag&=~(1<<6);//��������Ѿ���������.
//			
//			//modify by yanly
//			tcp_write(tcppcb,tcp_client_recvbuf, strlen((char *)tcp_client_recvbuf),1); //������ʱ��
////			tcp_output(tcppcb);		//�����ͻ�������е������������ͳ�ȥ
//			
//		}
//		if(tcp_client_flag&1<<5)//�Ƿ�������?
//		{
//			if(connflag==0)
//			{ 
//				//LCD_ShowString(30,190,lcddev.width-30,lcddev.height-190,16,"STATUS:Connected   ");//��ʾ��Ϣ		
//				POINT_COLOR=RED;
//				//LCD_ShowString(30,210,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//��ʾ��Ϣ		
//				POINT_COLOR=BLUE;//��ɫ����
//				connflag=1;//���������
//			} 
//		}else if(connflag)
//		{
// 			//LCD_ShowString(30,190,190,16,16,"STATUS:Disconnected");
//			//LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//����
//			connflag=0;	//������ӶϿ���
//		} 
//		lwip_periodic_handle();
//		delay_ms(2);
//		t++;
//		if(t==200)
//		{
//			if(connflag==0&&(tcp_client_flag&1<<5)==0)//δ������,��������
//			{ 
//				tcp_client_connection_close(tcppcb,0);//�ر�����
//				tcppcb=tcp_new();	//����һ���µ�pcb
//				if(tcppcb)			//�����ɹ�
//				{ 
//					tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
//				}
//			}
//			t=0;
//			LED0=!LED0;
//		}		
//	}
//	tcp_client_connection_close(tcppcb,0);//�ر�TCP Client����
//} 
//lwIP TCP���ӽ�������ûص�����
//err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
//{
//	struct tcp_client_struct *es=NULL;  
//	if(err==ERR_OK)   
//	{
//		es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //�����ڴ�
//		if(es) //�ڴ�����ɹ�
//		{
// 			es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
//			es->pcb=tpcb;  
//			es->p=NULL; 
//			tcp_arg(tpcb,es);        			//ʹ��es����tpcb��callback_arg
//			tcp_recv(tpcb,tcp_client_recv);  	//��ʼ��LwIP��tcp_recv�ص�����   
//			tcp_err(tpcb,tcp_client_error); 	//��ʼ��tcp_err()�ص�����
//			tcp_sent(tpcb,tcp_client_sent);		//��ʼ��LwIP��tcp_sent�ص�����
//			tcp_poll(tpcb,tcp_client_poll,1); 	//��ʼ��LwIP��tcp_poll�ص����� 
// 			tcp_client_flag|=1<<5; 				//������ӵ���������
//			err=ERR_OK;
//		}else
//		{ 
//			tcp_client_connection_close(tpcb,es);//�ر�����
//			err=ERR_MEM;	//�����ڴ�������
//		}
//	}else
//	{
//		tcp_client_connection_close(tpcb,0);//�ر�����
//	}
//	return err;
//}
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	tcp_client_handle_s *es;
	es = &tcpc_s;
	if(err==ERR_OK)   
	{
		printf("tcp_client_connected\n");
		tcpc_s.connect_state = ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
//		tcpc_s.pcb=tpcb; //modify by yanly 
		tcp_arg(tpcb,es);        			//ʹ��es����tpcb��callback_arg
		tcp_recv(tpcb,tcp_client_recv);  	//��ʼ��LwIP��tcp_recv�ص�����   
		tcp_err(tpcb,tcp_client_error); 	//��ʼ��tcp_err()�ص�����
		tcp_sent(tpcb,tcp_client_sent);		//��ʼ��LwIP��tcp_sent�ص�����
		tcp_poll(tpcb,tcp_client_poll,1); 	//��ʼ��LwIP��tcp_poll�ص����� 
	}
	else
	{
		printf("tcp client connected fail num: %d\n", err);
		tcp_client_connection_close(tpcb,0);//�ر�����
	}
	return err;
}
//lwIP tcp_recv()�����Ļص�����
//err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
//{ 
//	u32 data_len=0;
//	struct pbuf *q;
//	struct tcp_client_struct *es;
//	err_t ret_err; 
//	LWIP_ASSERT("arg != NULL",arg != NULL);
//	es=(struct tcp_client_struct *)arg; 
//	if(p==NULL)//����ӷ��������յ��յ�����֡�͹ر�����
//	{
//		es->state=ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������ 
// 		es->p=p; 
//		ret_err=ERR_OK;
//	}
//	else if(err!= ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
//	{ 
//		if(p)
//			pbuf_free(p);//�ͷŽ���pbuf
//		ret_err=err;
//	}
//	else if(es->state==ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
//	{
//		if(p!=NULL)//����������״̬���ҽ��յ������ݲ�Ϊ��ʱ
//		{
//			memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
//			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
//			{
//				//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
//				//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
//				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
//					memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
//				else 
//					memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
//				data_len += q->len;  	
//				if(data_len > TCP_CLIENT_RX_BUFSIZE) 
//					break; //����TCP�ͻ��˽�������,����	
//			}
//			tcp_client_flag|=1<<6;		//��ǽ��յ�������
// 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
//			pbuf_free(p);  	//�ͷ��ڴ�
//			ret_err=ERR_OK;
//		}
//	}
//	else  //���յ����ݵ��������Ѿ��ر�,
//	{ 
//		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
//		es->p=NULL;
//		pbuf_free(p); //�ͷ��ڴ�
//		ret_err=ERR_OK;
//	}
//	return ret_err;
//} 
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
{ 
	u32 data_len=0;
	struct pbuf *q;
	tcp_client_handle_s *es;
	err_t ret_err; 
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(tcp_client_handle_s *)arg; 
	if(p==NULL)//����ӷ��������յ��յ�����֡�͹ر�����
	{
		es->connect_state = ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������ 
		ret_err = ERR_OK;
	}
	else if(err!= ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
	{
		if(p)
			pbuf_free(p);//�ͷŽ���pbuf
		ret_err=err;
		printf("recv err flag: %d\n", err);
	}
	else if(es->connect_state==ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
	{
		if(p!=NULL)//����������״̬���ҽ��յ������ݲ�Ϊ��ʱ
		{
			memset(es->recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
			{
				//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
					memcpy(es->recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
				else 
					memcpy(es->recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_CLIENT_RX_BUFSIZE) 
				{
					printf("tcp client user buffer overflow\n");
					break; //����TCP�ͻ��˽�������,����	
				}
			}
			es->recv_state = 1;		//��ǽ��յ�������
 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
			pbuf_free(p);  	//�ͷ��ڴ�
			ret_err=ERR_OK;
		}
	}
	else  //���յ����ݵ��������Ѿ��ر�,
	{
		//zkrt_todo: ��Ҫ���쳣����
		printf("the tcp user connect state has closed but receives the data\n");
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		pbuf_free(p); //�ͷ��ڴ�
		ret_err=ERR_OK;
	}
	return ret_err;
} 
//lwIP tcp_err�����Ļص�����
void tcp_client_error(void *arg,err_t err)
{  
	//zkrt_todo
	printf("tcp_client_error err num: %d\n", err);
	//�������ǲ����κδ���
} 
//lwIP tcp_poll�Ļص�����
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es; 
	es=(struct tcp_client_struct*)arg;
	if(es!=NULL)  //���Ӵ��ڿ��п��Է�������
	{
		if(tcp_client_flag&(1<<7))	//�ж��Ƿ�������Ҫ���� 
		{
			es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_client_sendbuf),PBUF_POOL);	//�����ڴ� 
			pbuf_take(es->p,(char*)tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf));	//��tcp_client_sentbuf[]�е����ݿ�����es->p_tx��
			tcp_client_senddata(tpcb,es);//��tcp_client_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
			tcp_client_flag&=~(1<<7);	//������ݷ��ͱ�־
			if(es->p)pbuf_free(es->p);	//�ͷ��ڴ�
		}
		else if(es->state==ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb,es);//�ر�TCP����
		} 
		ret_err=ERR_OK;
	}else
	{ 
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT;
	}
	return ret_err;
} 
////lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
//err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_client_struct *es;
//	LWIP_UNUSED_ARG(len);
//	es=(struct tcp_client_struct*)arg;
//	if(es->p)
//		tcp_client_senddata(tpcb,es);//��������
//	return ERR_OK;
//}
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	tcp_client_handle_s *es;
	LWIP_UNUSED_ARG(len);
	es=(tcp_client_handle_s*)arg;
	if(es->send_p)
		tcp_client_senddata(tpcb,es);//��������
	return ERR_OK;
}
////�˺���������������
//void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
//{
//	struct pbuf *ptr; 
// 	err_t wr_err=ERR_OK;
//	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //��Ҫ���͵����ݼ��뵽���ͻ��������
//	{
//		ptr=es->p;
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
//		if(wr_err==ERR_OK)
//		{  
//			es->p=ptr->next;			//ָ����һ��pbuf
//			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
//			pbuf_free(ptr);				//�ͷ�ptr 
//		}else if(wr_err==ERR_MEM)es->p=ptr;
//		tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
//	} 
//}
/*
* @brief tcp cleint send data user api
* ���󷵻���ʱ������������tcp���治��������tcp���ͻص���������� //tips by yanly
*/
err_t tcp_client_send(struct tcp_pcb *tpcb, void *user_arg, u8 *sendbuf, u16 buf_len)
{
	err_t ret = 1;
	tcp_client_handle_s *es = (tcp_client_handle_s *)user_arg;
	es->send_p=pbuf_alloc(PBUF_TRANSPORT, buf_len,PBUF_RAM);	//�����ڴ� 
	pbuf_take(es->send_p, sendbuf, buf_len);	//��buf�����ݿ�����es->p_tx��
	ret = tcp_client_senddata(tpcb,user_arg);//��buf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
//	if((ret ==ERR_OK)&&(es->send_p))
//	{
//		pbuf_free(es->send_p);	//�ͷ��ڴ�	
//		es->send_p = NULL;
//		return ret;
//	}	
	return ret;
}
//�˺�����������pbuf����
err_t tcp_client_senddata(struct tcp_pcb *tpcb, void *user_arg)
{
	tcp_client_handle_s *es = (tcp_client_handle_s *)user_arg;
	struct pbuf *ptr; 
 	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->send_p&&(es->send_p->len<=tcp_sndbuf(tpcb))) //��Ҫ���͵����ݼ��뵽���ͻ��������
	{
		ptr=es->send_p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{
			es->send_p=ptr->next;			//ָ����һ��pbuf
			if(es->send_p)
				pbuf_ref(es->send_p);	//pbuf��ref��һ
			pbuf_free(ptr);				//�ͷ�ptr 
		}
		else if(wr_err==ERR_MEM)
			es->send_p=ptr;
		tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
	}
	
	if((wr_err ==ERR_OK)&&(es->send_p))
	{
		pbuf_free(es->send_p);	//�ͷ��ڴ�	
		es->send_p = NULL;
	}
	return wr_err;	
} 
//�ر��������������
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	//�Ƴ��ص�
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  //zkrt_todo: poll�ص���ʱû��
	tcpc_s.connect_state = ES_TCPCLIENT_INIT;
	tcpc_s.pcb = NULL;
}






















