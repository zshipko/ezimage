#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

extern "C" int SaveEXR16(const uint16_t *data, int width, int height,
                         int components, const char *outfilename,
                         const char **err) {
  if ((components == 1) || components == 3 || components == 4) {
    // OK
  } else {
    std::stringstream ss;
    ss << "Unsupported component value : " << components << std::endl;

    tinyexr::SetErrorMessage(ss.str(), err);
    return TINYEXR_ERROR_INVALID_ARGUMENT;
  }

  EXRHeader header;
  InitEXRHeader(&header);

  if ((width < 16) && (height < 16)) {
    // No compression for small image.
    header.compression_type = TINYEXR_COMPRESSIONTYPE_NONE;
  } else {
    header.compression_type = TINYEXR_COMPRESSIONTYPE_ZIP;
  }

  EXRImage image;
  InitEXRImage(&image);

  image.num_channels = components;

  std::vector<uint16_t> images[4];

  if (components == 1) {
    images[0].resize(static_cast<size_t>(width * height));
    memcpy(images[0].data(), data, sizeof(float) * size_t(width * height));
  } else {
    images[0].resize(static_cast<size_t>(width * height));
    images[1].resize(static_cast<size_t>(width * height));
    images[2].resize(static_cast<size_t>(width * height));
    images[3].resize(static_cast<size_t>(width * height));

    // Split RGB(A)RGB(A)RGB(A)... into R, G and B(and A) layers
    for (size_t i = 0; i < static_cast<size_t>(width * height); i++) {
      images[0][i] = data[static_cast<size_t>(components) * i + 0];
      images[1][i] = data[static_cast<size_t>(components) * i + 1];
      images[2][i] = data[static_cast<size_t>(components) * i + 2];
      if (components == 4) {
        images[3][i] = data[static_cast<size_t>(components) * i + 3];
      }
    }
  }

  uint16_t *image_ptr[4] = {0, 0, 0, 0};
  if (components == 4) {
    image_ptr[0] = &(images[3].at(0)); // A
    image_ptr[1] = &(images[2].at(0)); // B
    image_ptr[2] = &(images[1].at(0)); // G
    image_ptr[3] = &(images[0].at(0)); // R
  } else if (components == 3) {
    image_ptr[0] = &(images[2].at(0)); // B
    image_ptr[1] = &(images[1].at(0)); // G
    image_ptr[2] = &(images[0].at(0)); // R
  } else if (components == 1) {
    image_ptr[0] = &(images[0].at(0)); // A
  }

  image.images = reinterpret_cast<unsigned char **>(image_ptr);
  image.width = width;
  image.height = height;

  header.num_channels = components;
  header.channels = static_cast<EXRChannelInfo *>(malloc(
      sizeof(EXRChannelInfo) * static_cast<size_t>(header.num_channels)));
  // Must be (A)BGR order, since most of EXR viewers expect this channel order.
  if (components == 4) {
#ifdef _MSC_VER
    strncpy_s(header.channels[0].name, "A", 255);
    strncpy_s(header.channels[1].name, "B", 255);
    strncpy_s(header.channels[2].name, "G", 255);
    strncpy_s(header.channels[3].name, "R", 255);
#else
    strncpy(header.channels[0].name, "A", 255);
    strncpy(header.channels[1].name, "B", 255);
    strncpy(header.channels[2].name, "G", 255);
    strncpy(header.channels[3].name, "R", 255);
#endif
    header.channels[0].name[strlen("A")] = '\0';
    header.channels[1].name[strlen("B")] = '\0';
    header.channels[2].name[strlen("G")] = '\0';
    header.channels[3].name[strlen("R")] = '\0';
  } else if (components == 3) {
#ifdef _MSC_VER
    strncpy_s(header.channels[0].name, "B", 255);
    strncpy_s(header.channels[1].name, "G", 255);
    strncpy_s(header.channels[2].name, "R", 255);
#else
    strncpy(header.channels[0].name, "B", 255);
    strncpy(header.channels[1].name, "G", 255);
    strncpy(header.channels[2].name, "R", 255);
#endif
    header.channels[0].name[strlen("B")] = '\0';
    header.channels[1].name[strlen("G")] = '\0';
    header.channels[2].name[strlen("R")] = '\0';
  } else {
#ifdef _MSC_VER
    strncpy_s(header.channels[0].name, "A", 255);
#else
    strncpy(header.channels[0].name, "A", 255);
#endif
    header.channels[0].name[strlen("A")] = '\0';
  }

  header.pixel_types = static_cast<int *>(
      malloc(sizeof(int) * static_cast<size_t>(header.num_channels)));
  header.requested_pixel_types = static_cast<int *>(
      malloc(sizeof(int) * static_cast<size_t>(header.num_channels)));
  for (int i = 0; i < header.num_channels; i++) {
    header.pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of input image

    header.requested_pixel_types[i] =
        TINYEXR_PIXELTYPE_HALF; // save with half(fp16) pixel format
  }

  int ret = SaveEXRImageToFile(&image, &header, outfilename, err);

  free(header.channels);
  free(header.pixel_types);
  free(header.requested_pixel_types);

  return ret;
}
