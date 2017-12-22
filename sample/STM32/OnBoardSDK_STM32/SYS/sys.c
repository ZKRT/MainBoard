#include "sys.h"
#include "core_cm4.h"
#include "stm32f4xx_pwr.h"
/**********ϵͳʱ�����ú���******************************/
#define HSI_STARTUP_TIMEOUT HSE_STARTUP_TIMEOUT
void RCC_Configuration(void)
{
	uint32_t StartUpCounter = 0, HSIStatus = 0;
	
	RCC_DeInit();//RCC��λ
	RCC_HSICmd(ENABLE);//HSIʹ��
	
	while((RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) && (StartUpCounter != HSI_STARTUP_TIMEOUT))//�ȴ�HSIʹ�ܽ���
	{
		StartUpCounter++;
	}
	
	if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)//���ʹ�ܳɹ���
	{
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		HSIStatus = (uint32_t)0x00;
	}	
	
	if (HSIStatus == (uint32_t)0x01)
  {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_MainRegulatorModeConfig(PWR_Regulator_Voltage_Scale1);
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);  
		RCC_PCLK2Config(RCC_HCLK_Div2);  
		
		RCC_PLLConfig(RCC_PLLSource_HSI, 8, 168, 2, 7);
		
		RCC_PLLCmd(ENABLE);//ʹ��PLLʱ��

		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		
		FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		while (RCC_GetSYSCLKSource() != (uint8_t)RCC_CFGR_SWS_PLL)
		{
		}
  }
  else
  {
		//say something else!
  }  
}


///*************SYSTICK��ʼ��������us��ʱ����������ms��ʱ����******************/

void delay_init(void)
{
	while(SysTick_Config(168000000 / 1000) != 0);
}

void delay_us(uint16_t nus)					
{
	uint32_t start_num;								
	uint32_t temp;											
	uint32_t nus_pai;
	
	nus_pai = 168 * nus;
	start_num = SysTick->VAL;					
	do
	{
		temp=SysTick->VAL;								
		if (temp < start_num)						
		{
			temp = start_num - temp;				
		}
		else														
		{
			temp = start_num+168000-temp;		
		}
	}while (temp < nus_pai);      			
}

volatile uint32_t TimingDelay = 0XFFFFFFFF;				
volatile uint32_t usart1_tx_flag = 0XFFFFFFFF;		
volatile uint32_t usart1_rx_flag = 0XFFFFFFFF;
volatile uint32_t u433m_tx_flag = 0XFFFFFFFF;		
volatile uint32_t u433m_rx_flag = 0XFFFFFFFF;
volatile uint32_t can_tx_flag = 0XFFFFFFFF;				
volatile uint32_t can_rx_flag = 0XFFFFFFFF;				
volatile uint32_t mavlink_send_flag = 0XFFFFFFFF; 
volatile uint32_t _160_read_flag = 0XFFFFFFFF;		
volatile uint32_t posion_recv_flag = 0XFFFFFFFF;	
volatile uint32_t throw_recv_flag  = 0XFFFFFFFF;	
volatile uint32_t camera_recv_flag = 0XFFFFFFFF;
volatile uint32_t irradiate_recv_flag = 0XFFFFFFFF;
volatile uint32_t phone_recv_flag = 0XFFFFFFFF;
volatile uint32_t threemodeling_recv_flag = 0XFFFFFFFF;
volatile uint32_t multicamera_recv_flag = 0XFFFFFFFF;
volatile uint32_t _read_count = 0; 	
volatile uint32_t tcp_client_init_timeout;
volatile uint32_t adc_start_count = 0xffffffff;
volatile uint32_t temperature_read_count = 0xffffffff;
volatile uint32_t temperature_ctrl_count = 0xffffffff;
#ifdef CanSend2SubModule_TEST
volatile uint32_t can_send_debug  =0XFFFFFFFF;
#endif

void delay_ms(uint16_t nms)								//��ഫ��65535ms
{
	uint32_t start_num;
	
	start_num = TimingDelay;								
	while ((start_num - TimingDelay) < nms);
}

//���������¼�����ÿ�ζ�ʱ������Ϊ0��ʱ�򣬻ᴥ��һ���жϺ�����Ҫ1msʱ��
void SysTick_Handler(void)
{
	TimingDelay--;
}

/************************�ж����ú���*****************/
NVIC_InitTypeDef NVIC_InitStructure;

void NVICX_init(uint8_t pre_prrty, uint8_t sub_prrty)
{
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pre_prrty;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = sub_prrty;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//F4���е�DMA����
void DMAX_init(DMA_Stream_TypeDef *dma_stream, uint32_t dma_channel, uint32_t peripheral_addr, uint32_t memory_addr,uint32_t direction, 
	uint16_t buff_size, uint32_t peripheral_size, uint32_t memory_size, uint32_t dma_mode, uint32_t dma_prrty)
{
	DMA_InitTypeDef  DMA_InitStructure;
	
	DMA_DeInit(dma_stream);
	while (DMA_GetCmdStatus(dma_stream) != DISABLE)
	{
	}

  DMA_InitStructure.DMA_Channel = dma_channel;  											
  DMA_InitStructure.DMA_PeripheralBaseAddr = peripheral_addr;				
  DMA_InitStructure.DMA_Memory0BaseAddr = memory_addr;							
  DMA_InitStructure.DMA_DIR = direction;															
  DMA_InitStructure.DMA_BufferSize = buff_size;											
  DMA_InitStructure.DMA_PeripheralDataSize = peripheral_size;					
  DMA_InitStructure.DMA_MemoryDataSize = memory_size;									
  DMA_InitStructure.DMA_Mode = dma_mode;															
  DMA_InitStructure.DMA_Priority = dma_prrty;													
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;							       
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;				
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;					
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
  DMA_Init(dma_stream, &DMA_InitStructure);//��ʼ��DMA Stream
}


short glo_tempture_low;
short glo_tempture_high;

short last_tempture0 = 0;
short last_tempture1 = 0;

short tempture0 = TEMPTURE_HIGH_EXTRA+1;  //��ʼ��ʱ����Ч��ֵ
short tempture1 = TEMPTURE_HIGH_EXTRA+1;

uint8_t msg_smartbat_buffer[30] = {0};

/* Private variables ---------------------------------------------------------*/
char zkrt_log_buf[300]; //

