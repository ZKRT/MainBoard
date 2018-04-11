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

//TCP服务器发送数据内容
const u8 *tcp_server_sendbuf="Explorer STM32F407 TCP Server send data\r\n";
//TCP Server 测试全局状态标记变量
//bit7:0,没有数据要发送;1,有数据要发送
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有客户端连接上;1,有客户端连接上了.
//bit4~0:保留
u8 tcp_server_flag;	 
struct tcp_server_struct *es_global;  //LWIP回调函数使用的结构体的全局指针，失效时置NULL
tcp_server_handle_s tcpserver_s = {0, 0, NULL, NULL, NULL,NULL, 0, {0}, 0};
tcp_server_handle_s *tcpserver_sp;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  tcp server 初始化 
  * @param  None
  * @retval 0-init fail, 1-init success
  */
u8 tcp_server_init(void)
{
	uint8_t res =0;
	err_t err;
	struct tcp_pcb *tcppcbnew = NULL;  	//定义一个TCP服务器控制块
	struct tcp_pcb *tcppcbconn = NULL;  	//定义一个TCP服务器控制块
	
	tcppcbnew=tcp_new();	//创建一个新的pcb
	if(tcppcbnew)			//创建成功
	{
//		tcpserver_sp->localpcb = tcppcbnew; //by yanly 不需要保存，在tcp_listen时已经被释放掉了
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
		if(err==ERR_OK)	//绑定完成
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//设置tcppcb进入监听状态
			tcp_accept(tcppcbconn,tcp_server_accept); 	//初始化LWIP的tcp_accept的回调函数
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
  * @brief  tcp server 主循环
  * @param  None
  * @retval None
  */
void tcp_server_prcs(void)
{
	char handle_res=0;
	switch(tcpserver_sp->server_state)
	{
		case ES_TCPSERVER_NONE: //第一次初始化
			printf("Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
			printf("Server Port:%d\n",TCP_SERVER_PORT);//服务器端口号
			if(tcp_server_init() >0)
			{
				tcpserver_sp->server_state = ES_TCPSERVER_INIT;
			}
			else
			{
//				tcp_server_connection_close(tcpserver_sp->localpcb,0);//关闭TCP Server连接 //localpcb已经被释放掉了
				tcp_server_connection_close(tcpserver_sp->connectpcb,0);//关闭TCP Server连接 
				tcp_server_remove_timewait();  //强制删除TCP Server主动断开时的time wait
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
				//解析数据处理
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
			tcp_server_connection_close(tcpserver_sp->pcb, NULL); //释放当前关闭的连接pcb //zkrt_notice
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
////TCP Server 测试
//void tcp_server_test(void)
//{
//	err_t err;  
//	struct tcp_pcb *tcppcbnew;  	//定义一个TCP服务器控制块
//	struct tcp_pcb *tcppcbconn;  	//定义一个TCP服务器控制块
//	
// 	u8 key;
//	u8 res=0;		
//	u8 t=0; 
//	u8 connflag=0;		//连接标记
//	
//	printf("Server IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//服务器IP
//	printf("Server Port:%d",TCP_SERVER_PORT);//服务器端口号
//	tcppcbnew=tcp_new();	//创建一个新的pcb
//	if(tcppcbnew)			//创建成功
//	{ 
//		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,TCP_SERVER_PORT);	//将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
//		if(err==ERR_OK)	//绑定完成
//		{
//			tcppcbconn=tcp_listen(tcppcbnew); 			//设置tcppcb进入监听状态
//			tcp_accept(tcppcbconn,tcp_server_accept); 	//初始化LWIP的tcp_accept的回调函数
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
//		if(key==KEY0_PRES)//KEY0按下了,发送数据
//		{
//			tcp_server_flag|=1<<7;//标记要发送数据
//		}
//		if(tcp_server_flag&1<<6)//是否收到数据?
//		{
////			LCD_Fill(30,210,lcddev.width-1,lcddev.height-1,WHITE);//清上一次数据
////			LCD_ShowString(30,210,lcddev.width-30,lcddev.height-210,16,tcp_server_recvbuf);//显示接收到的数据			
//			tcp_server_flag&=~(1<<6);//标记数据已经被处理了.
//			
//			//////测试，将数据直接回复给远程 //modify by yanly
////			tcp_write(es_global->pcb,tcp_server_recvbuf,strlen((char*)tcp_server_recvbuf),1); 
////			tcp_output(es_global->pcb);   //将发送缓冲队列中的数据发送出去
//			
//			
////			es_global->p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//申请内存
////			if(!es_global->p)
////				printf("pbuf alloc failed\n");
////			else
////			{
////				printf("pbuf allo ok\n");
////				pbuf_take(es_global->p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
////				tcp_server_senddata(tcppcbconn,es_global); 
////				if(es_global->p!=NULL)
////					pbuf_free(es_global->p); 	//释放内存	
////			}
//			//////
//		}
//		if(tcp_server_flag&1<<5)//是否连接上?
//		{
//			if(connflag==0)
//			{ 
//				printf("Client IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//客户端IP
//// 				LCD_ShowString(30,170,230,16,16,tbuf);
////				LCD_ShowString(30,190,lcddev.width-30,lcddev.height-190,16,"Receive Data:");//提示消息		
//				connflag=1;//标记连接了
//				tcp_write(es_global->pcb,"hello world",strlen("hello world"),1); //test yanly
//				tcp_output(es_global->pcb);   //将发送缓冲队列中的数据发送出去
//			} 
//		}
//		else if(connflag)
//		{
////			LCD_Fill(30,170,lcddev.width-1,lcddev.height-1,WHITE);//清屏
//			connflag=0;	//标记连接断开了
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
//	tcp_server_connection_close(tcppcbnew,0);//关闭TCP Server连接
//	tcp_server_connection_close(tcppcbconn,0);//关闭TCP Server连接 
//	tcp_server_remove_timewait(); 
//	memset(tcppcbnew,0,sizeof(struct tcp_pcb));
//	memset(tcppcbconn,0,sizeof(struct tcp_pcb)); 
//} 

//lwIP tcp_accept()的回调函数
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
	err_t ret_err;
	tcp_server_handle_s *es; 
 	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//设置新创建的pcb优先级
	es = tcpserver_sp;
 	if(es!=NULL)
	{
		es->server_state = ES_TCPSERVER_ACCEPTED; //接收连接
		es->pcb=newpcb;
		es->send_p=NULL;
		es->remote_ip = newpcb->remote_ip;
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//初始化tcp_recv()的回调函数
		tcp_err(newpcb,tcp_server_error); 	//初始化tcp_err()回调函数
		tcp_poll(newpcb,tcp_server_poll,1);	//初始化tcp_poll回调函数
		tcp_sent(newpcb,tcp_server_sent);  	//初始化发送回调函数
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
//lwIP tcp_recv()函数的回调函数
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  tcp_server_handle_s *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(tcp_server_handle_s*)arg;
	es->pcb = tpcb; //当前pcb指向接收回调里的tpcb
	if(p==NULL) //从客户端接收到空数据
	{
		es->server_state = ES_TCPSERVER_CLOSING;//需要关闭TCP 连接了
		//zkrt__todo: 这里需要调用函数tcp_server_connection_close(tpcb, NULL)将当前pcb关闭？待测
		es->send_p = p;		
		ret_err = ERR_OK;
	}
	else if(err!=ERR_OK)	//从客户端接收到一个非空数据,但是由于某种原因err!=ERR_OK
	{
		if(p)
			pbuf_free(p);	//释放接收pbuf
		ret_err=err;
		ZKRT_LOG(LOG_NOTICE, "err flag: %d\n", err);
	}
	else if(es->server_state==ES_TCPSERVER_ACCEPTED) 	//处于连接状态
	{
		if(p!=NULL)  //当处于连接状态并且接收到的数据不为空时将其打印出来
		{
			memset(es->recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //数据接收缓冲区清零
			for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
			{
				//判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
				//的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
					memcpy(es->recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//拷贝数据
				else 
					memcpy(es->recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) 
					break; //超出TCP客户端接收数组,跳出	
			}
			es->recv_datalen = data_len; //add by yanly
//			//test by yanly
//			tcp_write(es_global->pcb,tcp_server_recvbuf,strlen((char*)tcp_server_recvbuf),1); 
//			tcp_output(es_global->pcb);   //将发送缓冲队列中的数据发送出去
			
			es->recv_state =1;	//标记接收到数据了
			es->remote_ip = tpcb->remote_ip;
			ZKRT_LOG(LOG_NOTICE, "recv ip =%d.%d.%d.%d\n", (int)(tpcb->remote_ip.addr&0xff), (int)(tpcb->remote_ip.addr>>8)&0xff, (int)((tpcb->remote_ip.addr>>16)&0xff), (int)((tpcb->remote_ip.addr>>24)&0xff));
//			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
//			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
//			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
//			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
 			tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
			pbuf_free(p);  	//释放内存
			ret_err=ERR_OK;
		}
	}
	else	//服务器关闭了
	{
		tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
		es->send_p=NULL;
		pbuf_free(p);	//释放内存
		ret_err=ERR_OK;
	}
	return ret_err;
}
//lwIP tcp_err函数的回调函数
void tcp_server_error(void *arg,err_t err) //调用此回调时，相应的pcb已经被释放掉了
{  
	LWIP_UNUSED_ARG(err);  
	ZKRT_LOG(LOG_NOTICE, "tcp error:%d\r\n",err);
//	if(arg!=NULL)
//		mem_free(arg);//释放内存
} 
//lwIP tcp_poll的回调函数
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
	if(es!=NULL)
	{
		if(tcp_server_flag&(1<<7))	//判断是否有数据要发送
		{
			es->send_p=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_server_sendbuf),PBUF_POOL);//申请内存
			pbuf_take(es->send_p,(char*)tcp_server_sendbuf,strlen((char*)tcp_server_sendbuf));
			tcp_server_senddata(tpcb,es); 		//轮询的时候发送要发送的数据 //zkrt__todo: 发送成功才改变标记
			tcp_server_flag&=~(1<<7);  			//清除数据发送标志位
			if(es->send_p!=NULL)
				pbuf_free(es->send_p); 	//释放内存	
		}
		else if(es->recv_state==ES_TCPSERVER_CLOSING)//需要关闭连接?执行关闭操作
		{
			tcp_server_connection_close(tpcb,es);//关闭连接
		}
		ret_err=ERR_OK;
	}
	else
	{
		tcp_abort(tpcb);//终止连接,删除pcb控制块
		ret_err=ERR_ABRT; 
	}
	return ret_err;
} 
//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->send_p)
		tcp_server_senddata(tpcb,es);//发送数据
	return ERR_OK;
} 
//err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_server_struct *es;
//	LWIP_UNUSED_ARG(len); 
//	es = (struct tcp_server_struct *) arg;
//	if(es->p)tcp_server_senddata(tpcb,es);//发送数据
//	return ERR_OK;
//} 
//此函数用来发送数据
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	struct pbuf *ptr;
	u16 plen;
	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->send_p&&(es->send_p->len<=tcp_sndbuf(tpcb)))
	{
		ptr=es->send_p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1); //将要发送的数据加入发送缓冲队列中
		if(wr_err==ERR_OK)
		{ 
			plen=ptr->len;
			es->send_p=ptr->next;			//指向下一个pbuf
			if(es->send_p)
				pbuf_ref(es->send_p);	//pbuf的ref加一
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//更新tcp窗口大小
		}
		else if(wr_err==ERR_MEM)
			es->send_p=ptr;
		tcp_output(tpcb);   //将发送缓冲队列中的数据发送出去
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
//		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1); //将要发送的数据加入发送缓冲队列中
//		if(wr_err==ERR_OK)
//		{ 
//			plen=ptr->len;
//			es->p=ptr->next;			//指向下一个pbuf
//			if(es->p)
//				pbuf_ref(es->p);	//pbuf的ref加一
//			pbuf_free(ptr);
//			tcp_recved(tpcb,plen); 		//更新tcp窗口大小
//		}
//		else if(wr_err==ERR_MEM)
//			es->p=ptr;
//		tcp_output(tpcb);   //将发送缓冲队列中的数据发送出去
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
	es->send_p=pbuf_alloc(PBUF_TRANSPORT, buf_len,PBUF_RAM);	//申请内存 
	pbuf_take(es->send_p, sendbuf, buf_len);	//将buf的数据拷贝到es->p_tx中
	tcp_server_senddata(tpcb,user_arg);//将buf[]里面复制给pbuf的数据发送出去
//	if((ret ==ERR_OK)&&(es->send_p))
//	{
//		pbuf_free(es->send_p);	//释放内存	
//		es->send_p = NULL;
//		return ret;
//	}	
	return ret;
}
//关闭tcp连接
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
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//在 tcp.c里面 
extern struct tcp_pcb *tcp_active_pcbs;			//在 tcp.c里面 
extern struct tcp_pcb *tcp_tw_pcbs;				//在 tcp.c里面  
//强制删除TCP Server主动断开时的time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	while(tcp_active_pcbs!=NULL)
	{
		lwip_periodic_handle();//继续轮询
		delay_ms(10);//等待tcp_active_pcbs为空  
	}
	pcb=tcp_tw_pcbs;
	while(pcb!=NULL)//如果有等待状态的pcbs
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
  * @brief  tcp_server_hwt_led_control  硬件测试的系统灯控制
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

