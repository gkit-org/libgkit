#pragma once

#include "gkit/core/input/keys.hpp"
#include <variant>

namespace gkit {
    class Input;
}

namespace gkit::input {
    class Action {
        friend class gkit::Input;

    public:
        enum class Type {
            Key,
            MouseButton,
            GamepadButton,
        };

    using InputChord = std::variant<KeyChord>;

    public:
        Action() = default;
        ~Action() = default;

    public:
        auto get_type() const -> Type { return type; }
        auto set_action(const InputChord& action) -> void { 
            if constexpr (std::is_same_v<KeyChord, typename std::decay_t<decltype(action)>>) {
                type = Type::Key;
                this->action = action;
            } else {
                
            }
        }

    private:
        Type type;
        InputChord action;
    }; // class Action
} // namespace gkit::input