#include "../src/ezimageio.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 1;
  }

  ezimageio_shape shape;
  void *data = ezimageio_imread(argv[1], NULL, &shape);
  if (data == NULL) {
    return 2;
  }

  printf("%lux%lux%d\n", shape.width, shape.height, (int)shape.channels);

  ezimageio_free(data, &shape);
  return 0;
}
