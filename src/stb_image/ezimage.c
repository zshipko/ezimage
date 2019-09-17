#include "../ezimage.h"
#include "stb.c"

#include <string.h>

void *ezimage_imread(const char *path, const ezimage_type *t,
                     ezimage_shape *shape) {
  if (path == NULL || shape == NULL) {
    return NULL;
  }

  int w, h, c;
  void *data = NULL;

  if (!t) {
    goto read_uint;
  }

  switch (t->kind) {
  case EZIMAGEIO_UINT:
    if (t->bits == 16) {
      data = stbi_load_16(path, &w, &h, &c, 0);
      shape->t.bits = 16;
      shape->t.kind = EZIMAGEIO_UINT;
    }
    break;
  case EZIMAGEIO_FLOAT:
    if (t->bits == 32) {
      data = stbi_loadf(path, &w, &h, &c, 0);
      shape->t.bits = 32;
      shape->t.kind = EZIMAGEIO_FLOAT;
    }
    break;
  default:
  read_uint:
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

bool ezimage_imwrite(const char *path, const void *data,
                     const ezimage_shape *shape) {
  if (path == NULL || data == NULL || shape == NULL) {
    return false;
  }

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

void *ezimage_alloc(const ezimage_shape *shape) {
  if (shape == NULL) {
    return NULL;
  }

  size_t b = ezimage_shape_num_bytes(shape);
  void *p = STBI_MALLOC(b);
  bzero(p, b);
  return p;
}

void ezimage_free(void *data, const ezimage_shape *shape) {
  if (data == NULL) {
    return;
  }

  (void)shape;
  stbi_image_free(data);
}
