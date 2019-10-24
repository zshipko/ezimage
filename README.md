# ezimage - bare-bones C image I/O

The goal of `ezimage` is to provide a simple interface for reading and writing images while allowing for different types of images to be supported by selecting a different backend.

## Backends

- `stb_image`
  * Requires `libtiff`
  * Based on [stb_image](https://github.com/nothings/stb), [TinyEXR](https://github.com/syoyo/tinyexr) and libtiff
  * Read: **PNG**, **JPEG**, **GIF**, **HDR**, **TGA**, **BMP**, **PSD**, **EXR**, **TIFF**
  * Write: **PNG**, **JPEG**, **HDR**, **EXR**, **TIFF**
- `rust`
  * Based on [image](https://github.com/image-rs/image)
  * Read: **PNG**, **JPEG**, **TIFF**, **GIF**, **HDR**, **TGA**, **BMP**
  * Write: **PNG**, **JPEG**, **TIFF(8-bit only)**, **HDR**, **GIF**
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

There are only 4 functions defined by `libezimage`: `ezimage_imread` for reading image, `ezimage_imwrite` for writing images, `ezimage_alloc` for allocating new image data and `ezimage_free` for freeing image data.

`ezimage_alloc` and `ezimage_free` are implemented by each backend depending on their memory needs. For example, in Rust, this will return a pointer to `Vec` data, which will later be dropped when calling `ezimage_free`, but in most cases they simply wrap `malloc`/`free`.

- `ezimage_imread(filename, requested_type, output_size) -> pixels`
  * The `requested_type` is a suggestions, however the output data type may differ. Always check the output type to ensure it is the type you're expecting
  * Returns `NULL` when the image can't be read
  * The pixel data returned by `ezimage_imread` should be freed using `ezimage_free`
- `ezimage_imwrite(filename, data, image_size) -> bool`
  * Returns `true` when the image has been written, `false` otherwise
- `ezimage_alloc(shape)`
  * Returns new image data for the corresponding shape
- `ezimage_free(data, image_size)`
  * Free image data allocated by `ezimage_imread`

See `src/ezimage.h`
