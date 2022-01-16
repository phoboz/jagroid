#include <stdlib.h>
#include <sprite.h>
#include <screen.h>
#include "border.h"

static screen *border_screen_ver;
static phrase *border_buf_ver;
static screen *border_screen_hor;
static phrase *border_buf_hor;
static sprite *border_sprite_left;
static sprite *border_sprite_right;
static sprite *border_sprite_top;
static sprite *border_sprite_bottom;

void init_border(void)
{
  border_screen_ver = new_screen();
  border_buf_ver = alloc_simple_screen(DEPTH1, BORDER_WIDTH, 240, border_screen_ver);
  memset(border_buf_ver, 0, BORDER_WIDTH * 240 / 8);

  border_screen_hor = new_screen();
  border_buf_hor = alloc_simple_screen(DEPTH1, 320 + 2 * BORDER_WIDTH, BORDER_HEIGHT, border_screen_hor);
  memset(border_buf_hor, 0, (320 + 2 * BORDER_WIDTH) * BORDER_HEIGHT / 8);

  border_sprite_left = sprite_of_screen(BORDER_X0, 0, border_screen_ver);
  border_sprite_left->hx = BORDER_WIDTH;
  border_sprite_left->hy = 0;
  border_sprite_left->use_hotspot = 1;
  border_sprite_left->trans = 0;

  border_sprite_right = sprite_of_screen(BORDER_X1, 0, border_screen_ver);
  border_sprite_right->trans = 0;

  border_sprite_top = sprite_of_screen(-BORDER_WIDTH, BORDER_Y0, border_screen_hor);
  border_sprite_top->hx = 0;
  border_sprite_top->hy = BORDER_HEIGHT;
  border_sprite_top->use_hotspot = 1;
  border_sprite_top->trans = 0;

  border_sprite_bottom = sprite_of_screen(-BORDER_WIDTH, BORDER_Y1, border_screen_hor);
  border_sprite_bottom->trans = 0;
}

void show_border(display *d)
{
  attach_sprite_to_display_at_layer(border_sprite_left, d, BORDER_LAYER);
  attach_sprite_to_display_at_layer(border_sprite_right, d, BORDER_LAYER);
  attach_sprite_to_display_at_layer(border_sprite_top, d, BORDER_LAYER);
  attach_sprite_to_display_at_layer(border_sprite_bottom, d, BORDER_LAYER);
}

void hide_border(void)
{
  detach_sprite_from_display(border_sprite_left);
  detach_sprite_from_display(border_sprite_right);
  detach_sprite_from_display(border_sprite_top);
  detach_sprite_from_display(border_sprite_bottom);
}

