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

Create nodes with `UniqueObject::create<T>()`. Moving the owner into a parent transfers the subtree.
Keep its `ObjectId` to access it later. `with<T>()` returns false for an expired ID or a mismatched type.

```cpp
gkit::Processer loop;
loop.set_initialize_sdl(false).set_target_frame_rate(120);
auto player = gkit::core::UniqueObject::create<gkit::scene::Object2D>();
auto id = player.get_id();
loop.root().add_child("player", std::move(player));
auto found = id.with<gkit::scene::Object2D>([](auto& node) { node.set_rotation(0.5f); });
```

Edits complete immediately on stopped trees and at frame boundaries during a run. Use the optional
completion callback to handle errors or recover an unsuccessful addition. `detach_child()` delivers the
subtree owner to its completion callback. Child lists and transforms are returned by value.

Node properties support concurrent reads and writes. Synchronize custom node fields or use `Node::post()`
to run a callback on the coordinator. Each `run()` validates the latest settings before initializing SDL
or starting workers. Changes made during a run apply to the next run.
