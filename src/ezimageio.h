#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define _DEFAULT_SOURCE

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
  EZIMAGEIO_INT,
  EZIMAGEIO_UINT,
  EZIMAGEIO_FLOAT,
} ezimageio_kind;

typedef struct {
  ezimageio_kind kind;
  uint16_t bits;
} ezimageio_type;

typedef struct {
  ezimageio_type t;
  uint64_t width, height;
  uint8_t channels;
} ezimageio_shape;

// void *ezimageio_data_alloc(ezimageio_type *shape);
// void ezimageio_data_free(void *data, ezimageio_type *shape);

void *ezimageio_imread(const char *filename, const ezimageio_type *t,
                       ezimageio_shape *shape);
bool ezimageio_imwrite(const char *filename, const void *data,
                       const ezimageio_shape *shape);
void ezimageio_free(void *data, const ezimageio_shape *shape);

static size_t ezimageio_shape_size(const ezimageio_shape *shape) {
  return (size_t)shape->width * (size_t)shape->height * (size_t)shape->channels;
}

__attribute__((unused)) static size_t
ezimageio_shape_num_bytes(const ezimageio_shape *shape) {
  return ezimageio_shape_size(shape) * (size_t)shape->t.bits / 8;
}

#ifdef __cplusplus
}
#endif
