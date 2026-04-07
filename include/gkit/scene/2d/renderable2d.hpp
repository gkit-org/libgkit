#pragma once

#include "gkit/scene/2d/object2d.hpp"

namespace gkit::scene::object2d {
    class Renderable2D : public Object2D {
    public:
        Renderable2D() = default;
        virtual ~Renderable2D() = default;

    public:
        virtual auto render() -> void = 0;
    };
}