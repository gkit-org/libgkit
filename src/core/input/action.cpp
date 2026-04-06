#include "gkit/core/input/action.hpp"
#include "gkit/core/input/input.hpp"


gkit::input::Action::Action(std::string name) : name(std::move(name)) { }


gkit::input::Action::Action(std::string name, const InputChord& chord, bool auto_register) : name(std::move(name)), chord(std::move(chord)) {
    if (auto_register) {
        gkit::Input::instance().register_action(*this);
    }
}
