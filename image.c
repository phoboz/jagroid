#include <stdlib.h>
#include <jagtypes.h>
#include <jagdefs.h>
#include <display.h>
#include <sprite.h>
#include <display.h>
#include <lz77.h>
#include "image.h"

void *image_gpu_addr;

static screen *img_screen;

extern void *gpu_addr;

void init_image_lib(void)
{
  image_gpu_addr = gpu_addr;
  gpu_addr = lz77_init(gpu_addr);

  img_screen = new_screen();
}

static unsigned int load_reg_image(struct reg_image *img, int w, int h, depth d, phrase *data)
{
  unsigned int result = 0;

  screen *orig_screen = new_screen();

  if (orig_screen != NULL)
  {
    set_simple_screen(d, w, h, orig_screen, data);

    img->img_data = alloc_simple_screen(d, w, h, img_screen);

    if (img->img_data != NULL)
    {
      screen_copy_straight(orig_screen, img_screen, w, h, MODE_S);
      result = 1;
    }

    free(orig_screen);
  }

  return result;
}

static unsigned int load_compressed_image(struct reg_image *img, int w, int h, depth d, phrase *data)
{
  unsigned int result = 0;

  img->img_data = alloc_simple_screen(d, w, h, img_screen);

  if (img->img_data != NULL)
  {
    lz77_unpack(image_gpu_addr, (uint8_t *) data, (uint8_t *) img->img_data);
    result = 1;
  }

  return result;
}

static void deinit_reg_image(struct reg_image *img)
{
  free(img->img_data);
}

#ifdef INCLUDE_ROT_IMAGE
static void load_rot_image(struct rot_image *img, int w, int h, depth d, phrase *data)
{
  screen *orig_screen = new_screen();
  set_simple_screen(d, w, h, orig_screen, data);
  orig_screen->x = w / 2;
  orig_screen->y = h / 2;

  int i;
  for (i = 0; i < IMAGE_ROT_STEPS + 1; i++) {
    img->rot_data[i] = alloc_simple_screen(d, w, h, img_screen);
    img_screen->x = w / 2;
    img_screen->y = h / 2;
    clear_screen(img_screen);

    screen_rotate(orig_screen, img_screen, i << IMAGE_ROT_SHIFT);
  }

  free(orig_screen);
}

static void deinit_rot_image(struct rot_image *img)
{
  int i;

  for (i = 0; i < IMAGE_ROT_STEPS; i++) {
    free(img->rot_data[i]);
  }
}
#endif

static unsigned int load_anim_image(struct anim_image *img, int num_frames, int w, int h, depth d, phrase **data)
{
  unsigned int result = 0;

  img->num_frames = num_frames;
  img->anim_data = (phrase **) malloc(sizeof(phrase*) * num_frames);

  if (img->anim_data != NULL)
  {
    screen *orig_screen = new_screen();

    if (orig_screen != NULL)
    {
      int i;
      for (i = 0; i < num_frames; i++) {
        set_simple_screen(d, w, h, orig_screen, data[i]);

        img->anim_data[i] = alloc_simple_screen(d, w, h, img_screen);

        if (img->anim_data[i] == NULL)
        {
          result = 0;
          break;
        }
        else
        {
          screen_copy_straight(orig_screen, img_screen, w, h, MODE_S);
          result = 1;
        }
      }

      free(orig_screen);
    }
  }

  return result;
}

static void deinit_anim_image(struct anim_image *img)
{
  int i;

  for (i = 0; i < img->num_frames; i++) {
    free(img->anim_data[i]);
  }

  free(img->anim_data);
}

static unsigned int load_compressed_anim_image(struct anim_image *img, int num_frames, int w, int h, depth d, phrase **data)
{
  unsigned int result = 0;

  img->num_frames = num_frames;
  img->anim_data = (phrase **) malloc(sizeof(phrase*) * num_frames);

  if (img->anim_data != NULL)
  {
    int i;
    for (i = 0; i < num_frames; i++) {
      img->anim_data[i] = alloc_simple_screen(d, w, h, img_screen);

      if (img->anim_data[i] == NULL)
      {
        result = 0;
        break;
      }
      else
      {
        lz77_unpack(
          image_gpu_addr,
          (uint8_t *) data[i],
          (uint8_t *) img->anim_data[i]
          );

        result = 1;
      }
    }
  }

  return result;
}

unsigned int load_image(struct image *img, enum image_type type, int w, int h, depth d, void *data, int param)
{
  unsigned int result = 0;

  img->w = w;
  img->h = h;
  img->d = d;
  img->type = type;

  if (type == IMAGE_TYPE_REGULAR) {
    result = load_reg_image(&img->img.reg_img, w, h, d, (phrase *) data);
  }
  else if (type == IMAGE_TYPE_COMPRESSED) {
    result = load_compressed_image(&img->img.reg_img, w, h, d, (phrase *) data);
  }
#ifdef INCLUDE_ROT_IMAGE
  else if (type == IMAGE_TYPE_ROTATE) {
    load_rot_image(&img->img.rot_img, w, h, d, (phrase *) data);
  }
#endif
  else if (type == IMAGE_TYPE_ANIMATION) {
    result = load_anim_image(&img->img.anim_img, param, w, h, d, (phrase **) data);
  }
  else if (type == IMAGE_TYPE_COMPRESSED_ANIMATION) {
    result = load_compressed_anim_image(&img->img.anim_img, param, w, h, d, (phrase **) data);
  }

  return result;
}

void deinit_image(struct image *img)
{
  if (img->type == IMAGE_TYPE_REGULAR) {
    deinit_reg_image(&img->img.reg_img);
  }
  else if (img->type == IMAGE_TYPE_COMPRESSED) {
    deinit_reg_image(&img->img.reg_img);
  }
#ifdef INCLUDE_ROT_IMAGE
  else if (img->type == IMAGE_TYPE_ROTATE) {
    deinit_rot_image(&img->img.rot_img);
  }
#endif
  else if (img->type == IMAGE_TYPE_ANIMATION) {
    deinit_anim_image(&img->img.anim_img);
  }
  else if (img->type == IMAGE_TYPE_COMPRESSED_ANIMATION) {
    deinit_anim_image(&img->img.anim_img);
  }
}

int get_image_param(struct image *img)
{
  int param = 0;

  if (img->type == IMAGE_TYPE_ANIMATION) {
    param = img->img.anim_img.num_frames;
  }
  else if (img->type == IMAGE_TYPE_COMPRESSED_ANIMATION) {
    param = img->img.anim_img.num_frames;
  }

  return param;
}

struct dynamic_image* new_dynamic_image_set(int num_entries)
{
  struct dynamic_image *img;

  img = (struct dynamic_image *) malloc(sizeof(struct dynamic_image) * num_entries);

  memset(img, 0, sizeof(struct dynamic_image) * num_entries);

  return img;
}

unsigned int load_dynamic_image(struct dynamic_image *img, int index, const struct dynamic_image_data *db)
{
  unsigned int result = 1;

  if (img[index].count == 0)
  {
    result = load_image(
      &img[index].img,
      db[index].type,
      db[index].w,
      db[index].h,
      db[index].d,
      db[index].data,
      db[index].param
      );

#if 0
    if (db[index].ncols > 0)
    {
      memcpy(
        (void *) &TOMREGS->clut1[db[index].index],
        db[index].pal,
        db[index].ncols * sizeof(uint16_t)
        );
    }
#endif
  }

  if (result)
  {
    img[index].count++;
  }

  return result;
}

void deinit_dynamic_image(struct dynamic_image *img, int index)
{
  img[index].count--;
 
  if (img[index].count == 0)
  {
    deinit_image(&img[index].img);
  }
}

void set_sprite_image(sprite *spr, int x, int y, struct dynamic_image *img, int index, const struct dynamic_image_data *db)
{
  int w = db[index].w;
  int h = db[index].h;
  depth d = db[index].d;

  set_sprite(spr, w, h, x, y, d, get_image_data(&img[index].img, 0));

  //spr->index = db[index].index >> 1;
}

