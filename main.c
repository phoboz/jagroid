#include <stdlib.h>
#include <jagdefs.h>
#include <jagtypes.h>
#include <interrupt.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include <joypad.h>
#include "config.h"
#include "border.h"
#include "image.h"
#include "draw_map.h"
#include "data.h"

#define SCROLL_SPEED_1 3
#define SCROLL_SPEED_2 2

#ifdef DEBUG
#include <stdio.h>
FILE *fp;
#endif

void *gpu_addr;
display *game_display;

unsigned long lock_keys;

Map *tile_map;
map_layer_t map_layer;

int main(int argc, char *argv[]) {
  TOMREGS->vmode = CRY16|CSYNC|BGEN|PWIDTH4|VIDEN;
  init_interrupts();
  init_display_driver();

  gpu_addr = &_GPU_FREE_RAM;
  init_image_lib();

  game_display = new_display(0);

  game_display->x = 20;
  game_display->y = 16;

  init_border();

#ifdef DEBUG
  fp = open_custom_console(
    game_display,
    0,
    SCREEN_HEIGHT,
    255,
    40,
    1,
    15
    );
#endif

  TOMREGS->bg = 0xff;
  tile_map = load_map(16, 16, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT, tiles_01, &level_01);
  init_map_layer(&map_layer, tile_map, SCROLL_DIR_HORIZONTAL, 0);

  memcpy((void*)TOMREGS->clut1, tiles_pal, MAP_NCOLS * sizeof(uint16_t));

  show_border();
  show_map_layer(game_display, 0, &map_layer);
  show_display(game_display);

  joypad_state joypads;

  lock_keys = 0;

#ifdef DEBUG
  fprintf(fp, "%d, %d\n", map_layer.map_x, map_layer.map_y);
#endif

  while(1) {
    vsync();
    read_joypad_state(&joypads);
    unsigned long cmd = joypads.j1;

    if (lock_keys) {
      if ((cmd & JOYPAD_1) == 0) {
        lock_keys &= ~JOYPAD_1;
      }

      if ((cmd & JOYPAD_2) == 0) {
        lock_keys &= ~JOYPAD_2;
      }
    }

    if(cmd & JOYPAD_RIGHT) {
      if (map_layer.dir == SCROLL_DIR_HORIZONTAL) {
        scroll_map_layer_right(&map_layer, SCROLL_SPEED_1);
      }
#ifdef DEBUG
      fprintf(fp, "%d, %d\n", map_layer.map_x, map_layer.map_y);
#endif
    } else if(cmd & JOYPAD_LEFT) {
      if (map_layer.dir == SCROLL_DIR_HORIZONTAL) {
        scroll_map_layer_left(&map_layer, SCROLL_SPEED_2);
      }
#ifdef DEBUG
      fprintf(fp, "%d, %d\n", map_layer.map_x, map_layer.map_y);
#endif
    } else if (cmd & JOYPAD_DOWN) {
      if (map_layer.dir == SCROLL_DIR_VERTICAL) {
        scroll_map_layer_down(&map_layer, SCROLL_SPEED_1);
      }
#ifdef DEBUG
      fprintf(fp, "%d, %d\n", map_layer.map_x, map_layer.map_y);
#endif
    } else if (cmd & JOYPAD_UP) {
      if (map_layer.dir == SCROLL_DIR_VERTICAL) {
        scroll_map_layer_up(&map_layer, SCROLL_SPEED_2);
      }
#ifdef DEBUG
      fprintf(fp, "%d, %d\n", map_layer.map_x, map_layer.map_y);
#endif
    } else if (cmd & JOYPAD_1) {
      if ((lock_keys & JOYPAD_1) == 0) {
	hide_map_layer(&map_layer);
        free_map_layer(&map_layer);
        free_map(tile_map);
        tile_map = load_map(16, 16, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT, tiles_01, &level_01);
        int x = rand() % (tile_map->w - 20);
        init_map_layer(&map_layer, tile_map, SCROLL_DIR_HORIZONTAL, x);
	show_map_layer(game_display, 0, &map_layer);
        lock_keys |= JOYPAD_1;
      }
    } else if (cmd & JOYPAD_2) {
      if ((lock_keys & JOYPAD_2) == 0) {
	hide_map_layer(&map_layer);
        free_map_layer(&map_layer);
        free_map(tile_map);
        tile_map = load_map(16, 16, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT, tiles_02, &level_02);
        int y = rand() % (tile_map->h - 12);
        init_map_layer(&map_layer, tile_map, SCROLL_DIR_VERTICAL, y);
	show_map_layer(game_display, 0, &map_layer);
        lock_keys |= JOYPAD_2;
      }
    }

    wait_display_refresh();
  }

  hide_map_layer(&map_layer);
  free_map_layer(&map_layer);

#ifdef DEBUG
  fclose(fp);
#endif
}

