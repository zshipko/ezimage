# ezimageio

A bare-bones C image I/O library with pluggable backends

## Backends

- `stb_image`
  * Based on [stb_image](https://github.com/nothings/stb)
- `rust`
  * Based on [image](https://github.com/PistonDevelopers/image)

## Building

```shell
$ make backend=$BACKEND
```

## API

See `src/ezimageio.h`
