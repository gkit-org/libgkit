#pragma once

#include "utils/Singleton.hpp"
#include "input/keydef.hpp"
#include <SDL3/SDL_keycode.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace gkit {

    class Input final : protected utils::Singleton<Input> {
    public:
        Input()  noexcept;
        ~Input() noexcept = default;


    public:
        /**
        * 
        */
        void bind_input_event(std::string event_name, std::vector<KeyCombination>& keys)  noexcept;

        /**
        *
        */
        void bind_input_event(std::string event_name, std::vector<KeyCombination>&& keys) noexcept;

        /**
        * 
        */
        bool is_event_pressed(std::string event_name) noexcept;


    private:
        std::unordered_map<std::string, int> input_event_cache;
        std::unordered_map<std::string, std::vector<KeyCombination>> input_map;
    }; // class Input

} // namespace gkit