#ifndef _BORDER_H
#define _BORDER_H

#include "config.h"

#define BORDER_WIDTH                    64
#define BORDER_HEIGHT                   128
#define BORDER_X0                       0
#define BORDER_X1                       SCREEN_WIDTH
#define BORDER_Y0                       0
#define BORDER_Y1                       SCREEN_HEIGHT

#define BORDER_LAYER                    14

void init_border(void);
void show_border(void);
void hide_border(void);

#endif

