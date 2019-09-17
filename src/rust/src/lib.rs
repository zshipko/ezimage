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
    let mut data = vec![0u8; size as usize];
    data.shrink_to_fit();
    let ptr = data.as_mut_ptr();
    std::mem::forget(data);
    return ptr as *mut std::ffi::c_void;

}

#[no_mangle]
pub extern "C" fn ezimage_free(data: *mut u8, shape: *const Shape) {
    if data.is_null() || shape.is_null() {
        return;
    }

    let shape = unsafe { &*shape };
    let len = (shape.width * shape.height * shape.channels as u64 * shape.t.bits as u64) as usize;
    let vec = unsafe { Vec::from_raw_parts(data, len, len) };
    std::mem::drop(vec);
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
