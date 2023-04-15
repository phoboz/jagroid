#include <jagdefs.h>
#include <jagtypes.h>
#include <stdlib.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include "config.h"
#include "draw_map.h"

#ifdef DEBUG
#include <stdio.h>
extern FILE *fp;
#endif

__inline__ static void copy_column(screen *tgt, Map *tile_map, int lrow, int hrow, int colno) {
  uint16_t nrows = tile_map->h;
  uint16_t *data = tile_map->dat + colno * nrows;
  int i;
  for(i = lrow; i < hrow; i++) {
    int tileno = data[i];
    screen *tile_screen = tile_map->images[tileno].img_screen;
    screen_copy_straight(tile_screen, tgt, 16, 16, MODE_S);
    tgt->y += 16;
  }
}

static void copy_zone(screen *tgt, Map *tile_map, int x, int y, int lrow, int hrow, int lcol, int hcol) {
  int i;
  tgt->x = x;
  for(i = lcol; i < hcol; i++) {
    tgt->y = y;
    copy_column(tgt, tile_map, lrow, hrow, i);
    tgt->x += 16;
  }
}

void init_map_layer(map_layer_t *l, Map *tile_map, enum scroll_dir dir, int pos) {
  l->tile_map = tile_map;
  l->screen1 = new_screen();
  l->screen2 = new_screen();

  l->screen1_data = alloc_simple_screen(DEPTH8, SCREEN_WIDTH, SCREEN_HEIGHT, l->screen1);
  l->screen2_data = alloc_simple_screen(DEPTH8, SCREEN_WIDTH, SCREEN_HEIGHT, l->screen2);

  l->screen1_sprite = sprite_of_screen(0, 0, l->screen1);
  if (dir == SCROLL_DIR_HORIZONTAL) {
    l->screen2_sprite = sprite_of_screen(SCREEN_WIDTH, 0, l->screen2);
  } else if (dir == SCROLL_DIR_VERTICAL) {
    l->screen2_sprite = sprite_of_screen(0, SCREEN_HEIGHT, l->screen2);
  }

  l->scr1 = l->screen1;
  l->scr2 = l->screen2;
  l->spr1 = l->screen1_sprite;
  l->spr2 = l->screen2_sprite;
  l->last_zone_1 = -1;
  l->last_zone_2 = -1;

  l->map_pos = pos << 4;
  l->layer_width = tile_map->w << 4;
  l->layer_height = tile_map->h << 4;

  if (dir == SCROLL_DIR_HORIZONTAL) {
    int col = pos;
    copy_zone(l->scr1, l->tile_map, 0, 0, 0, SCREEN_HEIGHT/16, col, SCREEN_WIDTH/16 + col);
  } else if (dir == SCROLL_DIR_VERTICAL) {
    int row = pos;
    copy_zone(l->scr1, l->tile_map, 0, 0, row, SCREEN_HEIGHT/16 + row, 0, SCREEN_WIDTH/16);
  }
}

void show_map_layer(display *d, int layer, map_layer_t *l) {
  attach_sprite_to_display_at_layer(l->screen1_sprite, d, layer);
  attach_sprite_to_display_at_layer(l->screen2_sprite, d, layer);
}

void hide_map_layer(map_layer_t *l) {
  detach_sprite_from_display(l->screen1_sprite);
  detach_sprite_from_display(l->screen2_sprite);
}

void scroll_map_layer_right(map_layer_t *l, int dx) {
  if (l->map_pos + dx < l->layer_width - SCREEN_WIDTH) {
    l->map_pos += dx;

    l->spr1->x -= dx;
    l->spr2->x -= dx;

    int xmod16 = l->map_pos & 15;
    if (xmod16 >= MAX_SCROLL_SPEED) {
      int col = SCREEN_WIDTH/16 + (l->map_pos >> 4);
      if (col != l->last_zone_2) {
        if (l->spr1->x <= -SCREEN_WIDTH) {
          l->spr1->x += 2 * SCREEN_WIDTH;

          screen *tmp_scr = l->scr1;
          l->scr1 = l->scr2;
          l->scr2 = tmp_scr;

          sprite *tmp_spr = l->spr1;
          l->spr1 = l->spr2;
          l->spr2 = tmp_spr;
        }

        l->scr2->x = -l->spr1->x - xmod16;
        l->scr2->y = 0;
        copy_column(l->scr2, l->tile_map, 0, SCREEN_HEIGHT/16, col);
        l->last_zone_2 = col;
      }
    }
  }
}

void scroll_map_layer_left(map_layer_t *l, int dx) {
  if (l->map_pos - dx > 0) {
    l->map_pos -= dx;

    l->spr1->x += dx;
    l->spr2->x += dx;

    int xmod16 = l->map_pos & 15;
    if (xmod16 >= MAX_SCROLL_SPEED) {
      int col = l->map_pos >> 4;
      if (col != l->last_zone_1) {
        if (l->spr2->x >= SCREEN_WIDTH) {
          l->spr2->x -= 2 * SCREEN_WIDTH;

          screen *tmp_scr = l->scr1;
          l->scr1 = l->scr2;
          l->scr2 = tmp_scr;

          sprite *tmp_spr = l->spr1;
          l->spr1 = l->spr2;
          l->spr2 = tmp_spr;
        }

        l->scr1->x = SCREEN_WIDTH - l->spr2->x - xmod16;
        l->scr1->y = 0;
        copy_column(l->scr1, l->tile_map, 0, SCREEN_HEIGHT/16, col);
        l->last_zone_1 = col;
      }
    }
  }
}

void scroll_map_layer_down(map_layer_t *l, int dy) {
  if (l->map_pos + dy < l->layer_height - SCREEN_HEIGHT) {
    l->map_pos += dy;

    l->spr1->y -= dy;
    l->spr2->y -= dy;

    int ymod16 = l->map_pos & 15;
    if (ymod16 >= MAX_SCROLL_SPEED) {
      int row = SCREEN_HEIGHT/16 + (l->map_pos >> 4);
      if (row != l->last_zone_2) {
        if (l->spr1->y <= -SCREEN_HEIGHT) {
          l->spr1->y += 2 * SCREEN_HEIGHT;

          screen *tmp_scr = l->scr1;
          l->scr1 = l->scr2;
          l->scr2 = tmp_scr;

          sprite *tmp_spr = l->spr1;
          l->spr1 = l->spr2;
          l->spr2 = tmp_spr;
        }

        int dy = -l->spr1->y - ymod16;
        copy_zone(l->scr2, l->tile_map, 0, dy, row, row + 1, 0, SCREEN_WIDTH/16);
        l->last_zone_2 = row;
      }
    }
  }
}

void scroll_map_layer_up(map_layer_t *l, int dy) {
  if (l->map_pos - dy > 0) {
    l->map_pos -= dy;

    l->spr1->y += dy;
    l->spr2->y += dy;

    int ymod16 = l->map_pos & 15;
    if (ymod16 >= MAX_SCROLL_SPEED) {
      int row = l->map_pos >> 4;
      if (row != l->last_zone_1) {
        if (l->spr2->y >= SCREEN_HEIGHT) {
          l->spr2->y -= 2 * SCREEN_HEIGHT;

          screen *tmp_scr = l->scr1;
          l->scr1 = l->scr2;
          l->scr2 = tmp_scr;

          sprite *tmp_spr = l->spr1;
          l->spr1 = l->spr2;
          l->spr2 = tmp_spr;
        }

        int dy = -l->spr1->y - ymod16;
        copy_zone(l->scr1, l->tile_map, 0, dy, row, row + 1, 0, SCREEN_WIDTH/16);
        l->last_zone_1 = row;
      }
    }
  }
}

void free_map_layer(map_layer_t *l) {
  free(l->screen1_sprite);
  free(l->screen1_data);
  free(l->screen1);
  free(l->screen2_sprite);
  free(l->screen2_data);
  free(l->screen2);
}

