#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "lan8720.h" 

#define LWIP_MAX_DHCP_TRIES		4   //DHCP服务器最大重试次数

#define LINK_UP					1
#define LINK_DOWN				0

enum net_status {
	NET_NONE = 0,
	NET_INITING,
	NET_INIT_FAILED,
	NET_INIT_OK
};

//lwip控制结构体
typedef struct  
{
	u8 mac[6];      //MAC地址
	u8 remoteip[4];	//远端主机IP地址 
	u8 ip[4];       //本机IP地址
	u8 netmask[4]; 	//子网掩码
	u8 gateway[4]; 	//默认网关的IP地址
	
	vu8 dhcpstatus;	//dhcp状态 
					//0,未获取DHCP地址;
					//1,进入DHCP获取状态
					//2,成功获取DHCP地址 或者静态时
					//0XFF,获取失败
	u8 netstatus; //网络状态，网络初始化状态
	u8 linkstatus; //网线连接状态
}__lwip_dev;
extern __lwip_dev lwipdev;	//lwip控制结构体

void lwip_pkt_handle(void);
void lwip_periodic_handle(void);
	
void lwip_comm_default_ip_set(__lwip_dev *lwipx);
u8 lwip_comm_mem_malloc(void);
void lwip_comm_mem_free(void);
u8 lwip_comm_init(void);
void lwip_dhcp_process_handle(void);

#endif
