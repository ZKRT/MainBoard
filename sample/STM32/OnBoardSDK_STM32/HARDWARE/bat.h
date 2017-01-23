#ifndef __BAT_H
#define __BAT_H

#include "sys.h"

uint16_t readtemperature(void);
uint16_t readvoltage(void);
uint16_t readcurrent(void);
uint16_t readpercent(void);
uint16_t readrelativeelectric(void);
uint16_t readelectric(void);
uint16_t readcyclecount(void);
uint16_t readdesigncapacity(void);
uint16_t reads1votlage(void);
uint16_t reads2votlage(void);
uint16_t reads3votlage(void);
uint16_t reads4votlage(void);
uint16_t reads5votlage(void);
uint16_t reads6votlage(void);
void bat_value_roll(void);
void bat_value_send(void);
void dji_bat_value_send(void);

#endif


