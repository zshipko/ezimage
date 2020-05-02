#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
  EZIMAGE_INT,
  EZIMAGE_UINT,
  EZIMAGE_FLOAT,
} ezimage_kind;

typedef struct {
  ezimage_kind kind;
  uint16_t bits;
} ezimage_type;

typedef struct {
  ezimage_type t;
  uint64_t width, height;
  uint8_t channels;
} ezimage_shape;

void *ezimage_imread(const char *filename, const ezimage_type *t,
                     ezimage_shape *shape);
bool ezimage_imwrite(const char *filename, const void *data,
                     const ezimage_shape *shape);

void *ezimage_alloc(const ezimage_shape *shape);
void ezimage_free(void *data);

static size_t ezimage_shape_size(const ezimage_shape *shape) {
  return (size_t)shape->width * (size_t)shape->height * (size_t)shape->channels;
}

__attribute__((unused)) static size_t
ezimage_shape_num_bytes(const ezimage_shape *shape) {
  return ezimage_shape_size(shape) * ((size_t)shape->t.bits / 8);
}

#ifdef __cplusplus
}
#endif
