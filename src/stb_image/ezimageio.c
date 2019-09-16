#include "../ezimageio.h"
#include "stb.c"

#include <string.h>

void *ezimageio_imread(const char *path, ezimageio_type *t,
                       ezimageio_shape *shape) {
  int w, h, c;
  void *data = NULL;

  if (!t) {
    goto x;
  }

  switch (t->kind) {
  case EZIMAGEIO_UINT:
    if (t->bits == 16) {
      data = stbi_load_16(path, &w, &h, &c, 0);
      shape->t.bits = 16;
      shape->t.kind = EZIMAGEIO_UINT;
    }
  case EZIMAGEIO_FLOAT:
    if (t->bits == 32) {
      data = stbi_loadf(path, &w, &h, &c, 0);
      shape->t.bits = 32;
      shape->t.kind = EZIMAGEIO_FLOAT;
    }
  default:
  x:
    data = stbi_load(path, &w, &h, &c, 0);
    shape->t.bits = 8;
    shape->t.kind = EZIMAGEIO_UINT;
  }

  if (data == NULL) {
    return NULL;
  }

  shape->width = w;
  shape->height = h;
  shape->channels = c;
  return data;
}

static const char *get_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

bool ezimageio_imwrite(const char *path, void *data, ezimageio_shape *shape) {
  const char *ext = get_ext(path);
  if (shape->t.kind == EZIMAGEIO_UINT && shape->t.bits == 8) {
    if (strncasecmp(ext, "png", 3) == 0) {
      return stbi_write_png(path, shape->width, shape->height, shape->channels,
                            data, shape->width * shape->channels) != 0;
    } else if (strncasecmp(ext, "jpg", 3) == 0 ||
               strncasecmp(ext, "jpeg", 4) == 0) {
      return stbi_write_jpg(path, shape->width, shape->height, shape->channels,
                            data, 95) != 0;
    }
  } else if (shape->t.kind == EZIMAGEIO_FLOAT && shape->t.bits == 32) {
    if (strncasecmp(ext, "hdr", 3) == 0) {
      return stbi_write_hdr(path, shape->width, shape->height, shape->channels,
                            data) != 0;
    }
  }

  return false;
}

void ezimageio_free(void *data, ezimageio_shape *shape) {
  stbi_image_free(data);
}
