#include <inttypes.h>

#include "osusart.h"
#include "pelcod_defs.h"
#include "pelcod.h"
#include "ostmr.h"

/////////////////////////////////////////////////////////////////////////
#define PELCOD_MSG_SIZE		      7
#define PELCOD_DEFAULT_ADDRES   1
#define PELCOD_DEFAULT_SPEED    1
/////////////////////////////////////////////////////////////////////////
typedef scommReturn_t (*Funcptr_t)(uint8_t*, uint16_t, USART_TypeDef*);
static Funcptr_t send_data = &t_osscomm_sendMessage;
/////////////////////////////////////////////////////////////////////////
/**
  * @brief  
  * @param  None
  * @retval None
  */
void pelcod_init(void)
{
//low level is rs485, use usart6 in this program
/////////////////test
	ostmr_wait(10);
	pelcod_move_right(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);
	printf("right\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);
	printf("stop\n");
	ostmr_wait(10);
	pelcod_move_left(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);
	printf("left\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);
	printf("stop\n");
	ostmr_wait(10);	
	pelcod_move_up(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);
	printf("up\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);
	printf("stop\n");
	ostmr_wait(10);	
	pelcod_move_down(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);printf("down\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);printf("stop\n");
	ostmr_wait(10);
	pelcod_set_preset(PELCOD_DEFAULT_ADDRES, 1);printf("set_preset\n");
	ostmr_wait(10);
	pelcod_move_left(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);printf("left\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);printf("stop\n");
	ostmr_wait(10);	
	pelcod_goto_preset(PELCOD_DEFAULT_ADDRES, 1);printf("goto_preset\n");
	ostmr_wait(40);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);printf("stop\n");
	ostmr_wait(10);		
	pelcod_clear_preset(PELCOD_DEFAULT_ADDRES, 1);printf("clear_preset\n");
	ostmr_wait(10);
	pelcod_move_left(PELCOD_DEFAULT_ADDRES, PELCOD_DEFAULT_SPEED);printf("left\n");
	ostmr_wait(30);
	pelcod_move_stop(PELCOD_DEFAULT_ADDRES);printf("stop\n");
	ostmr_wait(10);		
	pelcod_goto_preset(PELCOD_DEFAULT_ADDRES, 1);printf("goto_preset\n");
  ostmr_wait(10);
/////////////////test	
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void pelcod_prcs(void)
{

}
// static
unsigned char calc_checksum(unsigned char* data, int size) {
	short i;
	unsigned int total = 0;
	for(i = 0; i < size; i++)
		total += data[i];

	return total;
}
// static 
int pelcod_send(unsigned char address, unsigned short command, unsigned short payload) {
	unsigned char buffer[PELCOD_MSG_SIZE];	

	buffer[0] = 0xFF;						// preamble
	buffer[1] = address;					// device address
	buffer[2] = (command & 0xFF00) >> 8;	// command 1
	buffer[3] = (command & 0xFF);			// command 2
	buffer[4] = (payload & 0xFF00) >> 8;	// data 1
	buffer[5] = (payload & 0xFF);			// data 2 
	buffer[6] = calc_checksum(buffer+1, PELCOD_MSG_SIZE - 2);

	send_data(buffer, PELCOD_MSG_SIZE, USART6);

	return(1);
}

/* Standard pelcod functions */
int pelcod_move_right(unsigned char address, unsigned short speed) {
  return pelcod_send(address, PELCOD_STD_RIGHT, (speed & 0xFF) << 8);
}

int pelcod_move_left(unsigned char address, unsigned short speed) {
  return pelcod_send(address, PELCOD_STD_LEFT, (speed & 0xFF) << 8);
}

int pelcod_move_up(unsigned char address, unsigned short speed) {
  return pelcod_send(address, PELCOD_STD_UP, speed);
}

int pelcod_move_down(unsigned char address, unsigned short speed) {
  return pelcod_send(address, PELCOD_STD_DOWN, speed);
}

int pelcod_move_stop(unsigned char address) {
	return pelcod_send(address, 0x00, 0x00);
}

/* extended pelcod functions */
int pelcod_goto_preset(unsigned char address, unsigned short preset) {
	return pelcod_send(address, PELCOD_EXT_PRE_TO, preset);
}

int pelcod_set_preset(unsigned char address, unsigned short preset) {
	return pelcod_send(address, PELCOD_EXT_PRE_SET, preset);
}

int pelcod_clear_preset(unsigned char address, unsigned short preset) {
	return pelcod_send(address, PELCOD_EXT_PRE_CLR, preset);
}

