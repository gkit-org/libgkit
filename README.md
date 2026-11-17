# libgkit -- A simple library for game development

<div align="center"><img width=200 src="./gkit_logo.png"></div>

> [!IMPORTANT]
>
> - libgkit is still in development. We need your help to improve it.

> [!WARNING]
>
> - This library is used for technical implementation testing now. It is not stable and not recommended to use it in production.

## What is libgkit

libgkit is a simple game library for game development. It is written in C++. We will also support for different script language.

## Contribution

We hope that you can contribute your code into `libgkit`. If you want to contribute your code but don't know where to start, you can visit the [gkit-proposal](https://github.com/gkit-org/gkit-proposal) to know what plan we have and what feature user need. If you have some new idea, you can also visit it to express your idea.

## Building and testing

Requires C++20 with `std::format`, CMake 3.23+, Ninja, and Python with Jinja2.

```sh
git submodule update --init --recursive
cmake --preset x86_64-gcc-debug
cmake --build build/x86_64-gcc-debug --parallel
ctest --test-dir build/x86_64-gcc-debug --output-on-failure
```

For Clang or MSVC, use `x86_64-llvm-debug` or `windows-x86_64-msvc-debug`.

## Scene trees

Use `core::NodeOwner<T>::create()` to build a subtree, then attach it through
`Processer::root().with()` and `Node::edit_children()`. `scene::Scene` is a Node with activation and pause controls.
Configure frame timing and opt-in parallel subtrees before `run()`; use `NodeHandle::post()` for cross-domain operations.
See the [scene tests](test/core/scene/test_processer.cpp) for lifecycle, batching, and concurrency examples.
