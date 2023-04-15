#ifndef _BORDER_H
#define _BORDER_H

#include "config.h"

#define BORDER_WIDTH                    64
#define BORDER_HEIGHT                   128
#define BORDER_X0                       4
#define BORDER_X1                       (SCREEN_WIDTH - BORDER_X0)
#define BORDER_Y0                       4
#define BORDER_Y1                       (SCREEN_HEIGHT - BORDER_Y0)

#define BORDER_LAYER                    14

void init_border(void);
void show_border(void);
void hide_border(void);

#endif

