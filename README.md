# ezimage - bare-bones C image I/O

The goal of `ezimage` is to provide a simple interface for reading and writing images while allowing for different types of images to be supported by selecting a different backend.

## Backends

- `rust`
  * Based on [image](https://github.com/PistonDevelopers/image)
  * Default if `cargo` can be found
  * Read: **PNG**, **JPEG**, **TIFF**, **GIF**, **HDR**, **TGA**, **BMP**
  * Write: **PNG**, **JPEG**, **TIFF(8-bit only)**, **HDR**, **GIF**
- `stb_image`
  * Based on [stb_image](https://github.com/nothings/stb)
  * Default when `cargo` cannot be found
  * Read: **PNG**, **JPEG**, **GIF**, **HDR**, **TGA**, **BMP**, **PSD**
  * Write: **PNG**, **JPEG**, **HDR**
- `openimageio`
  * Based on [OpenImageIO](https://github.com/OpenImageIO/oiio)
  * `libOpenImageIO` must be installed (for example, `libopenimageio-dev` on Debian based distros)
  * Read: **PNG**, **JPEG**, **TIFF**, **GIF**, **HDR**, **TGA**, **BMP**, **RAW**, **OpenEXR**, **DPX**, **PSD**, ...
  * Write: **PNG**, **JPEG**, **TIFF**, **GIF**, **HDR**, **TGA**, **BMP**, **OpenEXR**, **DPX**, ...

## Building

```shell
$ make backend=$BACKEND
```

## API

There are only 3 functions defined by `libezimage`: `ezimage_imread` for reading image, `ezimage_imwrite` for writing images and `ezimage_free` for freeing image data.

- `ezimage_imread(filename, requested_type, output_size) -> pixels`
  * The `requested_type` is a suggestions, however the output data type may differ. Always check the output type to ensure it is the type you're expecting
  * Returns `NULL` when the image can't be read
  * The pixel data returned by `ezimage_imread` should be freed using `ezimage_free`
- `ezimage_imwrite(filename, data, image_size) -> bool`
  * Returns `true` when the image has been written, `false` otherwise
- `ezimage_free(data, image_size)`
  * Free image data allocated by `ezimage_imread`

See `src/ezimage.h`
