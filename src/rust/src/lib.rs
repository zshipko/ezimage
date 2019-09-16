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

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        assert_eq!(2 + 2, 4);
    }
}
