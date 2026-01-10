pub struct Application{
    raw_ptr: *mut Gkit_Application
}

impl Application {
    #[allow(unused)]
    pub fn new() -> Self {
        return unsafe {
            Self {
                raw_ptr: gkit_application_new()
            }
        }
    }
}

impl Drop for Application {
    fn drop(&mut self) {
        unsafe {
            gkit_application_delete(self.raw_ptr)
        }
    }
}


// ffi //
#[repr(C)]
struct Gkit_Application {
    _data: ()
}

unsafe extern "C" {
    fn gkit_application_new() -> *mut Gkit_Application;
    fn gkit_application_delete(app_ptr: *mut Gkit_Application);
}
