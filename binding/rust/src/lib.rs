pub(crate) mod application {
    pub mod application;
}
pub use application::application::Application;

pub mod graphic {
    pub(crate) mod window;
    pub use window::Window;
}