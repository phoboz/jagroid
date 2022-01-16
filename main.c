#include <stdlib.h>
#include <jagdefs.h>
#include <jagtypes.h>
#include <interrupt.h>
#include <display.h>
#include <sprite.h>
#include <screen.h>
#include <joypad.h>
#include "border.h"
#include "map.h"
#include "data.h"

unsigned long lock_keys;

int main(int argc, char *argv[]) {
  TOMREGS->vmode = RGB16|CSYNC|BGEN|PWIDTH4|VIDEN;
  init_interrupts();
  init_display_driver();

  display *d = new_display(0);

  d->x = 20;
  d->y = 16;

  //init_map_lib(d);
  init_border();

  load_map(&tiscavLevel01, SCROLL_DIR_HORIZONTAL, tiscavTiles, tiscavPal, MAP_NCOLS, 0, 0);

  show_border(d);
  show_map(d, 0);
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
      scroll_map_right();
    } else if(cmd & JOYPAD_LEFT) {
      scroll_map_left();
    } else if (cmd & JOYPAD_DOWN) {
      scroll_map_down();
    } else if (cmd & JOYPAD_UP) {
      scroll_map_up();
    } else if (cmd & JOYPAD_1) {
      if ((lock_keys & JOYPAD_1) == 0) {
	hide_map();
        free_map();
        int x = rand() % (tiscavLevel01.ncols - 20);
        load_map(&tiscavLevel01, SCROLL_DIR_HORIZONTAL, tiscavTiles, tiscavPal, MAP_NCOLS, x, 0);
	show_map(d, 0);
        lock_keys |= JOYPAD_1;
      }
    } else if (cmd & JOYPAD_2) {
      if ((lock_keys & JOYPAD_2) == 0) {
	hide_map();
        free_map();
        int y = rand() % (tiscavLevel02.nrows - 12);
        load_map(&tiscavLevel02, SCROLL_DIR_VERTICAL, tiscavTiles, tiscavPal, MAP_NCOLS, 0, y);
	show_map(d, 0);
        lock_keys |= JOYPAD_2;
      }
    }

    wait_display_refresh();
  }

  hide_map();
  free_map();
}

