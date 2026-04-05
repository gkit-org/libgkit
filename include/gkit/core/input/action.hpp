#pragma once

#include "gkit/core/input/keys.hpp"
#include <string>
#include <variant>

namespace gkit {
    class Input;
}

namespace gkit::input {
    /** @brief A variant type representing different types of input chords */
    using InputChord = std::variant<KeyChord>;

    class Action {
        friend class gkit::Input;

    public:
        /** @brief Default constructor for a new Action object */
        Action() = default;
        /**
         * @brief Construct a new Action object with a name
         * @param name The name of the action
         */
        Action(std::string name) : name(std::move(name)) {};
        ~Action() = default;

    public:
        /**
         * @brief Set the action's input chord
         * @param chord The input chord to set for this action.
         * The type of chord can be @ref KeyChord
         */
        auto set_action(const InputChord& chord) -> void {
            std::visit([this](const auto& input_chord) {
                using ChordT = std::decay_t<decltype(input_chord)>;
                if constexpr (std::is_same_v<KeyChord, ChordT>) {
                    /* this->type = Type::Key; */
                    this->chord = input_chord;
                }
            }, chord);
        }

    private:
        std::string name;
        InputChord chord = KeyChord{};
    }; // class Action
} // namespace gkit::input