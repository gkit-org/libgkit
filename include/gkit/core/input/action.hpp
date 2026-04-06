#pragma once

#include "gkit/core/input/keys.hpp"
#include "gkit/core/input/mouse.hpp"
#include <string>
#include <variant>

namespace gkit {
    class Input;
}

namespace gkit::input {
    /** @brief A variant type representing different types of input chords */
    using InputChord = std::variant<KeyChord, MouseChord>;

    class Action {
        friend class gkit::Input;

    public:
        /** @brief Default constructor for a new Action object */
        Action() = default;
        /**
         * @brief Construct a new Action object with a name
         * @param name The name of the action
         */
        Action(std::string name);
        ~Action() = default;

    public:
        /**
         * @brief Set the action's input chord
         * @param chord The input chord to set for this action.
         * The type of chord can be @ref KeyChord or @ref MouseChord
         */
        inline auto set_action(const InputChord& chord) -> void {
            this->chord = chord;
        }

        /**
         * @brief Get the name of the action
         * @return The name of the action
         */
        inline auto get_name() const -> const std::string& {
            return this->name;
        }

    private:
        std::string name;
        InputChord chord = KeyChord{};
    }; // class Action
} // namespace gkit::input