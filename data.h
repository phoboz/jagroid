#ifndef _DATA_H
#define _DATA_H

#include <stdint.h>

#define MAP_NCOLS 16

extern uint16_t tiscavPal[MAP_NCOLS];
extern uint8_t tiscavTiles[];
extern level_t tiscavLevel01;
extern level_t tiscavLevel02;

extern phrase *player_frames[];

#endif

