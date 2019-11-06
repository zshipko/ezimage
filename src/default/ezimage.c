#include "../ezimage.h"
#include "stb.c"
#include "tinyexr.h"

#include <string.h>

#include <stdlib.h>
#include <string.h>
#include <tiffio.h>

static void tiffErrorHandler(const char *module, const char *fmt, va_list ap) {
  // ignore
}

static bool tiffInitialized = false;

static void tiffInit() {
  TIFFSetWarningHandler(tiffErrorHandler);
  TIFFSetErrorHandler(tiffErrorHandler);
  tiffInitialized = true;
}

static bool saveTIFF(const char *filename, const void *data,
                     const ezimage_shape *shape) {
  if (!tiffInitialized) {
    tiffInit();
  }

  TIFF *tif = TIFFOpen(filename, "w");
  if (!tif) {
    return false;
  }

  TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  if (shape->channels == 1) {
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  } else {
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  }
  TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, shape->width);
  TIFFSetField(tif, TIFFTAG_IMAGELENGTH, shape->height);
  TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, shape->t.bits);
  TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, shape->channels);

  if (shape->t.kind == EZIMAGE_FLOAT) {
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
  } else if (shape->t.kind == EZIMAGE_INT) {
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT);
  } else {
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
  }

  uint64_t offs =
      (shape->t.bits / 8) * (uint64_t)shape->channels * shape->width;

  uint32_t j;
  for (j = 0; j < shape->height; j++) {
    if (TIFFWriteScanline(tif, (void *)(data + offs * j), j, 0) < 0) {
      TIFFClose(tif);
      remove(filename);
      return false;
    }
  }

  TIFFClose(tif);
  return true;
}

static void *openTIFF(const char *filename, ezimage_shape *shape) {

  if (!tiffInitialized) {
    tiffInit();
  }

  TIFF *tif = TIFFOpen(filename, "r");
  if (!tif) {
    return NULL;
  }

  void *data = NULL;
  uint32_t w, h;
  uint16_t depth, channels, fmt;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &depth);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &channels);
  TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &fmt);

  shape->t.bits = depth;
  shape->width = w;
  shape->height = h;
  shape->channels = channels;

  if (fmt == SAMPLEFORMAT_IEEEFP) {
    shape->t.kind = EZIMAGE_FLOAT;
  } else if (fmt == SAMPLEFORMAT_INT) {
    shape->t.kind = EZIMAGE_INT;
  } else if (fmt == SAMPLEFORMAT_UINT) {
    shape->t.kind = EZIMAGE_UINT;
  } else {
    goto error0;
  }

  data = ezimage_alloc(shape);
  if (!data) {
    goto error0;
  }

  tstrip_t strip;
  tdata_t *buf = malloc(TIFFStripSize(tif));
  if (!buf) {
    goto error0;
  }

  for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++) {
    tsize_t n = TIFFReadEncodedStrip(tif, strip, buf, -1);
    if (n <= 0) {
      goto error1;
    }

    memcpy(data + TIFFStripSize(tif) * strip, buf, n);
  }

  free(buf);
  TIFFClose(tif);
  return data;

error1:
  free(buf);
error0:
  if (data)
    ezimage_free(data);
  TIFFClose(tif);
  return NULL;
}

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
  case EZIMAGE_UINT:
    if (t->bits == 16) {
      data = stbi_load_16(path, &w, &h, &c, 0);
      shape->t.bits = 16;
      shape->t.kind = EZIMAGE_UINT;
    }
    if (data == NULL) {
      goto read_float;
    }
    break;
  case EZIMAGE_FLOAT:
  read_float:
    if (t->bits == 32) {
      data = stbi_loadf(path, &w, &h, &c, 0);
      shape->t.bits = 32;
      shape->t.kind = EZIMAGE_FLOAT;
    }
    if (data == NULL) {
      goto read_uint;
    }
    break;
  default:
  read_uint:
    data = stbi_load(path, &w, &h, &c, 0);
    shape->t.bits = 8;
    shape->t.kind = EZIMAGE_UINT;
  }

  if (data == NULL) {
    data = openTIFF(path, shape);
    if (data != NULL) {
      return data;
    }

    const char *err = NULL;
    if (LoadEXR((float **)&data, &w, &h, path, &err) != TINYEXR_SUCCESS) {
      FreeEXRErrorMessage(err);
      return NULL;
    }
    shape->t.bits = 32;
    shape->t.kind = EZIMAGE_FLOAT;
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

  if (strncasecmp(ext, "tif", 3) == 0 || strncasecmp(ext, "tiff", 4) == 0) {
    return saveTIFF(path, data, shape);
  } else if (shape->t.kind == EZIMAGE_UINT && shape->t.bits == 8) {
    if (strncasecmp(ext, "png", 3) == 0) {
      return stbi_write_png(path, shape->width, shape->height, shape->channels,
                            data, shape->width * shape->channels) != 0;
    } else if (strncasecmp(ext, "jpg", 3) == 0 ||
               strncasecmp(ext, "jpeg", 4) == 0) {
      return stbi_write_jpg(path, shape->width, shape->height, shape->channels,
                            data, 95) != 0;
    }
  } else if (shape->t.kind == EZIMAGE_FLOAT && shape->t.bits == 32) {
    if (strncasecmp(ext, "hdr", 3) == 0) {
      return stbi_write_hdr(path, shape->width, shape->height, shape->channels,
                            data) != 0;
    } else if (strncasecmp(ext, "exr", 3) == 0) {
      return SaveEXR(data, shape->width, shape->height, shape->channels, 0,
                     path, NULL) >= 0;
    }
  } else if (shape->t.kind == EZIMAGE_FLOAT && shape->t.bits == 16) {
    if (strncasecmp(ext, "exr", 3) == 0) {
      return SaveEXR(data, shape->width, shape->height, shape->channels, 1,
                     path, NULL) >= 0;
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

void ezimage_free(void *data) {
  if (data == NULL) {
    return;
  }

  stbi_image_free(data);
}
