#include <stdlib.h>
#include <jagdefs.h>
#include <jagtypes.h>
#include <interrupt.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include <joypad.h>
#include "map.h"

#define NCOLS 16

extern uint16_t tiscavPal[NCOLS];
extern uint8_t tiscavTiles[];
extern level_t tiscavLevel01;
extern level_t tiscavLevel02;

unsigned long lock_keys;

int main(int argc, char *argv[]) {
  TOMREGS->vmode = RGB16|CSYNC|BGEN|PWIDTH4|VIDEN;
  init_interrupts();
  init_display_driver();

  display *d = new_display(0);

  d->x = 20;
  d->y = 16;

  init_map_lib(d);
  load_map(d, &tiscavLevel01, SCROLL_DIR_HORIZONTAL, tiscavTiles, tiscavPal, NCOLS);

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
      scroll_map_right();
    } else if(cmd & JOYPAD_LEFT) {
      scroll_map_left();
    } else if (cmd & JOYPAD_DOWN) {
      scroll_map_down();
    } else if (cmd & JOYPAD_UP) {
      scroll_map_up();
    } else if (cmd & JOYPAD_1) {
      if ((lock_keys & JOYPAD_1) == 0) {
        free_map();
        load_map(d, &tiscavLevel01, SCROLL_DIR_HORIZONTAL, tiscavTiles, tiscavPal, NCOLS);
        lock_keys |= JOYPAD_1;
      }
    } else if (cmd & JOYPAD_2) {
      if ((lock_keys & JOYPAD_2) == 0) {
        free_map();
        load_map(d, &tiscavLevel02, SCROLL_DIR_VERTICAL, tiscavTiles, tiscavPal, NCOLS);
        lock_keys |= JOYPAD_2;
      }
    }

    wait_display_refresh();
  }

  free_map();
}

