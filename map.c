#include <jagdefs.h>
#include <jagtypes.h>
#include <stdlib.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include "map.h"

#define WIDTH 320
#define HEIGHT 192

#define TILES_PER_LINE (320 / 16)
#define NUM_TILES (TILES_PER_LINE * 192 / 16)

#define BORDER_WIDTH 6
#define BORDER_HEIGHT 6

typedef struct {
  uint16_t grid_x;
  uint16_t grid_y;
} tile_t;

typedef struct {
  uint16_t ncols;
  uint16_t nrows;
  uint8_t *data;
} level_data_t;

static tile_t *tile_data;
static level_data_t level_data;
static int map_x;
static int map_y;
static int map_width;
static int map_height;

static screen *tiles;
static phrase *tiles_data;
static screen *screen1;
static screen *screen2;
static phrase *screen1_data;
static phrase *screen2_data;
static sprite *screen1_sprite;
static sprite *screen2_sprite;

static screen *scr1;
static screen *scr2;
static sprite *spr1;
static sprite *spr2;

static void copy_column(screen *tiles, screen *tgt, level_data_t *lvl, int lrow, int hrow, int colno) {
  uint16_t nrows = lvl->nrows;
  uint8_t *data = lvl->data + colno * nrows;
  int i;
  for(i = lrow; i < hrow; i++) {
    int tileno = data[i];
    tile_t *tile = &tile_data[tileno];
    tiles->x = tile->grid_x;
    tiles->y = tile->grid_y;
    screen_copy_straight(tiles, tgt, 16, 16, MODE_S);
    tgt->y += 16;
  }
}

static void copy_zone(screen *tiles, screen *tgt, level_data_t *lvl, int x, int y, int lrow, int hrow, int lcol, int hcol) {
  int i;
  tgt->x = x;
  for(i = lcol; i < hcol; i++) {
    tgt->y = y;
    copy_column(tiles, tgt, lvl, lrow, hrow, i);
    tgt->x += 16;
  }
}

void load_map(level_t *lvl, enum scroll_dir dir, uint8_t tiles_img[], uint16_t tiles_pal[], int tiles_ncols, int x, int y) {
  screen *tmp = new_screen();
  set_simple_screen(DEPTH8, 320, HEIGHT, tmp, (phrase *) tiles_img);

  tiles = new_screen();
  tiles_data = alloc_simple_screen(DEPTH8, 320, HEIGHT, tiles);
  screen_copy_straight(tmp, tiles, 320, HEIGHT, MODE_S);

  free(tmp);

  memcpy((void*)TOMREGS->clut1, tiles_pal, tiles_ncols * sizeof(uint16_t));

  screen1 = new_screen();
  screen2 = new_screen();

  screen1_data = alloc_simple_screen(DEPTH8, WIDTH, HEIGHT, screen1);
  screen2_data = alloc_simple_screen(DEPTH8, WIDTH, HEIGHT, screen2);

  screen1_sprite = sprite_of_screen(0, 0, screen1);
  if (dir == SCROLL_DIR_HORIZONTAL) {
    screen2_sprite = sprite_of_screen(WIDTH, 0, screen2);
  } else if (dir == SCROLL_DIR_VERTICAL) {
    screen2_sprite = sprite_of_screen(0, HEIGHT, screen2);
  }

  screen1_sprite->trans = 0;
  screen2_sprite->trans = 0;

  clear_screen(screen1);
  clear_screen(screen2);

  scr1 = screen1;
  scr2 = screen2;
  spr1 = screen1_sprite;
  spr2 = screen2_sprite;

  tile_data = (tile_t *) malloc(sizeof(tile_t) * NUM_TILES);

  int i;
  for (i = 0; i < NUM_TILES; i++) {
    tile_data[i].grid_x = (i % TILES_PER_LINE) * 16;
    tile_data[i].grid_y = (i / TILES_PER_LINE) * 16;
  }

  level_data.ncols = lvl->ncols;
  level_data.nrows = lvl->nrows;
  level_data.data = (uint8_t *) malloc(level_data.ncols * level_data.nrows);
  memcpy(level_data.data, lvl->data, level_data.ncols * level_data.nrows);

  map_x = x * 16;
  map_y = y * 16;
  map_width = level_data.ncols * 16;
  map_height = level_data.nrows * 16;

  int map_tx = map_x / 16;
  int map_ty = map_y / 16;

  copy_zone(tiles, scr1, &level_data, 0, 0, map_ty, HEIGHT/16 + map_ty, map_tx, WIDTH/16 + map_tx);

  if (dir == SCROLL_DIR_HORIZONTAL) {
    copy_zone(tiles, scr2, &level_data, 0, 0, map_ty, HEIGHT/16 + map_ty, WIDTH/16 + map_tx, WIDTH/16 + map_tx + 1);
  } else if (dir == SCROLL_DIR_VERTICAL) {
    copy_zone(tiles, scr2, &level_data, 0, 0, HEIGHT/16 + map_ty, HEIGHT/16 + map_ty + 1, map_tx, WIDTH/16 + map_tx);
  }
}

void show_map(display *d, int layer) {
  attach_sprite_to_display_at_layer(screen1_sprite, d, layer);
  attach_sprite_to_display_at_layer(screen2_sprite, d, layer);
}

void hide_map(void) {
  detach_sprite_from_display(screen1_sprite);
  detach_sprite_from_display(screen2_sprite);
}

void scroll_map_right(void) {
  if (map_x < map_width - WIDTH) {
    map_x++;

    if (spr1->x <= -WIDTH) {
      spr1->x += 2 * WIDTH;

      screen *tmp_scr = scr1;
      scr1 = scr2;
      scr2 = tmp_scr;

      sprite *tmp_spr = spr1;
      spr1 = spr2;
      spr2 = tmp_spr;
    }

    spr1->x--;
    spr2->x--;

    if ((map_x & 15) == 8) {
      int map_tx = map_x >> 4;
      int dx = -spr1->x - 8;
      copy_zone(tiles, scr2, &level_data, dx, 0, 0, HEIGHT/16, WIDTH/16 + map_tx, WIDTH/16 + map_tx + 1);
    }
  }
}

void scroll_map_left(void) {
  if (map_x > 0) {
    map_x--;

    if (spr2->x >= WIDTH) {
      spr2->x -= 2 * WIDTH;

      screen *tmp_scr = scr1;
      scr1 = scr2;
      scr2 = tmp_scr;

      sprite *tmp_spr = spr1;
      spr1 = spr2;
      spr2 = tmp_spr;
    }

    spr1->x++;
    spr2->x++;

    if ((map_x & 15) == 8) {
      int map_tx = map_x >> 4;
      int dx = -spr1->x - 8;
      copy_zone(tiles, scr1, &level_data, dx, 0, 0, HEIGHT/16, map_tx, map_tx + 1);
    }
  }
}

void scroll_map_down(void) {
  if (map_y < map_height - HEIGHT) {
    map_y++;

    if (spr1->y <= -HEIGHT) {
      spr1->y += 2 * HEIGHT;

      screen *tmp_scr = scr1;
      scr1 = scr2;
      scr2 = tmp_scr;

      sprite *tmp_spr = spr1;
      spr1 = spr2;
      spr2 = tmp_spr;
    }

    spr1->y--;
    spr2->y--;

    if ((map_y & 15) == 8) {
      int map_ty = map_y >> 4;
      int dy = -spr1->y - 8;
      copy_zone(tiles, scr2, &level_data, 0, dy, HEIGHT/16 + map_ty, HEIGHT/16 + map_ty + 1, 0, WIDTH/16);
    }
  }
}

void scroll_map_up(void) {
  if (map_y > 0) {
    map_y--;

    if (spr2->y >= HEIGHT) {
      spr2->y -= 2 * HEIGHT;

      screen *tmp_scr = scr1;
      scr1 = scr2;
      scr2 = tmp_scr;

      sprite *tmp_spr = spr1;
      spr1 = spr2;
      spr2 = tmp_spr;
    }

    spr1->y++;
    spr2->y++;

    if ((map_y & 15) == 8) {
      int map_ty = map_y >> 4;
      int dy = -spr1->y - 8;
      copy_zone(tiles, scr1, &level_data, 0, dy, map_ty, map_ty + 1, 0, WIDTH/16);
    }
  }
}

void free_map(void) {
  free(screen1_sprite);
  free(screen1_data);
  free(screen1);
  free(screen2_sprite);
  free(screen2_data);
  free(screen2);
  free(tiles);
  free(tiles_data);
  free(tile_data);
  free(level_data.data);
}

