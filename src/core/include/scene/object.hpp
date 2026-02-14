#pragma once

#include "unit.hpp"
namespace gkit::scene {
    /**
     * @brief This is the basic unit of game composition
     */
    class Object : protected gkit::scene::Unit {
    public:
        Object() = default;
        virtual ~Object();
    };
}