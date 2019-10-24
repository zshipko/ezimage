#ifdef EZIMAGE_MALLOC
#define STBI_MALLOC EZIMAGE_MALLOC
#endif

#ifdef EZIMAGE_FREE
#define STBI_FREE EZIMAGE_FREE
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
