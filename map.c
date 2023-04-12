/******************************************************************************
*   DO NOT ALTER OR REMOVE COPYE NOTICES OR THIS HEADER.
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
 * map.c -- tiled map format
 */

#include <stdio.h>
#include <stdlib.h>
#include <jagtypes.h>
#include <jagdefs.h>
#include <display.h>
#include <sprite.h>
#include <display.h>
#include <lz77.h>
#include "image.h"
#include "map.h"

int map_anim_dir;

static void init_map_tiles(Map *map, uint16_t tiles_img_w, screen *tiles_screen)
{
  unsigned int i;
  uint16_t tiles_per_line = tiles_img_w / map->tw;

  for (i = 0; i < map->num_tiles; i++) {

    map->tiles[i].index = i;
    map->tiles[i].x = (i % tiles_per_line) * map->tw;
    map->tiles[i].y = (i / tiles_per_line) * map->th;
    map->tiles[i].type = 0;
    map->tiles[i].animated = MAP_ANIM_NONE;
    map->tiles[i].start_index = map->tiles[i].end_index = 0;
    map->tiles[i].counter = map->tiles[i].treshold = 0;

    map->images[i].img_screen = new_screen();
    map->images[i].img_buf = alloc_simple_screen(DEPTH8, map->tw, map->th, map->images[i].img_screen);

    tiles_screen->x = map->tiles[i].x;
    tiles_screen->y = map->tiles[i].y;
    screen_copy_straight(tiles_screen, map->images[i].img_screen, map->tw, map->th, MODE_S);

    map->tiles[i].img_buf = map->images[i].img_buf;
  }
}

Map* new_map(uint16_t tw, uint16_t th, uint16_t num_tiles, uint16_t w, uint16_t h, uint16_t tiles_img_w, uint16_t tiles_img_h, uint8_t *tile_data)
{
  Map *map;

  map = (Map *) malloc( sizeof(Map) );
  if (map == NULL)
    return NULL;

  map->tiles = (Tile *) malloc( sizeof(Tile) * num_tiles);
  if (map->tiles == NULL) {
    free(map);
    return NULL;

  }

  map->images = (MapImg *) malloc( sizeof(MapImg) * num_tiles);
  if (map->tiles == NULL) {
    free(map);
    free(map->tiles);
    return NULL;

  }

  map->tw = tw;
  map->th = th;
  map->num_tiles = num_tiles;
  map->w = w;
  map->h = h;
  map->size = w * h;
  map->dat = (uint16_t *) malloc( sizeof(map->dat[0]) * map->size);

  screen *tmp_scr = new_screen();
#ifdef PAK_TILES
  phrase *tmp_buf = alloc_simple_screen(DEPTH8, tiles_img_w, tiles_img_h, tmp_scr);
  lz77_unpack(image_gpu_addr, tile_data, (uint8_t *) tmp_buf);
#else
  set_simple_screen(DEPTH8, tiles_img_w, tiles_img_h, tmp_scr, (phrase *) tile_data);
#endif

  init_map_tiles(map, tiles_img_w, tmp_scr);

#ifdef PAK_TILES
  free(tmp_buf);
#endif
  free(tmp_scr);

  map_anim_dir = 1;

  map->num_objects = 0;
  map->num_areas = 0;

  return map;
}

Map* load_map(uint16_t tw, uint16_t th, uint16_t tiles_img_w, uint16_t tiles_img_h, uint8_t *tile_data, uint8_t *level_data)
{
  Map *map;
  int i;

  map = (Map *) malloc( sizeof(Map) );
  if (map == NULL)
    return NULL;

  map->tw = tw;
  map->th = th;
  map->num_tiles = (level_data[0] << 8) | (level_data[1]); level_data += 2;

  map->tiles = (Tile *) malloc( sizeof(Tile) * map->num_tiles);
  if (map->tiles == NULL) {
    free(map);
    return NULL;

  }

  map->images = (MapImg *) malloc( sizeof(MapImg) * map->num_tiles);
  if (map->tiles == NULL) {
    free(map->tiles);
    free(map);
    return NULL;

  }

  screen *tmp_scr = new_screen();
#ifdef PAK_TILES
  phrase *tmp_buf = alloc_simple_screen(DEPTH8, tiles_img_w, tiles_img_h, tmp_scr);
  lz77_unpack(image_gpu_addr, tile_data, (uint8_t *) tmp_buf);
#else
  set_simple_screen(DEPTH8, tiles_img_w, tiles_img_h, tmp_scr, (phrase *) tile_data);
#endif

  init_map_tiles(map, tiles_img_w, tmp_scr);

#ifdef PAK_TILES
  free(tmp_buf);
#endif
  free(tmp_scr);

  map_anim_dir = 1;

  for (i = 0; i < map->num_tiles; i++) {
    map->tiles[i].type = *level_data; level_data++;
    map->tiles[i].mask = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  }

  map->w = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  map->h = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  map->size = map->w * map->h;
  map->dat = (uint16_t *) malloc( sizeof(map->dat[0]) * map->size);

  for (i = 0; i < map->size; i++) {
    map->dat[i] = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  }

  map->num_objects = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  map->objects = (Object *) malloc( sizeof(Object) * map->num_objects);
  if (map->objects == NULL) {
    free(map->images);
    free(map->tiles);
    free(map);
    return NULL;

  }

  for (i = 0; i < map->num_objects; i++) {
    map->objects[i].type = *level_data; level_data++;
    map->objects[i].index = *level_data; level_data++;
    map->objects[i].dir = *level_data; level_data++;
    map->objects[i].param = *level_data; level_data++;
    map->objects[i].x = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->objects[i].y = (level_data[0] << 8) | (level_data[1]); level_data += 2;

    if (map->objects[i].type == OBJECT_TYPE_NPC)
    {
      int p;
      uint8_t num_points = *level_data; level_data++;
      map->objects[i].num_points = num_points;
      if (num_points > 0)
      {
        map->objects[i].points = (NPCPoint *) malloc( sizeof(NPCPoint) * num_points);
        for (p = 0; p < map->objects[i].num_points; p++)
        {
          map->objects[i].points[p].x = (level_data[0] << 8) | (level_data[1]); level_data += 2;
          map->objects[i].points[p].y = (level_data[0] << 8) | (level_data[1]); level_data += 2;
        }
      }
    }
  }

  map->num_areas = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  map->areas = (Area *) malloc( sizeof(Area) * map->num_areas);
  if (map->areas == NULL) {
    free(map->objects);
    free(map->images);
    free(map->tiles);
    free(map);
    return NULL;
  }

  for (i = 0; i < map->num_areas; i++) {
    map->areas[i].type = *level_data; level_data++;
    map->areas[i].level = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].start_x = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].start_y = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].dir = *level_data; level_data++;
    map->areas[i].x = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].y = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].w = (level_data[0] << 8) | (level_data[1]); level_data += 2;
    map->areas[i].h = (level_data[0] << 8) | (level_data[1]); level_data += 2;
  }

  return map;
}

void free_map(Map *m)
{
  int i;

  if (m->num_objects > 0) {
    for (i = 0; i < m->num_objects; i++) {
      if (m->objects[i].type == OBJECT_TYPE_NPC) {
        if (m->objects[i].num_points > 0) {
          free(m->objects[i].points);
        }
      }
    }
    free(m->objects);
  }

  if (m->num_areas > 0) {
    free(m->areas);
  }

  for (i = 0; i < m->num_tiles; i++) {
    free(m->images[i].img_buf);
    free(m->images[i].img_screen);
  }

  free(m->images);
  free(m->tiles);

  free(m->dat);
  free(m);
}

void clear_map(Map *map, uint16_t tile_num)
{
  unsigned int i;

  for (i = 0; i < map->size; i++)
    map->dat[i] = tile_num;
}

void update_animations_map(Map *m)
{
  unsigned int i;

  for(i=0;i<m->num_tiles;i++)
  {
    switch(m->tiles[i].animated)
    {
      case MAP_ANIM_LOOP :
        animate_tile_loop(m, i);
        break;

      case MAP_ANIM_PINGPONG :
        animate_tile_pingpong(m, i);
        break;

      default:
        break;
    }
  }
}

