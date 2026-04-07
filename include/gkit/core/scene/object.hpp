#pragma once

#include "gkit/core/scene/unit.hpp"
namespace gkit::core::scene {
    /**
     * @brief This is the basic unit of game composition
     */
    class Object : public gkit::core::scene::Unit {
    public:
        Object() = default;
        virtual ~Object();
    };
}