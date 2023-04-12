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

#define SCROLL_SPEED 2

void *gpu_addr;

unsigned long lock_keys;

Map *tile_map;
map_layer_t map_layer;

int main(int argc, char *argv[]) {
  TOMREGS->vmode = CRY16|CSYNC|BGEN|PWIDTH4|VIDEN;
  init_interrupts();
  init_display_driver();

  gpu_addr = &_GPU_FREE_RAM;
  init_image_lib();

  display *d = new_display(0);

  d->x = 20;
  d->y = 16;

  init_border();

  tile_map = load_map(16, 16, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT, &tiles_01, &level_01);
  init_map_layer(&map_layer, tile_map, SCROLL_DIR_HORIZONTAL, 0, 0);

  memcpy((void*)TOMREGS->clut1, tiles_01_pal, MAP_NCOLS * sizeof(uint16_t));

  show_border(d);
  show_map_layer(d, 0, &map_layer);
  show_display(d);

  joypad_state joypads;

  lock_keys = 0;

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
      scroll_map_layer_right(&map_layer, SCROLL_SPEED);
    } else if(cmd & JOYPAD_LEFT) {
      scroll_map_layer_left(&map_layer, SCROLL_SPEED);
    } else if (cmd & JOYPAD_DOWN) {
      scroll_map_layer_down(&map_layer, SCROLL_SPEED);
    } else if (cmd & JOYPAD_UP) {
      scroll_map_layer_up(&map_layer, SCROLL_SPEED);
    } else if (cmd & JOYPAD_1) {
      if ((lock_keys & JOYPAD_1) == 0) {
	hide_map_layer(&map_layer);
        free_map_layer(&map_layer);
        int x = rand() % (tile_map->w - 20);
        init_map_layer(&map_layer, tile_map, SCROLL_DIR_HORIZONTAL, x, 0);
	show_map_layer(d, 0, &map_layer);
        lock_keys |= JOYPAD_1;
      }
    } else if (cmd & JOYPAD_2) {
      if ((lock_keys & JOYPAD_2) == 0) {
	hide_map_layer(&map_layer);
        free_map_layer(&map_layer);
        int y = rand() % (tile_map->h - 12);
        init_map_layer(&map_layer, tile_map, SCROLL_DIR_VERTICAL, 0, y);
	show_map_layer(d, 0, &map_layer);
        lock_keys |= JOYPAD_2;
      }
    }

    wait_display_refresh();
  }

  hide_map_layer(&map_layer);
  free_map_layer(&map_layer);
}

