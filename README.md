# ezimageio

A bare-bones C image I/O library with pluggable backends

## Backends

- `rust`
  * Based on [image](https://github.com/PistonDevelopers/image)
  * Default if `cargo` can be found
- `stb_image`
  * Based on [stb_image](https://github.com/nothings/stb)
  * Default when `cargo` cannot be found
- `openimageio`
  * Based on [OpenImageIO](https://github.com/OpenImageIO/oiio)
  * `libOpenImageIO` must be installed (for example, `libopenimageio-dev` on Debian based distros)

## Building

```shell
$ make backend=$BACKEND
```

## API

- `ezimageio_imread(filename, requested_type, output_size) -> pixels`
  * Returns `NULL` when the image can't be read
  * The pixel data returned by `ezimageio_imread` should be freed using `ezimageio_free`
- `ezimageio_imwrite(filename, data, image_size) -> bool`
  * Returns `true` when the image has been written, `false` otherwise
- `ezimageio_free(data, image_size)`
  * Free image data allocated by `ezimageio_imread`

See `src/ezimageio.h`
