use std::ffi::CStr;

use crate::*;

#[no_mangle]
pub extern "C" fn ezimage_imwrite(path: *const i8, data: *const std::ffi::c_void, shape: *const Shape) -> bool {
    if path.is_null() || data.is_null() || shape.is_null() {
        return false
    }

    let filename = unsafe { CStr::from_ptr(path) };
    let filename = match filename.to_str() {
        Ok(x) => x,
        Err(_) => return false,
    };

    let shape = unsafe {&*shape};

    let size = shape.width * shape.height * shape.channels as u64 * shape.t.bits as u64 / 8;
    let slice = unsafe { std::slice::from_raw_parts(data as *const u8, size as usize) };

    let color = match shape.channels {
        1 => {
            image::ColorType::Gray(shape.t.bits as u8)
        }
        2 => {
            image::ColorType::GrayA(shape.t.bits as u8)
        }
        3 => {
            image::ColorType::RGB(shape.t.bits as u8)
        }
        4 => {
            image::ColorType::RGBA(shape.t.bits as u8)
        }
        _ => return false
    };

    match image::save_buffer(filename, slice, shape.width as u32, shape.height as u32, color) {
        Ok(()) => true,
        Err(_) => false,
    }
}

