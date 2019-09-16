use std::ffi::CStr;
use std::fs::File;
use std::{mem, ptr};

use image::ImageDecoder;

fn image_decode<'a, D: ImageDecoder<'a>>(decoder: D, width: *mut u64, height: *mut u64, channels: *mut u8, bits: *mut u16) -> *mut u8 {
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
            ptr
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
        pub extern "C" fn $name(path: *const i8, width: *mut u64, height: *mut u64, channels: *mut u8, bits: *mut u16) -> *mut u8 {
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
pub extern "C" fn image_read_file(path: *const i8, width: *mut u64, height: *mut u64, channels: *mut u8, bits: *mut u16) -> *mut u8 {
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

    match ext {
        Some("tiff") | Some("TIFF") | Some("tif") | Some("TIF") => image_read_tiff(path, width, height, channels, bits),
        Some("jpeg") | Some("JPEG") | Some("jpg") | Some("JPG") => image_read_jpeg(path, width, height, channels, bits),
        Some("png") | Some("PNG")  => image_read_png(path, width, height, channels, bits),
        _ => {
            let mut r = image_read_png(path, width, height, channels, bits);
            if r.is_null() {
               r = image_read_jpeg(path, width, height, channels, bits);
               if r.is_null() {
                    r = image_read_tiff(path, width, height, channels, bits);
               }
            }
            return r;
        }
    }
}

#[no_mangle]
pub extern "C" fn image_free_data(data: *mut u8, width: u64, height: u64, channels: u8, bits: u16) {
    if data.is_null() {
        return;
    }

    let len = (width * height * channels as u64 * bits as u64) as usize;
    let vec = unsafe { Vec::from_raw_parts(data, len, len) };
    mem::drop(vec);
}
