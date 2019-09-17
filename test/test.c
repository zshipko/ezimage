#include "../src/ezimage.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 1;
  }

  ezimage_shape shape;
  void *data = ezimage_imread(argv[1], NULL, &shape);
  if (data == NULL) {
    return 2;
  }

  printf("%lux%lux%d : %d_%d\n", shape.width, shape.height, (int)shape.channels,
         shape.t.bits, shape.t.kind);

  assert(ezimage_imwrite("out.jpg", data, &shape));
  ezimage_free(data, &shape);
  putchar('\n');
  return 0;
}
