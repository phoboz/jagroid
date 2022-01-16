#ifndef _BORDER_H
#define _BORDER_H

#include <display.h>

#define BORDER_WIDTH                    64
#define BORDER_HEIGHT                   128
#define BORDER_X0                       16
#define BORDER_X1                       (320 - BORDER_X0)
#define BORDER_Y0                       16
#define BORDER_Y1                       (200 - BORDER_Y0)

#define BORDER_LAYER                    14

void init_border(void);
void show_border(display *d);
void hide_border(void);

#endif

