use std::ffi::CString;
use std::os::raw::{c_int, c_char};


pub struct Window {
    raw_ptr: *mut Gkit_Window
}


impl Window {
    #[allow(unused)]
    fn new(title: String, width: i32, height: i32) -> Self {
        let title_cstr = CString::new(title).expect("filed to cast title to CString");
        return unsafe {
            Self {
                raw_ptr: gkit_window_new(title_cstr.as_ptr(), width, height)
            }
        };
    }

    #[allow(unused)]
    fn show(&self) {
        unsafe {
            gkit_window_show(self.raw_ptr);
        }
    }

    #[allow(unused)]
    fn hide(&self) {
        unsafe {
            gkit_window_hide(self.raw_ptr);
        }
    }
}


impl Drop for Window {
    fn drop(&mut self) -> () {
        unsafe {
            gkit_window_delete(self.raw_ptr);   
        }
    }
}


// ffi //
#[repr(C)]
struct Gkit_Window {
    _data: ()
}

unsafe extern "C" {
    unsafe fn gkit_window_new(title: *const c_char, width: c_int, height: c_int) -> *mut Gkit_Window;      
    unsafe fn gkit_window_delete(win_ptr: *mut Gkit_Window);
    unsafe fn gkit_window_show(win_ptr: *mut Gkit_Window);
    unsafe fn gkit_window_hide(win_ptr: *mut Gkit_Window);
}