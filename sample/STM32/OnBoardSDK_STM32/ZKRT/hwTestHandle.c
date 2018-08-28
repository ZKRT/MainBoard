/**
  ******************************************************************************
  * @file    hwTestHandle.c
  * @author  ZKRT
  * @version V0.0.1
  * @date    23-December-2016
  * @brief   hwTestHandle program body
  *          + (1) init --by yanly
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hwTestHandle.h"
#include "adc.h"
#include "usart.h"
#include "timer_zkrt.h"
#include "led.h"
#include "iic.h"
#include "flash.h"
#include "can.h"
#include "ds18b20.h"
#include "osqtmr.h"
#include "ostmr.h"
#include "sram.h"
#include "pwm.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "exfuns.h"
#include "usb_usr_process.h"
#include "tcp_server_demo.h"
#include "24cxx.h"

/* Private typedef -----------------------------------------------------------*/
/**
*   @brief  every hardware test funciton
  * @param1  void* source data
  * @param2  void* respond data
  * @param3  short* respond data length
  * @retval RETURN_SUCCESS_RES: need send respond msg, RETURN_SUCCESS_NONE: no need send respond msg, RETURN_FAILURE: data parse failed, no need send respond msg
  */
typedef char (*HW_TEST_FUN)(void*, void*, u16*);

/* Private define ------------------------------------------------------------*/

//can1 can2 std id
#define CAN1_TEST_STDID         0X01
#define CAN2_TEST_STDID         0X01

//test send buffer size
#define TEST_BUFFER_SIZE				200

/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

//every hardware test function
char connect_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char led_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char uart_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char can_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char pwm_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char ad_temp_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char ds18b20_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char iic_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char usb_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char sbus_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char flash_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);
char external_ram_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len);

//every uart sub hardware test
char uart3_test(void);
char uart1_test(void);
char uart6_test(void);
//every can sub test
char can1_test(void);
#ifdef USE_CAN2_FUN
char can2_test(void);
#endif
//pwm sub test
char pwm_output_test(void);
//iic eeprom sub test
char iic_24cxx_test(void);
//usb flash disk test
char usbfd_test(void);
//sbus sub test
char sbus_test(void);
//flash sub test
char flash_test(void);
//external sram sub test
char esram_test(void);
/* Private variables ---------------------------------------------------------*/
//
const HW_TEST_FUN hw_test_fun[] = {
	connect_hwtf,
	led_hwtf,
	uart_hwtf,
	can_hwtf,
	pwm_hwtf,
	ad_temp_hwtf,
	ds18b20_hwtf,
	iic_hwtf,
	usb_hwtf,
	sbus_hwtf,
	flash_hwtf,
	external_ram_hwtf
};
//send and recv test buffer
char test_send_buf[TEST_BUFFER_SIZE];
char test_recv_buf[TEST_BUFFER_SIZE];

//hwtest_1s_task timeout flag
volatile char pwm_timeout_flag = 0;
volatile char hwtest_timeout = 0;

//ack arrays
u8 hwtest_bufack[] = {0xEB, 0xEB, 0xEB, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFE};

//usb test file
FIL testfile_fp;

//extern
extern u8 AppState;

/* Private functions ---------------------------------------------------------*/

/**
*   @brief  hwtest_1s_task
  * @retval none
  */
static void hwtest_1s_task(void) {
	if (hwtest_timeout) {
		hwtest_timeout--;
	}
}
/**
*   @brief  hwtest_init
  * @retval none
  */
void hwtest_init(void) {
	t_ostmr_insertTask(hwtest_1s_task, 1000, OSTMR_PERIODIC);
}
/**
*   @brief  hwtest_data_handle: hardware protocol data handle function
  * @param  buf
  * @param  buflen
  * @param  resbuf
  * @param  resbuflen
  * @retval RETURN_SUCCESS_RES: need send respond msg, RETURN_SUCCESS_NONE: no need send respond msg, RETURN_FAILURE: data parse failed, no need send respond msg
  */
u8 hwtest_data_handle(u8 *buf, u16 buflen, u8 *resbuf, u16 *resbuflen) {
	char ret = RETURN_FAILURE;
	u16 datalen;
	u16 endcode_index;
	u16 respond_datalen = 1;
	u8 cmdnum;

	//check start code
	if ((buf[0] | buf[1] << 8 | buf[2] << 16) != STARTCODE_HWT) {
		ZKRT_LOG(LOG_ERROR, "start code\n");
		return ret;
	}

	//check datalen
	datalen = buf[5] | buf[6] << 8;
	if (datalen + 7 + 7 != buflen) {
		ZKRT_LOG(LOG_ERROR, "data length\n");
		return ret;
	}

	//check end code
	endcode_index = datalen + 7 + 4;
	if ((buf[endcode_index] | buf[endcode_index + 1] << 8 | buf[endcode_index + 2] << 16) != ENDCODE_HWT) {
		ZKRT_LOG(LOG_ERROR, "end code\n");
		return ret;
	}

	//check cmd type
	if (buf[4] != SET_CMDT_HWT) {
		ZKRT_LOG(LOG_ERROR, "cmd type\n");
		return ret;
	}

	//check cmd number
	if (buf[3] > MAX_CMDN_HWT) {
		ZKRT_LOG(LOG_ERROR, "cmd number\n");
		return ret;
	}
	cmdnum = buf[3];
	//send ack
	hwtest_bufack[3] = cmdnum;
	//run led set
	_RUN_LED = 0;
	tcp_server_send(tcpserver_sp->pcb, tcpserver_sp, hwtest_bufack, sizeof(hwtest_bufack));
	//handle test fun
	ret = hw_test_fun[buf[3]](buf + 7, resbuf + 7, &respond_datalen); //para1: packet data, para2: respond packet data, para3: respond packet data length
	//fixed encode
	resbuf[0] = 0xeb;
	resbuf[1] = 0xeb;
	resbuf[2] = 0xeb;
	resbuf[3] = cmdnum;
	resbuf[4] = RESPOND_CMDT_HWT;
	memcpy(resbuf + 5, &respond_datalen, 2);
	memset(resbuf + 7 + respond_datalen, 0, 4);
	resbuf[7 + respond_datalen + 4] = 0xfe;
	resbuf[7 + respond_datalen + 5] = 0xfe;
	resbuf[7 + respond_datalen + 6] = 0xfe;
	*resbuflen = 14 + respond_datalen;

	//led state reset
	_RUN_LED = 1;
	_ALARM_LED = 1;
	_HS_LED = 1;
	_FLIGHT_UART_TX_LED = 1;
	_FLIGHT_UART_RX_LED = 1;
	_OBSTACLE_AVOIDANCE_TX_LED = 1;
	_OBSTACLE_AVOIDANCE_RX_LED = 1;
	_CAN_RX_LED = 1;
	_CAN_TX_LED = 1;

	return ret;
}

/** @fungroup  every hardware test funciton
  * @{
  */
//通讯测试，返回设备名称
char connect_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "connect_hwtf\n");
	outdata[0] = RESULT_OK_HWT;
	memcpy(outdata + 1, MAINBOARD_NAME, sizeof(MAINBOARD_NAME));
	*out_arg_len = 0x11;
	return RETURN_SUCCESS_RES;
}
//led test fun
char led_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
//	u8 *indata = (u8 *)in_arg;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "led test\n");

	//doing test
	led_init();
	led_test();
	//
	outdata[0] = RESULT_UNCONFIRM_HWT;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//uart test fun
char uart_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	int i;
	char ret;
	char data1 = 0;
//	u8 *indata = (u8 *)in_arg;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "uart test\n");

	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_send_buf[i] = i + 1;

	//doing test
	uart_init();

	//uart3 test //test com
	ret = uart3_test();
	ZKRT_LOG(LOG_NOTICE, "uart3_test, ret = 0x%x\n", ret);
	if (ret != RESULT_OK_HWT)
		data1 = data1 | (1 << 1);

	//uart1 test //flight controller com
	ret = uart1_test();
	ZKRT_LOG(LOG_NOTICE, "uart1_test, ret = 0x%x\n", ret);
	if (ret != RESULT_OK_HWT)
		data1 = data1 | (1 << 0);

	//uart6 test //433M com
	ret = uart6_test();
	ZKRT_LOG(LOG_NOTICE, "uart6_test, ret = 0x%x\n", ret);
	if (ret != RESULT_OK_HWT)
		data1 = data1 | (1 << 2);

	if (data1)
		outdata[0] = RESULT_FAIL_HWT;
	else
		outdata[0] = RESULT_OK_HWT;
	outdata[1] = data1;
	*out_arg_len = 2;
	return RETURN_SUCCESS_RES;
}
//can test fun
char can_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	int i;
	char ret;
	char data1 = 0;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "can test\n");

	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_send_buf[i] = i + 1;

	///doing test
	can_all_init();

	//can1 test
	ret = can1_test();
	ZKRT_LOG(LOG_NOTICE, "can1_test, ret = 0x%x\n", ret);
	if (ret != RESULT_OK_HWT)
		data1 = data1 | (1 << 0);
#ifdef USE_CAN2_FUN
	//can2 test
	ret = can2_test();
	ZKRT_LOG(LOG_NOTICE, "can2_test, ret = 0x%x\n", ret);
	if (ret != RESULT_OK_HWT)
		data1 = data1 | (1 << 1);
#endif
	///

	if (data1)
		outdata[0] = RESULT_FAIL_HWT;
	else
		outdata[0] = RESULT_OK_HWT;
	outdata[1] = data1;

	*out_arg_len = 2;
	return RETURN_SUCCESS_RES;
}
//pwm test fun
char pwm_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "pwm_hwtf\n");

	///doing test
	pwm_tim_init();
	//pwm test
	pwm_output_test();

	outdata[0] = RESULT_UNCONFIRM_HWT;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//ad test fun
char ad_temp_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	int i, j;
	char uploadcnt;
	u16 t1, t2;
	u8 *indata = (u8 *)in_arg;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "ad_temp_hwtf\n");

	uploadcnt = indata[1];
	test_send_buf[0] = 0xeb;
	test_send_buf[1] = 0xeb;
	test_send_buf[2] = 0xeb;
	test_send_buf[3] = AD_CMDN_HWT;
	test_send_buf[4] = RESPOND_CMDT_HWT;
	test_send_buf[5] = 5;
	test_send_buf[6] = 0;
	test_send_buf[7] = RESULT_OK_HWT;
	memset(test_send_buf + 12, 0, 4);
	test_send_buf[12 + 4] = 0xfe;
	test_send_buf[12 + 5] = 0xfe;
	test_send_buf[12 + 6] = 0xfe;
	///doing test
	ADC1_Init();
	//ad temp test
	ostmr_wait(5); //delay x*100 ms

	for (i = 0; i < uploadcnt; i++) {
		for (j = 0; j < 10; j++) {
			ostmr_wait(1);
			ADC_SoftwareStartConv(ADC1);
		}
		t1 = ADC1_get_value(_T1_value);
		t2 = ADC1_get_value(_T2_value);
		ZKRT_LOG(LOG_NOTICE, "t1:%d\n", t1);
		ZKRT_LOG(LOG_NOTICE, "t2:%d\n", t2);
		//upload
		test_send_buf[8] = t1 & 0xff;
		test_send_buf[9] = (t1 >> 8) & 0xff;
		test_send_buf[10] = t2 & 0xff;
		test_send_buf[11] = (t2 >> 8) & 0xff;
		tcp_server_send(tcpserver_sp->pcb, tcpserver_sp, (u8 *)test_send_buf, 19);
	}

	outdata[0] = RESULT_UNCONFIRM_HWT;
	*out_arg_len = 1;
	return RETURN_SUCCESS_NONE;
}
//ds18b20 test fun
char ds18b20_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	int i;
//	char ret;
	u16 t1, t2;
	char uploadcnt;
	u8 *outdata = (u8 *)out_arg;
	u8 *indata = (u8 *)in_arg;
	ZKRT_LOG(LOG_NOTICE, "ds18b20_hwtf\n");

	uploadcnt = indata[1];
	test_send_buf[0] = 0xeb;
	test_send_buf[1] = 0xeb;
	test_send_buf[2] = 0xeb;
	test_send_buf[3] = DS18B20_CMDN_HWT;
	test_send_buf[4] = RESPOND_CMDT_HWT;
	test_send_buf[5] = 5;
	test_send_buf[6] = 0;
	test_send_buf[7] = RESULT_OK_HWT;
	memset(test_send_buf + 12, 0, 4);
	test_send_buf[12 + 4] = 0xfe;
	test_send_buf[12 + 5] = 0xfe;
	test_send_buf[12 + 6] = 0xfe;
	///doing test
	DS18B20_Init();
	//temp test
	ostmr_wait(10); //delay x*100 ms
	for (i = 0; i < uploadcnt; i++) {
		ostmr_wait(10);
		t1 = DS18B20_Get_Temp(DS18B20_NUM1);
		t2 = DS18B20_Get_Temp(DS18B20_NUM2);
		ZKRT_LOG(LOG_NOTICE, "t1:%d\n", t1);
		ZKRT_LOG(LOG_NOTICE, "t2:%d\n", t2);
		//upload
		test_send_buf[8] = t1 & 0xff;
		test_send_buf[9] = (t1 >> 8) & 0xff;
		test_send_buf[10] = t2 & 0xff;
		test_send_buf[11] = (t2 >> 8) & 0xff;
		tcp_server_send(tcpserver_sp->pcb, tcpserver_sp, (u8 *)test_send_buf, 19);
	}

	outdata[0] = RESULT_UNCONFIRM_HWT;
	*out_arg_len = 1;
	return RETURN_SUCCESS_NONE;
}
//iic test fun
char iic_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	char ret;

	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "iic_hwtf\n");
	///doing test
	IIC_Init();
	//iic test
	ret = iic_24cxx_test();
	ZKRT_LOG(LOG_NOTICE, "iic 24cxx test, ret = 0x%x\n", ret);

	outdata[0] = ret;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//usb test fun
char usb_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	char ret;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "usb_hwtf\n");
	hwtest_timeout = 30; //30s timeout
	///doing test
//	f_mount(fs[2],"2:",1); 	//挂载U盘
//  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  //初始化USB主机
	//usb test
	while (1) {
		usb_user_prcs();
		if (AppState == USH_USR_FS_TEST) {
			ret = usbfd_test();
			ZKRT_LOG(LOG_NOTICE, "usbfd_test, ret = %d\n", ret);
			break;
		}
		if (!hwtest_timeout) {
			ZKRT_LOG(LOG_NOTICE, "usb test timeout!\n");
			ret = RESULT_FAIL_HWT;
			break;
		}
	}

	outdata[0] = ret;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//sbus test fun
char sbus_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
//	int i;
	char ret;

	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "sbus_hwtf\n");

	///doing test
	uart_init();
	//sbus test
	ret = sbus_test();
	ZKRT_LOG(LOG_NOTICE, "sbus_test, ret = 0x%x\n", ret);

	outdata[0] = ret;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//flash test fun
char flash_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	char ret;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "flash_hwtf\n");
	///doing test
	//flash test
	ret = flash_test();
	ZKRT_LOG(LOG_NOTICE, "flash_test, ret = 0x%x\n", ret);
	outdata[0] = ret;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
//external ram test fun
char external_ram_hwtf(void *in_arg, void *out_arg, u16 *out_arg_len) {
	char ret;
	u8 *outdata = (u8 *)out_arg;
	ZKRT_LOG(LOG_NOTICE, "external_ram_hwtf\n");

	///doing test
	FSMC_SRAM_Init();
	//esram test
	ret = esram_test();
	ZKRT_LOG(LOG_NOTICE, "esram_test, ret = 0x%x\n", ret);
	outdata[0] = ret;
	*out_arg_len = 1;
	return RETURN_SUCCESS_RES;
}
/**
  * @}
  */
/** @fungroup  every uart test funciton
  * @{
  */
char uart3_test(void) { //test uart
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_DEBUG, "uart3_test\n");
	//send msg
	usart3_tx_copyed(test_send_buf, TEST_BUFFER_SIZE);
	usart3_tx_DMA();
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(30); //delay x*100 ms //modify by yanly
	recv_pos = 0;
	while (usart3_rx_check() == 1) {
		value = usart3_rx_byte();
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}
	return ret;
}
char uart1_test(void) { //flight uart
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_DEBUG, "uart1_test\n");
	//send msg
	usart1_tx_copyed(test_send_buf, TEST_BUFFER_SIZE);
	usart1_tx_DMA();
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(10); //delay x*100 ms
	recv_pos = 0;
	while (usart1_rx_check() == 1) {
		value = usart1_rx_byte();
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}
	return ret;
}
char uart6_test(void) { //433M uart
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_DEBUG, "uart6_test\n");
	//send msg
	usart6_tx_copyed(test_send_buf, TEST_BUFFER_SIZE);
	usart6_tx_DMA();
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(10); //delay x*100 ms
	recv_pos = 0;
	while (usart6_rx_check() == 1) {
		value = usart6_rx_byte();
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}
	return ret;
}
/**
  * @}
  */

/** @fungroup  every can test funciton
  * @{
  */
char can1_test(void) {
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_DEBUG, "can1_test\n");
	//send msg
	CAN1_send_message_fun((u8*)test_send_buf, TEST_BUFFER_SIZE, CAN1_TEST_STDID);
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(10); //delay x*100 ms
	recv_pos = 0;

	while (CAN1_rx_check(CAN1_TEST_STDID) == 1) {
		value = CAN1_rx_byte(CAN1_TEST_STDID);
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}

	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}

	return ret;
}
#ifdef USE_CAN2_FUN
char can2_test(void) {
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_DEBUG, "can2_test\n");
	//send msg
	CAN2_send_message_fun((u8*)test_send_buf, TEST_BUFFER_SIZE, CAN2_TEST_STDID);
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(10); //delay x*100 ms
	recv_pos = 0;

	while (CAN2_rx_check(CAN2_TEST_STDID) == 1) {
		value = CAN2_rx_byte(CAN2_TEST_STDID);
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}

	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}

	return ret;
}
#endif
/**
  * @}
  */

/**
  * @brief  PWM output test
  * @param  None
* @retval ret: test result
  */
char pwm_output_test(void) {
	short pwmval = 1100;
	char change = 1; //0-递增，1-递减
	ostmr_wait(1);
//	PWM_OUT(pwmval);
	PWM1_OUT(pwmval);
	PWM2_OUT(pwmval);
	PWM3_OUT(pwmval);
	PWM4_OUT(pwmval);
	hwtest_timeout = 30; //30s timeout
	while (1) { //实现比较值从1100~1900递增，到1900后从1900~1100递减，循环
		delay_ms(1);
		if (change)
			pwmval += 1;
		else
			pwmval -= 1;
		if (pwmval > 1900)
			change = 0; //pwmval到达后，方向为递减
		if (pwmval < 1100)
			change = 1;	//pwmval递减到0后，方向改为递增
//		PWM_OUT(pwmval);
		PWM1_OUT(pwmval);
		PWM2_OUT(pwmval);
		PWM3_OUT(pwmval);
		PWM4_OUT(pwmval);
		if (!hwtest_timeout) {
			ZKRT_LOG(LOG_NOTICE, "pwm timeout\n");
			break;
		}
	}
	return 0;
}
/**
  * @brief  sbus_test: sbus数据发送接收测试
  * @param  None
  * @retval ret: test result, RESULT_OK_HWT, RESULT_FAIL_HWT, RESULT_UNCONFIRM_HWT
  */

char sbus_test(void) {
	int i;
	int recv_pos;
	char value;
	char ret;
	ZKRT_LOG(LOG_NOTICE, "sbus_test\n");
	//send msg
	uart4_tx_copyed(test_send_buf, TEST_BUFFER_SIZE);
	uart4_tx_DMA();
	//recv msg
	for (i = 0; i < TEST_BUFFER_SIZE; i++)
		test_recv_buf[i] = 0xff;
	ostmr_wait(10); //delay x*100 ms
	recv_pos = 0;
	while (uart4_rx_check() == 1) {
		value = uart4_rx_byte();
//		printf("%x ", value);
		test_recv_buf[recv_pos] = value;
		recv_pos++;
		if (recv_pos >= TEST_BUFFER_SIZE)
			break;
	}
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}
	return ret;
}
/**
  * @brief  iic 24cxx test: iic eeprom test, check test, write test, read test
  * @param  None
  * @retval ret: test result, RESULT_OK_HWT, RESULT_FAIL_HWT, RESULT_UNCONFIRM_HWT
  */
char iic_24cxx_test(void) {
	char ret;
	int i;
	hwtest_timeout = 30; //30s
	//check connect
	while (AT24CXX_Check()) { //检测不到24c02
		ZKRT_LOG(LOG_ERROR, "24C02 Check Failed!\n");
		ostmr_wait(10); //wait 1s
		if (!hwtest_timeout) {
			ZKRT_LOG(LOG_NOTICE, "iic test timeout!\n");
			ret = RESULT_FAIL_HWT;
			return ret;
		}
	}
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		test_send_buf[i] = i;
		test_recv_buf[i] = 0;
	}
	//write
	AT24CXX_Write(0, (u8*)test_send_buf, TEST_BUFFER_SIZE);
	//read
	AT24CXX_Read(0, (u8*)test_recv_buf, TEST_BUFFER_SIZE);
	//check
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] == test_send_buf[i]) {
			ret = RESULT_OK_HWT;
		} else {
			ret = RESULT_FAIL_HWT;
			break;
		}
	}
	return ret;
}
/**
  * @brief  usbfd_test: 读写文件测试
  * @param  None
  * @retval ret: test result, RESULT_OK_HWT, RESULT_FAIL_HWT, RESULT_UNCONFIRM_HWT
  */
char usbfd_test(void) {
	u32 i;
	u8 res;
	u32 _bw;
	char ret = RESULT_FAIL_HWT;
	//open
	res = f_open(&testfile_fp, "2:zkrt_usbfd_test_file", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	if (res != FR_OK) {
		ZKRT_LOG(LOG_ERROR, "f_open failed!\n");
		f_close(&testfile_fp);
		return ret;
	}
	//write
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		test_send_buf[i] = i;
	}
	res = f_write(&testfile_fp, test_send_buf, TEST_BUFFER_SIZE, &_bw);
	if (res != FR_OK) {
		ZKRT_LOG(LOG_ERROR, "f_write failed!\n");
		f_close(&testfile_fp);
		return ret;
	}
	f_sync(&testfile_fp);
	//read
	memset(test_recv_buf, 0, TEST_BUFFER_SIZE);
	f_lseek(&testfile_fp, 0);
	res = f_read(&testfile_fp, test_recv_buf, TEST_BUFFER_SIZE, &_bw);
	if (res != FR_OK) {
		ZKRT_LOG(LOG_ERROR, "f_read failed!\n");
		f_close(&testfile_fp);
		return ret;
	}
	//check
	for (i = 0; i < TEST_BUFFER_SIZE; i++) {
		if (test_recv_buf[i] != test_send_buf[i]) {
			ZKRT_LOG(LOG_ERROR, "check failed!\n");
			f_close(&testfile_fp);
			return ret;
		}
	}
	f_close(&testfile_fp);
	ret = RESULT_OK_HWT;
	return ret;
}
/**
  * @brief  flash_test: 对配置存储区域-扇区7进行擦除、读写检测
  * @param  None
  * @retval ret: test result, RESULT_OK_HWT, RESULT_FAIL_HWT, RESULT_UNCONFIRM_HWT
  */
char flash_test(void) {
	char ret = RESULT_FAIL_HWT;
	uint32_t i;
	uint32_t handle_addr = ADDR_FLASH_SECTOR_7;
	uint32_t addr_value;
	//erase
	STMFLASH_Erase(FLASH_Sector_7);
	//check erase result
	for (i = 0; i < SIZE_FLASH_SECTOR_7 / 4; i++) {
		addr_value = STMFLASH_ReadWord(handle_addr);
		//sub check
		if (addr_value != 0xffffffff) { //erase result failed
			ret = RESULT_FAIL_HWT;
			ZKRT_LOG(LOG_ERROR, "erase check failed\n");
			return ret;
		}
		handle_addr += 4;
		addr_value = 0;
	}

	//write
	handle_addr = ADDR_FLASH_SECTOR_7;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);
	for (i = 0; i < SIZE_FLASH_SECTOR_7 / 4; i++) {
		if (FLASH_ProgramWord(handle_addr,  0xAA55AA55) != FLASH_COMPLETE) {
			ret = RESULT_FAIL_HWT;
			FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
			FLASH_Lock();//上锁
			ZKRT_LOG(LOG_ERROR, "write failed\n");
			return ret;
		}
		handle_addr += 4;
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
	//check write result
	handle_addr = ADDR_FLASH_SECTOR_7;
	for (i = 0; i < SIZE_FLASH_SECTOR_7 / 4; i++) {
		addr_value = STMFLASH_ReadWord(handle_addr);
		//sub check
		if (addr_value != 0xAA55AA55) {
			ret = RESULT_FAIL_HWT;
			ZKRT_LOG(LOG_ERROR, "write check failed\n");
			return ret;
		}
		handle_addr += 4;
		addr_value = 0;
	}
	ret = RESULT_OK_HWT;
	return ret;
}
/**
  * @brief  esram_test: 对外部sram进行读写检测
  *
  * @param  None
  * @retval ret: test result, RESULT_OK_HWT, RESULT_FAIL_HWT, RESULT_UNCONFIRM_HWT
  */
char esram_test(void) {
	char ret = RESULT_FAIL_HWT;
	uint32_t i;
	u8 u8value;
	uint32_t handle_addr = 0; //check_addr = 0;
	uint32_t addr_value = 0;
//	u8 write_value, read_value;
//  u8 value_00 = 0;
	u8 value_arry[4];

//	//reset all byte to value_00
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE/4; i++)
//	{
//		FSMC_SRAM_WriteBuffer((u8*)&value_00, handle_addr, 4);
//		handle_addr += 4;
//	}
//
//	///write value 0x55 and check [start]
//	write_value = 0x55;
//	for(j=0; j<IS62WV51216_SIZE; j++)
//	{
//		FSMC_SRAM_WriteBuffer((u8*)&write_value, j, 1); //write one byte
//		for(i=j; i<IS62WV51216_SIZE; i++) //check all bytes
//		{
//			FSMC_SRAM_ReadBuffer((u8*)&read_value, i, 1);
//			if(i == j)
//			{
//				if(read_value != write_value)
//				{
//					ret = RESULT_FAIL_HWT;
//					ZKRT_LOG(LOG_ERROR, "check failed\n");
//					return ret;
//				}
//			}
//			else
//			{
//				if(read_value != 0)
//				{
//					ret = RESULT_FAIL_HWT;
//					ZKRT_LOG(LOG_ERROR, "check failed\n");
//					return ret;
//				}
//			}
//		}
////		FSMC_SRAM_WriteBuffer((u8*)&value_00, j, 1); 		//reset one byte
//	}
//	///write value 0x55 and check [end]
//
//	///write value 0xaa and check [start]
//	write_value = 0xaa;
//	for(j=0; j<IS62WV51216_SIZE; j++)
//	{
//		FSMC_SRAM_WriteBuffer((u8*)&write_value, j, 1); //write one byte
//		for(i=j; i<IS62WV51216_SIZE; i++) //check all bytes
//		{
//			FSMC_SRAM_ReadBuffer((u8*)&read_value, i, 1);
//			if(i == j)
//			{
//				if(read_value != write_value)
//				{
//					ret = RESULT_FAIL_HWT;
//					ZKRT_LOG(LOG_ERROR, "check failed\n");
//					return ret;
//				}
//			}
//			else
//			{
//				if(read_value != 0)
//				{
//					ret = RESULT_FAIL_HWT;
//					ZKRT_LOG(LOG_ERROR, "check failed\n");
//					return ret;
//				}
//			}
//		}
////		FSMC_SRAM_WriteBuffer((u8*)&value_00, j, 1); 		//reset one byte
//	}
//	////write value 0xaa and check [end]

//	//write 0x55 and check
//	write_value = 0x55;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE; i++)
//	{
//		read_value = 0;
//		FSMC_SRAM_WriteBuffer((u8*)&write_value, handle_addr, 1);
//		FSMC_SRAM_ReadBuffer((u8*)&read_value, handle_addr, 1);
//		if(read_value != write_value) //check failed
//		{
//			ret = RESULT_FAIL_HWT;
//			ZKRT_LOG(LOG_ERROR, "check failed\n");
//			return ret;
//		}
//		handle_addr += 1;
//	}
//	//write 0xaa and check
//	write_value = 0xaa;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE; i++)
//	{
//		read_value = 0;
//		FSMC_SRAM_WriteBuffer((u8*)&write_value, handle_addr, 1);
//		FSMC_SRAM_ReadBuffer((u8*)&read_value, handle_addr, 1);
//		if(read_value != write_value) //check failed
//		{
//			ret = RESULT_FAIL_HWT;
//			ZKRT_LOG(LOG_ERROR, "check failed\n");
//			return ret;
//		}
//		handle_addr += 1;
//	}

//	//write 0x55555555 and check
//	addr_value = 0x55555555;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE/4; i++)
//	{
//		FSMC_SRAM_WriteBuffer((u8*)&addr_value, handle_addr, 4);
//		handle_addr += 4;
//	}
//	addr_value = 0;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE/4; i++)
//	{
//		FSMC_SRAM_ReadBuffer((u8*)&addr_value, handle_addr, 4);
//		//sub check
//		if(addr_value != 0x55555555) //check failed
//		{
//			ret = RESULT_FAIL_HWT;
//			ZKRT_LOG(LOG_ERROR, "check failed\n");
//			return ret;
//		}
//		handle_addr += 4;
//		addr_value = 0;
//	}
//	//write 0xaaaaaaaa and check
//	addr_value = 0xaaaaaaaa;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE/4; i++)
//	{
//		FSMC_SRAM_WriteBuffer((u8*)&addr_value, handle_addr, 4);
//		handle_addr += 4;
//	}
//	addr_value = 0;
//	handle_addr = 0;
//	for(i = 0;i < IS62WV51216_SIZE/4; i++)
//	{
//		FSMC_SRAM_ReadBuffer((u8*)&addr_value, handle_addr, 4);
//		//sub check
//		if(addr_value != 0xaaaaaaaa) //check failed
//		{
//			ret = RESULT_FAIL_HWT;
//			ZKRT_LOG(LOG_ERROR, "check failed\n");
//			return ret;
//		}
//		handle_addr += 4;
//		addr_value = 0;
//	}

	//erase
	handle_addr = 0;
	addr_value = 0;
	for (i = 0; i < IS62WV51216_SIZE / 4; i++) {
		FSMC_SRAM_WriteBuffer((u8*)&addr_value, handle_addr, 4);
		handle_addr += 4;
	}
	addr_value = 0xffffffff;
	//check erase result
	for (i = 0; i < IS62WV51216_SIZE / 4; i++) {
		FSMC_SRAM_ReadBuffer((u8*)&addr_value, handle_addr, 4);
		//sub check
		if (addr_value != 0) { //erase result failed
			ret = RESULT_FAIL_HWT;
			ZKRT_LOG(LOG_ERROR, "erase check failed\n");
			return ret;
		}
		handle_addr += 4;
		addr_value = 0xffffffff;
	}

	//write
	handle_addr = 0;
	u8value = 0;
	for (i = 0; i < IS62WV51216_SIZE; i += 4) {
		value_arry[0] = u8value;
		value_arry[1] = u8value + 1;
		value_arry[2] = u8value + 2;
		value_arry[3] = u8value + 3;
		FSMC_SRAM_WriteBuffer((u8*)value_arry, handle_addr, 4);
		u8value += 4;
		handle_addr += 4;
	}
	//check write result
//	addr_value = 0;
	handle_addr = 0;
	u8value = 0;
	for (i = 0; i < IS62WV51216_SIZE; i += 4) {
		FSMC_SRAM_ReadBuffer((u8*)value_arry, handle_addr, 4);
		//sub check
		if ((value_arry[0] != u8value) || (value_arry[1] != u8value + 1) || (value_arry[2] != u8value + 2) || (value_arry[3] != u8value + 3)) {
			ret = RESULT_FAIL_HWT;
			ZKRT_LOG(LOG_ERROR, "write check failed\n");
			return ret;
		}
		u8value += 4;
		handle_addr += 4;
//		addr_value = 0;
	}

	ret = RESULT_OK_HWT;
	return ret;
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/
