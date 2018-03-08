#ifndef PELCOD_H
#define PELCOD_H

/*globally used functions */
void pelcod_init(void);
void pelcod_prcs(void);

/* Pelcod standard protocol */
int pelcod_move_right(unsigned char address, unsigned short speed);
int pelcod_move_left(unsigned char address, unsigned short speed);
int pelcod_move_up(unsigned char address, unsigned short speed);
int pelcod_move_down(unsigned char address, unsigned short speed);

int pelcod_move_stop(unsigned char address);

/* Pelcod extended protocol */
int pelcod_goto_preset(unsigned char address, unsigned short preset);
int pelcod_set_preset(unsigned char address, unsigned short preset);
int pelcod_clear_preset(unsigned char address, unsigned short preset);
#endif // PELCOD_H
