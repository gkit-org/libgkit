fn main() {
    println!("cargo:rustc-link-search=all=../bin/");
    println!("cargo:rustc-link-lib=static=gkit");
}