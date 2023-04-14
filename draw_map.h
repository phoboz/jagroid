#ifndef _DRAW_MAP_H
#define _DRAW_MAP_H

#include <stdint.h>
#include <jagdefs.h>
#include <jagtypes.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include "map.h"

enum scroll_dir {
  SCROLL_DIR_HORIZONTAL = 0,
  SCROLL_DIR_VERTICAL
};

typedef struct map_layer {
  Map *tile_map;
  int map_x, map_y;
  int layer_width, layer_height;
  screen *screen1, *screen2;
  phrase *screen1_data, *screen2_data;
  sprite *screen1_sprite, *screen2_sprite;

  screen *scr1, *scr2;
  sprite *spr1, *spr2;
  int last_zone_1, last_zone_2;
} map_layer_t;

void init_map_layer(map_layer_t *l, Map *tile_map, enum scroll_dir dir, int x, int y);
void show_map_layer(display *d, int layer, map_layer_t *l);
void hide_map_layer(map_layer_t *l);
void scroll_map_layer_right(map_layer_t *l, int dx);
void scroll_map_layer_left(map_layer_t *l, int dx);
void scroll_map_layer_down(map_layer_t *l, int dy);
void scroll_map_layer_up(map_layer_t *l, int dy);
void free_map_layer(map_layer_t *l);

#endif

