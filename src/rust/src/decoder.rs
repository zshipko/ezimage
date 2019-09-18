use std::ffi::CStr;
use std::fs::File;
use std::{ptr};

use image::{ImageDecoder};

use crate::*;

fn image_decode<'a, D: ImageDecoder<'a>>(decoder: D, width: *mut u64, height: *mut u64, channels: *mut u8, bits: *mut u16) -> *mut std::ffi::c_void {
    let color = decoder.colortype();
    let ch = color.channel_count();
    let b = color.bits_per_pixel() / ch as u16;
    let (w, h) = decoder.dimensions();

    unsafe {
        *width = w;
        *height = h;
        *channels = ch;
        *bits = b;
    }

    let data = match decoder.read_image() {
        Ok(data) => {
            let data = data.into_boxed_slice();
            Box::into_raw(data) as *mut std::ffi::c_void
        }
        Err(_) => {
            return ptr::null_mut();
        }
    };

    return data;
}

fn open_file(path: *const i8) -> Option<File> {
    let path = unsafe { CStr::from_ptr(path) };

    let filename = match path.to_str() {
        Ok(f) => f,
        Err(_) => return None,
    };


    let f = match File::open(filename) {
        Ok(f) => f,
        Err(_) => return None,
    };

    return Some(f)
}

macro_rules! decoder {
    ($name:ident, $dec:expr) => {
        #[no_mangle]
        pub extern "C" fn $name(path: *const i8, width: *mut u64, height: *mut u64, channels: *mut u8, bits: *mut u16) -> *mut std::ffi::c_void {
            let f = match open_file(path) {
                Some(f) => f,
                None => return ptr::null_mut(),
            };

            let f = std::io::BufReader::new(f);

            let decoder = match $dec(f) {
                Ok(d) => d,
                Err(_) => return ptr::null_mut(),
            };

            return image_decode(decoder, width, height, channels, bits);
        }
    }
}

decoder!(image_read_tiff, image::tiff::TIFFDecoder::new);
decoder!(image_read_jpeg, image::jpeg::JPEGDecoder::new);
decoder!(image_read_png, image::png::PNGDecoder::new);
decoder!(image_read_hdr, image::hdr::HDRAdapter::new);
decoder!(image_read_gif, image::gif::Decoder::new);
decoder!(image_read_bmp, image::bmp::BMPDecoder::new);
decoder!(image_read_tga, image::tga::TGADecoder::new);

#[no_mangle]
pub extern "C" fn ezimage_imread(path: *const i8, _t: *const Type, shape: *mut Shape) -> *mut std::ffi::c_void {
    if path.is_null() || shape.is_null() {
        return ptr::null_mut();
    }

    let filename = unsafe { CStr::from_ptr(path) };
    let filename = match filename.to_str() {
        Ok(x) => x,
        Err(_) => return ptr::null_mut(),
    };
    let filename = std::path::PathBuf::from(filename);

    let ext = match filename.extension() {
        Some(x) => x.to_str(),
        None => None,
    };

    let shape = unsafe { &mut *shape };

    shape.t.kind = Kind::UInt;

    match ext {
        Some("tiff") | Some("TIFF") | Some("tif") | Some("TIF") => image_read_tiff(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("jpeg") | Some("JPEG") | Some("jpg") | Some("JPG") => image_read_jpeg(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("png") | Some("PNG")  => image_read_png(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("gif") | Some("GIF")  => image_read_gif(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("hdr") | Some("HDR") => {
            shape.t.kind = Kind::Float;
            image_read_hdr(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits)
        },
        Some("bmp") | Some("BMP") => image_read_bmp(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("tga") | Some("TGA") => image_read_tga(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        _ => {
            let mut r = std::ptr::null_mut();

            let attempt = &[
                image_read_png,
                image_read_jpeg,
                image_read_tiff,
                image_read_hdr,
                image_read_bmp,
                image_read_tga,
            ];
            for f in attempt {
                r = f(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits);
                if !r.is_null(){
                    break
                }
            }
            return r;
        }
    }
}


