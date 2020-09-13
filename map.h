#ifndef _MAP_H
#define _MAP_H

#include <stdint.h>
#include <display.h>

typedef struct {
  uint16_t ncols;
  uint16_t nrows;
  uint8_t data[];
} level_t;

enum scroll_dir {
  SCROLL_DIR_HORIZONTAL = 0,
  SCROLL_DIR_VERTICAL
};

void init_map_lib(display *d);
void load_map(display *d, level_t *lvl, enum scroll_dir dir, uint8_t tiles_img[], uint16_t tiles_pal[], int tiles_ncols);
void scroll_map_right(void);
void scroll_map_left(void);
void scroll_map_down(void);
void scroll_map_up(void);
void free_map(void);

#endif

