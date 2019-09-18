pub mod decoder;
pub mod encoder;

#[repr(C)]
pub enum Kind {
    Int,
    UInt,
    Float,
}

#[repr(C)]
pub struct Type {
    kind: Kind,
    bits: u16,
}

#[repr(C)]
pub struct Shape {
    t: Type,
    width: u64,
    height: u64,
    channels: u8,
}

#[no_mangle]
pub extern "C" fn ezimage_alloc(shape: *const Shape) -> *mut std::ffi::c_void {
    if shape.is_null() {
        return std::ptr::null_mut();
    }

    let shape = unsafe { &*shape };
    let size = shape.width * shape.height * shape.channels as u64 * (shape.t.bits as u64 / 8);
    let data = vec![0u8; size as usize].into_boxed_slice();
    return Box::into_raw(data) as *mut std::ffi::c_void;

}

#[no_mangle]
pub extern "C" fn ezimage_free(data: *mut std::ffi::c_void) {
    if data.is_null() {
        return;
    }

    let _data = unsafe { Box::from_raw(data) };
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
