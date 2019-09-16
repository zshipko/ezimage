use std::ffi::CStr;
use std::fs::File;
use std::{mem, ptr};

use image::ImageDecoder;

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
        Ok(mut data) => {
            let ptr = data.as_mut_ptr();
            data.shrink_to_fit();
            mem::forget(data);
            ptr as *mut std::ffi::c_void
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

#[no_mangle]
pub extern "C" fn ezimageio_imread(path: *const i8, _t: *const Type, shape: *mut Shape) -> *mut std::ffi::c_void {
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

    match ext {
        Some("tiff") | Some("TIFF") | Some("tif") | Some("TIF") => image_read_tiff(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("jpeg") | Some("JPEG") | Some("jpg") | Some("JPG") => image_read_jpeg(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        Some("png") | Some("PNG")  => image_read_png(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits),
        _ => {
            let mut r = image_read_png(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits);
            if r.is_null() {
               r = image_read_jpeg(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits);
               if r.is_null() {
                    r = image_read_tiff(path, &mut shape.width, &mut shape.height, &mut shape.channels, &mut shape.t.bits);
               }
            }
            return r;
        }
    }
}

#[no_mangle]
pub extern "C" fn ezimageio_imwrite(_path: *const i8, _data: *const std::ffi::c_void, _shape: *const Shape) -> bool {
    return false;
}

#[no_mangle]
pub extern "C" fn ezimageio_free(data: *mut u8, shape: *const Shape) {
    if data.is_null() {
        return;
    }

    let shape = unsafe { &*shape };
    let len = (shape.width * shape.height * shape.channels as u64 * shape.t.bits as u64) as usize;
    let vec = unsafe { Vec::from_raw_parts(data, len, len) };
    mem::drop(vec);
}
