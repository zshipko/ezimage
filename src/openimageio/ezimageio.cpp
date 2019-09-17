#include "../ezimageio.h"

#include <OpenImageIO/imageio.h>

using namespace OIIO;

static bool getType(const ezimageio_type *t, TypeDesc::BASETYPE *dest) {
  TypeDesc::BASETYPE base;
  switch (t->kind) {
  case EZIMAGEIO_INT:
    switch (t->bits) {
    case 8:
      base = TypeDesc::INT8;
      break;
    case 16:
      base = TypeDesc::INT16;
      break;
    case 32:
      base = TypeDesc::INT32;
      break;
    case 64:
      base = TypeDesc::INT64;
      break;
    default:
      return false;
    }
    break;
  case EZIMAGEIO_UINT:
    switch (t->bits) {
    case 8:
      base = TypeDesc::UINT8;
      break;
    case 16:
      base = TypeDesc::UINT16;
      break;
    case 32:
      base = TypeDesc::UINT32;
      break;
    case 64:
      base = TypeDesc::UINT64;
      break;
    default:
      return false;
    }
    break;
  case EZIMAGEIO_FLOAT:
    switch (t->bits) {
    case 16:
      base = TypeDesc::HALF;
      break;
    case 32:
      base = TypeDesc::FLOAT;
      break;
    case 64:
      base = TypeDesc::DOUBLE;
      break;
    default:
      return false;
    }
    break;
  default:
    return false;
  }

  *dest = base;
  return true;
}

extern "C" void *ezimageio_imread(const char *filename, const ezimageio_type *t,
                                  ezimageio_shape *shape) {
  TypeDesc::BASETYPE base;
  if (t != NULL) {
    if (!getType(t, &base)) {
      return NULL;
    }
  } else {
    base = TypeDesc::UINT8;
  }

  auto input = ImageInput::open(filename);
  if (!input) {
    return NULL;
  }

  const ImageSpec &spec = input->spec();
  shape->width = spec.width;
  shape->height = spec.height;
  shape->channels = spec.nchannels;

  if (t) {
    shape->t.bits = t->bits;
    shape->t.kind = t->kind;
  } else {
    shape->t.bits = 8;
    shape->t.kind = EZIMAGEIO_UINT;
  }

  void *data = ::operator new(ezimageio_shape_num_bytes(shape));
  if (data == NULL) {
    input->close();
    return NULL;
  }

  input->read_image(base, data);
  input->close();

  return data;
}

extern "C" bool ezimageio_imwrite(const char *filename, const void *data,
                                  const ezimageio_shape *shape) {
  TypeDesc::BASETYPE base;
  if (!getType(&shape->t, &base)) {
    puts("A");
    return false;
  }

  std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
  if (!out) {
    puts("B");
    return false;
  }

  ImageSpec spec(shape->width, shape->height, shape->channels, base);
  out->open(filename, spec);
  auto written = out->write_image(base, data);
  out->close();
  return written;
}

extern "C" void ezimageio_free(void *data, const ezimageio_shape *shape) {
  if (data == NULL) {
    return;
  }

  (void)shape;
  ::operator delete(data);
}
