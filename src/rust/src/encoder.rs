use std::ffi::CStr;

use crate::*;

#[no_mangle]
pub extern "C" fn ezimage_imwrite(
    path: *const i8,
    data: *const std::ffi::c_void,
    shape: *const Shape,
) -> bool {
    if path.is_null() || data.is_null() || shape.is_null() {
        return false;
    }

    let filename = unsafe { CStr::from_ptr(path) };
    let filename = match filename.to_str() {
        Ok(x) => x,
        Err(_) => return false,
    };

    let shape = unsafe { &*shape };

    let size = shape.width * shape.height * shape.channels as u64 * shape.t.bits as u64 / 8;
    let slice = unsafe { std::slice::from_raw_parts(data as *const u8, size as usize) };

    let color = match (shape.channels, shape.t.bits) {
        (1, 8) => image::ColorType::L8,
        (1, 16) => image::ColorType::L16,
        (2, 8) => image::ColorType::La8,
        (2, 16) => image::ColorType::La16,
        (3, 8) => image::ColorType::Rgb8,
        (3, 16) => image::ColorType::Rgb16,
        (4, 8) => image::ColorType::Rgba8,
        (4, 16) => image::ColorType::Rgba16,
        _ => return false,
    };

    match image::save_buffer(
        filename,
        slice,
        shape.width as u32,
        shape.height as u32,
        color,
    ) {
        Ok(()) => true,
        Err(_) => false,
    }
}
