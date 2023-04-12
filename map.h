/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
*
*   This file is part of yz.
*   Copyright (C) 2014 Surplus Users Ham Society
*
*   Yz is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 2 of the License, or
*   (at your option) any later version.
*
*   Yz is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public License
*   along with Yz.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

/*
 * map.h -- tiled map format
 * header for map.c
 */

#ifndef _map_h
#define _map_h

#include <stdint.h>
#include <screen.h>

typedef enum {
  MAP_ANIM_NONE = 0,
  MAP_ANIM_LOOP,
  MAP_ANIM_PINGPONG
} MapAnim;

typedef struct	{
  union {
    struct {
      phrase *img_buf;
      uint16_t index;
      uint16_t x, y;
      uint8_t type;
      uint16_t mask;
      MapAnim animated;
      uint16_t start_index,end_index;
      uint16_t counter,treshold;
    };
    struct {
      uint8_t foo[32];
    };
  };
} Tile;

typedef struct {
  screen *img_screen;
  phrase *img_buf;
} MapImg;

enum object_type
{
  OBJECT_TYPE_UNKNOWN = 0,
  OBJECT_TYPE_ENEMY,
  OBJECT_TYPE_BOSS,
  OBJECT_TYPE_ITEM,
  OBJECT_TYPE_SAVETUBE,
  OBJECT_TYPE_LIGHT,
  OBJECT_TYPE_NPC,
  OBJECT_TYPE_STATIC
};

typedef struct {
  int16_t x;
  int16_t y;
} NPCPoint;

typedef struct {
  uint8_t type;
  uint8_t index;
  uint8_t dir;
  uint8_t param;
  uint16_t x, y;
  uint8_t num_points;
  NPCPoint *points;
} Object;

enum area_type
{
  AREA_TYPE_UNKNOWN,
  AREA_TYPE_DOOR,
  AREA_TYPE_DAMAGE,
  AREA_TYPE_TRIGGER
};

typedef struct {
  uint8_t type;
  uint16_t level;
  uint16_t start_x, start_y;
  uint8_t dir;
  uint16_t x, y;
  uint16_t w, h;
} Area;

typedef struct	{
  uint16_t tw,th;
  uint16_t w,h;
  uint16_t num_tiles,size;
  uint16_t  *dat;
  Tile *tiles;
  MapImg *images;

  uint16_t num_objects;
  Object *objects;

  uint16_t num_areas;
  Area *areas;
} Map;

extern int map_anim_dir;

Map* new_map(uint16_t tw, uint16_t th, uint16_t num_tiles, uint16_t w, uint16_t h, uint16_t tiles_img_w, uint16_t tiles_img_h, uint8_t *tile_data);
Map* load_map(uint16_t tw, uint16_t th, uint16_t tiles_img_w, uint16_t tiles_img_h, uint8_t *tile_data, uint8_t *level_data);
void free_map(Map *m);
void clear_map(Map *m, uint16_t tile_num);
void update_animations_map(Map *m);

__inline__ static void get_tile_size_map(Map *m, uint16_t *w, uint16_t *h)
{
  *w = m->tw;
  *h = m->th;
}

__inline__ static void get_screen_size_map(Map *m, uint16_t *w, uint16_t *h)
{
  *w = m->w * m->tw;
  *h = m->h * m->th;
}

__inline__ static void puttile_map(Map *m,int x,int y,uint16_t t)
{
  m->dat[x+y*m->w]=t;
}

__inline__ static uint16_t gettile_map(Map *m,int x,int y)
{
  return(m->dat[x+y*m->w]);
}

__inline__ static int8_t gettile_type_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x+y*m->w])].type);
}

__inline__ static uint16_t gettile_index_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x+y*m->w])].index);
}

__inline__ static phrase* getblock_data_map(Map *m,int x,int y)
{
  return(m->tiles[(int)(m->dat[x+y*m->w])].img_buf);
}

__inline__ static void animate_tile_loop(Map *m, int index)
{
  Tile *tl = &m->tiles[index];

  if (++tl->counter >= tl->treshold)
  {
    tl->counter = 0;

    tl->index++;

    if(tl->index < tl->start_index)
      tl->index = tl->end_index;

    else if(tl->index > tl->end_index)
      tl->index = tl->start_index;

    tl->img_buf = m->images[tl->index].img_buf;
  }
}

__inline__ static void animate_tile_pingpong(Map *m, int index)
{
  Tile *tl = &m->tiles[index];
 
  if (++tl->counter >= tl->treshold)
  {
    tl->counter = 0;

    if(tl->index <= tl->start_index)
      map_anim_dir=1;

    else if(tl->index >= tl->end_index)
      map_anim_dir=-1;

    tl->index += map_anim_dir;

    tl->img_buf = m->images[tl->index].img_buf;
  }
}

#endif

