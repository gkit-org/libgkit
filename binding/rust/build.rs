fn main() {
    println!("cargo:rustc-link-search=native=/usr/lib");
    println!("cargo:rustc-link-search=native=../../bin");
    println!("cargo:rustc-link-lib=static=stdc++");
    println!("cargo:rustc-link-lib=SDL3");
    println!("cargo:rustc-link-lib=static=gkit_static");
}