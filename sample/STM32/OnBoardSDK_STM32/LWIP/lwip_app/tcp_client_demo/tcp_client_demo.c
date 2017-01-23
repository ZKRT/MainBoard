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
//TCP Client接收数据缓冲区
//u8 tcp_client_recvbuf[TCP_CLIENT_RX_BUFSIZE];	
//TCP服务器发送数据内容
const u8 *tcp_client_sendbuf="Explorer STM32F407 TCP Client send data\r\n";
tcp_client_handle_s tcpc_s = {0, 0, NULL, NULL, {0}};

//TCP Client 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上服务器;1,连接上服务器了.
//bit4~0:保留
u8 tcp_client_flag;	 

//设置远端IP地址
void tcp_client_set_remoteip(void)
{
	//前三个IP保持和DHCP得到的IP一致
	lwipdev.remoteip[0]=lwipdev.ip[0];
	lwipdev.remoteip[1]=lwipdev.ip[1];
	lwipdev.remoteip[2]=lwipdev.ip[2]; 
	lwipdev.remoteip[3]= 105; 
}
/**
  * @brief  tcp client 初始化 
  * @param  None
  * @retval 0-init fail, 1-init success
  */
u8 tcp_client_init(void)
{
	struct tcp_pcb *tcppcb;  	//定义一个TCP服务器控制块
	struct ip_addr rmtipaddr;  	//远端ip地址
	
	tcppcb=tcp_new();	//创建一个新的pcb
	
	if(tcppcb)			//创建成功
	{
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
		tcpc_s.connect_state = ES_TCPCLIENT_INIT;
		tcpc_s.pcb = tcppcb;
		//todo: 开始计时，检测规定时间内有无连接
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
		case ES_TCPCLIENT_NONE: //第一次初始化
			tcp_client_set_remoteip();//先选择IP
			printf("Local IP:%d.%d.%d.%d\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
			printf("Remote IP:%d.%d.%d.%d\n",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//远端IP
			printf("Remote Port:%d\n",TCP_CLIENT_PORT);//客户端端口号
			tcp_client_init(); 
			break;
		case ES_TCPCLIENT_INIT:
			//zkrt_todo: 10s后未连接上，尝试重连
			if(lwip_localtime - tcp_client_init_timeout > 10000)
			{
				tcp_client_init_timeout = lwip_localtime;
				tcp_client_connection_close(tcpc_s.pcb,0);//关闭连接，清除参数配置  //modify by yanly
				//zkrt_tips: 需不需要加lwip_periodic_handle();
				tcp_client_init(); //重新初始化
			}
			break;
		case ES_TCPCLIENT_CONNECTED:
			if(tcpc_s.recv_state)
			{
				printf("recv: %s\n", tcpc_s.recvbuf);
				tcpc_s.recv_state = 0;
				//todo: 解析数据处理
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
//TCP Client 测试
//void tcp_client_test(void)
//{
// 	struct tcp_pcb *tcppcb;  	//定义一个TCP服务器控制块
//	struct ip_addr rmtipaddr;  	//远端ip地址
// 	u8 key;
//	u8 res=0;		
//	u8 t=0; 
//	u8 connflag=0;		//连接标记
//	
//	tcp_client_set_remoteip();//先选择IP

//	printf("Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
//	printf("Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//远端IP
//	printf("Remote Port:%d",TCP_CLIENT_PORT);//客户端端口号
//	tcppcb=tcp_new();	//创建一个新的pcb
//	
//	if(tcppcb)			//创建成功
//	{
//		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
//		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
// 	}
//	else 
//		res=1;
//	while(res==0)
//	{
//		key=KEY_Scan(0);
//		if(key==WKUP_PRES)break;
//		if(key==KEY0_PRES)//KEY0按下了,发送数据
//		{
//			tcp_client_flag|=1<<7;//标记要发送数据
//		}
//		if(tcp_client_flag&1<<6)//是否收到数据?
//		{
//			//LCD_ShowString(30,230,lcddev.width-30,lcddev.height-230,16,tcp_client_recvbuf);//显示接收到的数据		
//			printf("recv: %s\n", tcp_client_recvbuf);
//			tcp_client_flag&=~(1<<6);//标记数据已经被处理了.
//			
//			//modify by yanly
//			tcp_write(tcppcb,tcp_client_recvbuf, strlen((char *)tcp_client_recvbuf),1); //这里暂时把
////			tcp_output(tcppcb);		//将发送缓冲队列中的数据立即发送出去
//			
//		}
//		if(tcp_client_flag&1<<5)//是否连接上?
//		{
//			if(connflag==0)
//			{ 
//				//LCD_ShowString(30,190,lcddev.width-30,lcddev.height-190,16,"STATUS:Connected   ");//提示消息		
//				POINT_COLOR=RED;
//				//LCD_ShowString(30,210,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//提示消息		
//				POINT_COLOR=BLUE;//蓝色字体
//				connflag=1;//标记连接了
//			} 
//		}else if(connflag)
//		{
// 			//LCD_ShowString(30,190,190,16,16,"STATUS:Disconnected");
//			//LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//清屏
//			connflag=0;	//标记连接断开了
//		} 
//		lwip_periodic_handle();
//		delay_ms(2);
//		t++;
//		if(t==200)
//		{
//			if(connflag==0&&(tcp_client_flag&1<<5)==0)//未连接上,则尝试重连
//			{ 
//				tcp_client_connection_close(tcppcb,0);//关闭连接
//				tcppcb=tcp_new();	//创建一个新的pcb
//				if(tcppcb)			//创建成功
//				{ 
//					tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
//				}
//			}
//			t=0;
//			LED0=!LED0;
//		}		
//	}
//	tcp_client_connection_close(tcppcb,0);//关闭TCP Client连接
//} 
//lwIP TCP连接建立后调用回调函数
//err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
//{
//	struct tcp_client_struct *es=NULL;  
//	if(err==ERR_OK)   
//	{
//		es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //申请内存
//		if(es) //内存申请成功
//		{
// 			es->state=ES_TCPCLIENT_CONNECTED;//状态为连接成功
//			es->pcb=tpcb;  
//			es->p=NULL; 
//			tcp_arg(tpcb,es);        			//使用es更新tpcb的callback_arg
//			tcp_recv(tpcb,tcp_client_recv);  	//初始化LwIP的tcp_recv回调功能   
//			tcp_err(tpcb,tcp_client_error); 	//初始化tcp_err()回调函数
//			tcp_sent(tpcb,tcp_client_sent);		//初始化LwIP的tcp_sent回调功能
//			tcp_poll(tpcb,tcp_client_poll,1); 	//初始化LwIP的tcp_poll回调功能 
// 			tcp_client_flag|=1<<5; 				//标记连接到服务器了
//			err=ERR_OK;
//		}else
//		{ 
//			tcp_client_connection_close(tpcb,es);//关闭连接
//			err=ERR_MEM;	//返回内存分配错误
//		}
//	}else
//	{
//		tcp_client_connection_close(tpcb,0);//关闭连接
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
		tcpc_s.connect_state = ES_TCPCLIENT_CONNECTED;//状态为连接成功
//		tcpc_s.pcb=tpcb; //modify by yanly 
		tcp_arg(tpcb,es);        			//使用es更新tpcb的callback_arg
		tcp_recv(tpcb,tcp_client_recv);  	//初始化LwIP的tcp_recv回调功能   
		tcp_err(tpcb,tcp_client_error); 	//初始化tcp_err()回调函数
		tcp_sent(tpcb,tcp_client_sent);		//初始化LwIP的tcp_sent回调功能
		tcp_poll(tpcb,tcp_client_poll,1); 	//初始化LwIP的tcp_poll回调功能 
	}
	else
	{
		printf("tcp client connected fail num: %d\n", err);
		tcp_client_connection_close(tpcb,0);//关闭连接
	}
	return err;
}
//lwIP tcp_recv()函数的回调函数
//err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
//{ 
//	u32 data_len=0;
//	struct pbuf *q;
//	struct tcp_client_struct *es;
//	err_t ret_err; 
//	LWIP_ASSERT("arg != NULL",arg != NULL);
//	es=(struct tcp_client_struct *)arg; 
//	if(p==NULL)//如果从服务器接收到空的数据帧就关闭连接
//	{
//		es->state=ES_TCPCLIENT_CLOSING;//需要关闭TCP 连接了 
// 		es->p=p; 
//		ret_err=ERR_OK;
//	}
//	else if(err!= ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
//	{ 
//		if(p)
//			pbuf_free(p);//释放接收pbuf
//		ret_err=err;
//	}
//	else if(es->state==ES_TCPCLIENT_CONNECTED)	//当处于连接状态时
//	{
//		if(p!=NULL)//当处于连接状态并且接收到的数据不为空时
//		{
//			memset(tcp_client_recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
//			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
//			{
//				//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
//				//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
//				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
//					memcpy(tcp_client_recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
//				else 
//					memcpy(tcp_client_recvbuf+data_len,q->payload,q->len);
//				data_len += q->len;  	
//				if(data_len > TCP_CLIENT_RX_BUFSIZE) 
//					break; //超出TCP客户端接收数组,跳出	
//			}
//			tcp_client_flag|=1<<6;		//标记接收到数据了
// 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
//			pbuf_free(p);  	//释放内存
//			ret_err=ERR_OK;
//		}
//	}
//	else  //接收到数据但是连接已经关闭,
//	{ 
//		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
//		es->p=NULL;
//		pbuf_free(p); //释放内存
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
	if(p==NULL)//如果从服务器接收到空的数据帧就关闭连接
	{
		es->connect_state = ES_TCPCLIENT_CLOSING;//需要关闭TCP 连接了 
		ret_err = ERR_OK;
	}
	else if(err!= ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
	{
		if(p)
			pbuf_free(p);//释放接收pbuf
		ret_err=err;
		printf("recv err flag: %d\n", err);
	}
	else if(es->connect_state==ES_TCPCLIENT_CONNECTED)	//当处于连接状态时
	{
		if(p!=NULL)//当处于连接状态并且接收到的数据不为空时
		{
			memset(es->recvbuf,0,TCP_CLIENT_RX_BUFSIZE);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_CLIENT_RX_BUFSIZE中的数据是否大于TCP_CLIENT_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_CLIENT_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len)) 
					memcpy(es->recvbuf+data_len,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//拷贝数据
				else 
					memcpy(es->recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_CLIENT_RX_BUFSIZE) 
				{
					printf("tcp client user buffer overflow\n");
					break; //超出TCP客户端接收数组,跳出	
				}
			}
			es->recv_state = 1;		//标记接收到数据了
 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
		}
	}
	else  //接收到数据但是连接已经关闭,
	{
		//zkrt_todo: 需要做异常处理
		printf("the tcp user connect state has closed but receives the data\n");
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		pbuf_free(p); //释放内存
		ret_err=ERR_OK;
	}
	return ret_err;
} 
//lwIP tcp_err函数的回调函数
void tcp_client_error(void *arg,err_t err)
{  
	//zkrt_todo
	printf("tcp_client_error err num: %d\n", err);
	//这里我们不做任何处理
} 
//lwIP tcp_poll的回调函数
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es; 
	es=(struct tcp_client_struct*)arg;
	if(es!=NULL)  //连接处于空闲可以发送数据
	{
		if(tcp_client_flag&(1<<7))	//判断是否有数据要发送 
		{
			es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_client_sendbuf),PBUF_POOL);	//申请内存 
			pbuf_take(es->p,(char*)tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf));	//将tcp_client_sentbuf[]中的数据拷贝到es->p_tx中
			tcp_client_senddata(tpcb,es);//将tcp_client_sentbuf[]里面复制给pbuf的数据发送出去
			tcp_client_flag&=~(1<<7);	//清除数据发送标志
			if(es->p)pbuf_free(es->p);	//释放内存
		}
		else if(es->state==ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb,es);//关闭TCP连接
		} 
		ret_err=ERR_OK;
	}else
	{ 
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err=ERR_ABRT;
	}
	return ret_err;
} 
////lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
//err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_client_struct *es;
//	LWIP_UNUSED_ARG(len);
//	es=(struct tcp_client_struct*)arg;
//	if(es->p)
//		tcp_client_senddata(tpcb,es);//发送数据
//	return ERR_OK;
//}
//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	tcp_client_handle_s *es;
	LWIP_UNUSED_ARG(len);
	es=(tcp_client_handle_s*)arg;
	if(es->send_p)
		tcp_client_senddata(tpcb,es);//发送数据
	return ERR_OK;
}
////此函数用来发送数据
//void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
//{
//	struct pbuf *ptr; 
// 	err_t wr_err=ERR_OK;
//	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //将要发送的数据加入到发送缓冲队列中
//	{
//		ptr=es->p;
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
//		if(wr_err==ERR_OK)
//		{  
//			es->p=ptr->next;			//指向下一个pbuf
//			if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
//			pbuf_free(ptr);				//释放ptr 
//		}else if(wr_err==ERR_MEM)es->p=ptr;
//		tcp_output(tpcb);		//将发送缓冲队列中的数据立即发送出去
//	} 
//}
/*
* @brief tcp cleint send data user api
* 错误返回暂时不处理，错误是tcp缓存不够，会在tcp发送回调里继续发送 //tips by yanly
*/
err_t tcp_client_send(struct tcp_pcb *tpcb, void *user_arg, u8 *sendbuf, u16 buf_len)
{
	err_t ret = 1;
	tcp_client_handle_s *es = (tcp_client_handle_s *)user_arg;
	es->send_p=pbuf_alloc(PBUF_TRANSPORT, buf_len,PBUF_RAM);	//申请内存 
	pbuf_take(es->send_p, sendbuf, buf_len);	//将buf的数据拷贝到es->p_tx中
	ret = tcp_client_senddata(tpcb,user_arg);//将buf[]里面复制给pbuf的数据发送出去
//	if((ret ==ERR_OK)&&(es->send_p))
//	{
//		pbuf_free(es->send_p);	//释放内存	
//		es->send_p = NULL;
//		return ret;
//	}	
	return ret;
}
//此函数用来发送pbuf数据
err_t tcp_client_senddata(struct tcp_pcb *tpcb, void *user_arg)
{
	tcp_client_handle_s *es = (tcp_client_handle_s *)user_arg;
	struct pbuf *ptr; 
 	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->send_p&&(es->send_p->len<=tcp_sndbuf(tpcb))) //将要发送的数据加入到发送缓冲队列中
	{
		ptr=es->send_p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{
			es->send_p=ptr->next;			//指向下一个pbuf
			if(es->send_p)
				pbuf_ref(es->send_p);	//pbuf的ref加一
			pbuf_free(ptr);				//释放ptr 
		}
		else if(wr_err==ERR_MEM)
			es->send_p=ptr;
		tcp_output(tpcb);		//将发送缓冲队列中的数据立即发送出去
	}
	
	if((wr_err ==ERR_OK)&&(es->send_p))
	{
		pbuf_free(es->send_p);	//释放内存	
		es->send_p = NULL;
	}
	return wr_err;	
} 
//关闭与服务器的连接
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	//移除回调
	tcp_abort(tpcb);//终止连接,删除pcb控制块
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  //zkrt_todo: poll回调暂时没用
	tcpc_s.connect_state = ES_TCPCLIENT_INIT;
	tcpc_s.pcb = NULL;
}






















