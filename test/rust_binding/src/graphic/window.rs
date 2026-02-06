#[test]
fn test_graphic_window() {
    use std::time::Duration;

    use gkit::Application;
    use gkit::graphic::Window;

    #[allow(unused)]
    let app = Application::new();
    let win_title = "Hello, World!".to_string();
    let win = Window::new(win_title, 640, 480);
    win.show();
    std::thread::sleep(Duration::from_millis(500));
}