#ifndef __SYS_H
#define __SYS_H	 

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "port.h"
#include "commonzkrt.h"

/* Exported macro --------------------------------------------------------*/		
#define _Send_Normal  1
//#define _Send_test    2
							
/* Exported constants --------------------------------------------------------*/							
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#define MAVLINK_TX_INIT_VAL 0XFFFFFFFF 	

#if defined _TEMPTURE_IO_
#define TEMPTURE_LOW_EXTRA         -550
#define TEMPTURE_HIGH_EXTRA        1250
#define TEMPTURE_DIFF              8
#elif defined _TEMPTURE_ADC_
#define TEMPTURE_CALIBRATE         100                        //校准10度
#define TEMPTURE_LOW_EXTRA         -450+TEMPTURE_CALIBRATE    //异常值下限，-45度
#define TEMPTURE_HIGH_EXTRA        5000+TEMPTURE_CALIBRATE    //异常值上限，500度
#else
#define TEMPTURE_LOW_EXTRA         -400
#define TEMPTURE_HIGH_EXTRA        5000
#endif

#define GLO_TEMPTURE_LOW_INIT      -450         //温度上下限初始值
#define GLO_TEMPTURE_HIGH_INIT     4800

/* definition of return value */
#define	RETURN_FAILURE             0
#define RETURN_SUCCESS_RES         1
#define RETURN_SUCCESS_NONE        2

//! This is the default status printing mechanism
#define ZKRT_LOG(title, fmt, ...)                                  				\
  if ((title))                                                            \
  {                                                                       \
    int len = (sprintf(zkrt_log_buf, "%s %s,line %d: " fmt,    						\
        (title) ? (title) : "NONE", __func__, __LINE__, ##__VA_ARGS__));  \
    if ((len != -1) && (len < sizeof(zkrt_log_buf)))                                      	\
      printf("%s", zkrt_log_buf);                                         \
    else                                                                  \
      printf("ERROR: log printer inner fault\n");           							\
  }
#define LOG_PRINTF_UCHAR	//开启打印16进制数
#define LOG_NOTICE_IDATA  //重要提示，在产品代码考虑打印的log数据		
//#define LOG_NOTICE_DATA   //普通提示	 //zkrt_need_mask
#define LOG_ERROR_DATA
//#define LOG_DEBUG_DATA //zkrt_need_mask //zkrt_debug
//#define LOG_WARNING_DATA //zkrt_need_mask	
#ifdef LOG_DEBUG_DATA
#define LOG_DEBUG "DEBUG"
#else
#define LOG_DEBUG 0
#endif

#ifdef LOG_ERROR_DATA
#define LOG_ERROR "ERROR"
#else
#define LOG_ERROR 0
#endif

#ifdef LOG_WARNING_DATA
#define LOG_WARNING "WARNING"
#else
#define LOG_WARNING 0
#endif

#ifdef LOG_NOTICE_DATA
#define LOG_NOTICE "NOTICE"
#else
#define LOG_NOTICE 0
#endif

#ifdef LOG_NOTICE_IDATA
#define LOG_INOTICE "IMPORT_NOTICE"
#else
#define LOG_INOTICE 0
#endif

/* Exported variables ---------------------------------------------------------*/
extern volatile uint32_t TimingDelay;
extern volatile uint32_t usart1_tx_flag;
extern volatile uint32_t usart1_rx_flag;
extern volatile uint32_t u433m_tx_flag;		
extern volatile uint32_t u433m_rx_flag;
extern volatile uint32_t can_tx_flag;
extern volatile uint32_t can_rx_flag;
extern volatile uint32_t mavlink_send_flag;
extern volatile uint32_t _160_read_flag;
extern volatile uint32_t posion_recv_flag;
extern volatile uint32_t throw_recv_flag;
extern volatile uint32_t camera_recv_flag;
extern volatile uint32_t irradiate_recv_flag;
extern volatile uint32_t phone_recv_flag;
extern volatile uint32_t threemodeling_recv_flag;
extern volatile uint32_t multicamera_recv_flag;
extern volatile uint32_t _read_count;
extern volatile uint32_t adc_start_count;
extern volatile uint32_t temperature_read_count;
extern volatile uint32_t temperature_ctrl_count;
extern volatile uint32_t tcp_client_init_timeout;
#ifdef CanSend2SubModule_TEST
extern volatile uint32_t can_send_debug;
#endif
extern NVIC_InitTypeDef NVIC_InitStructure;

extern short last_tempture0;
extern short last_tempture1;

extern uint8_t msg_smartbat_buffer[30];

extern char zkrt_log_buf[300];

/* Exported functions ------------------------------------------------------- */
void RCC_Configuration(void);
void delay_init(void);
void delay_us(uint16_t nus);					//最多传入999us
void delay_ms(uint16_t nms);					//最多传入65535ms
void NVICX_init(uint8_t pre_prrty, uint8_t sub_prrty);
void DMAX_init(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel, uint32_t peripheral_addr, uint32_t memory_addr,uint32_t direction, uint16_t buff_size, uint32_t peripheral_size, uint32_t memory_size, uint32_t dma_mode, uint32_t dma_prrty);

#endif
