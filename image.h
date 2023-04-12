#ifndef _IMAGE_H
#define _IMAGE_H

#include <display.h>
#include <sprite.h>
#include <screen.h>

#define IMAGE_ROT_STEPS 32
#define IMAGE_ROT_SHIFT 3

struct reg_image {
  phrase *img_data;
};

#ifdef INCLUDE_ROT_IMAGE
struct rot_image {
  phrase *rot_data[IMAGE_ROT_STEPS + 1];
};
#endif

struct anim_image {
  int num_frames;
  phrase **anim_data;
};

enum image_type {
  IMAGE_TYPE_REGULAR = 0,
  IMAGE_TYPE_COMPRESSED,
#ifdef INCLUDE_ROT_IMAGE
  IMAGE_TYPE_ROTATE,
#endif
  IMAGE_TYPE_ANIMATION,
  IMAGE_TYPE_COMPRESSED_ANIMATION
};

struct image {
  int w, h;
  depth d;
  enum image_type type;
  union {
    struct reg_image reg_img;
#ifdef INCLUDE_ROT_IMAGE
    struct rot_image rot_img;
#endif
    struct anim_image anim_img;
  } img;
};

struct dynamic_image
{
  short count;
  struct image img;
};

struct dynamic_image_data
{
  int w;
  int h;
  depth d;
  enum image_type type;
  void *data;
  int param;
  void *cdata;
};

extern void *image_gpu_addr;

void init_image_lib(void);
unsigned int load_image(struct image *img, enum image_type type, int w, int h, depth d, void *data, int param);
void deinit_image(struct image *img);
int get_image_param(struct image *img);

__inline__ static phrase* get_image_data(struct image *img, int index)
{
  phrase *data = 0;
 
  if (img->type == IMAGE_TYPE_REGULAR) {
    data = img->img.reg_img.img_data;
  }
  else if (img->type == IMAGE_TYPE_COMPRESSED) {
    data = img->img.reg_img.img_data;
  }
#ifdef INCLUDE_ROT_IMAGE
  else if (img->type == IMAGE_TYPE_ROTATE) {
    data = img->img.rot_img.rot_data[index >> IMAGE_ROT_SHIFT];
  }
#endif
  else if (img->type == IMAGE_TYPE_ANIMATION) {
    data = img->img.anim_img.anim_data[index];
  }
  else if (img->type == IMAGE_TYPE_COMPRESSED_ANIMATION) {
    data = img->img.anim_img.anim_data[index];
  }

  return data;
}

struct dynamic_image* new_dynamic_image_set(int);
unsigned int load_dynamic_image(struct dynamic_image *img, int index, const struct dynamic_image_data *db);
void deinit_dynamic_image(struct dynamic_image *img, int index);

void set_sprite_image(sprite *spr, int x, int y, struct dynamic_image *img, int index, const struct dynamic_image_data *db);

#endif

