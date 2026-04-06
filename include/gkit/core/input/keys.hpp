#pragma once

#include <cstdint>
#include <vector>

namespace gkit::input {
    enum class Key : std::uint32_t {
        Unknown = 0,

        // Alphabet keys (4-29)
        A = 4,
        B = 5,
        C = 6,
        D = 7,
        E = 8,
        F = 9,
        G = 10,
        H = 11,
        I = 12,
        J = 13,
        K = 14,
        L = 15,
        M = 16,
        N = 17,
        O = 18,
        P = 19,
        Q = 20,
        R = 21,
        S = 22,
        T = 23,
        U = 24,
        V = 25,
        W = 26,
        X = 27,
        Y = 28,
        Z = 29,

        // Number row (30-39)
        Num1 = 30,
        Num2 = 31,
        Num3 = 32,
        Num4 = 33,
        Num5 = 34,
        Num6 = 35,
        Num7 = 36,
        Num8 = 37,
        Num9 = 38,
        Num0 = 39,

        // Control keys (40-57)
        Return      = 40,
        Escape      = 41,
        Backspace   = 42,
        Tab         = 43,
        Space       = 44,
        Minus       = 45,
        Equals      = 46,
        LeftBracket  = 47,
        RightBracket = 48,
        Backslash    = 49,
        Semicolon    = 51,
        Apostrophe   = 52,
        Grave        = 53, // backtick / tilde key
        Comma        = 54,
        Period       = 55,
        Slash        = 56,
        CapsLock     = 57,

        // Function keys (58-69)
        F1  = 58,
        F2  = 59,
        F3  = 60,
        F4  = 61,
        F5  = 62,
        F6  = 63,
        F7  = 64,
        F8  = 65,
        F9  = 66,
        F10 = 67,
        F11 = 68,
        F12 = 69,

        // Extended function keys (104-115)
        F13 = 104,
        F14 = 105,
        F15 = 106,
        F16 = 107,
        F17 = 108,
        F18 = 109,
        F19 = 110,
        F20 = 111,
        F21 = 112,
        F22 = 113,
        F23 = 114,
        F24 = 115,

        // Screen / print (70-72)
        PrintScreen = 70,
        ScrollLock  = 71,
        Pause       = 72,

        // Navigation keys (73-82)
        Insert   = 73,
        Home     = 74,
        PageUp   = 75,
        Delete   = 76,
        End      = 77,
        PageDown = 78,
        Right    = 79,
        Left     = 80,
        Down     = 81,
        Up       = 82,

        // Numpad (83-103)
        NumLock       = 83,
        KpDivide      = 84,
        KpMultiply    = 85,
        KpMinus       = 86,
        KpPlus        = 87,
        KpEnter       = 88,
        Kp1           = 89,
        Kp2           = 90,
        Kp3           = 91,
        Kp4           = 92,
        Kp5           = 93,
        Kp6           = 94,
        Kp7           = 95,
        Kp8           = 96,
        Kp9           = 97,
        Kp0           = 98,
        KpPeriod      = 99,
        KpEquals      = 103,

        // Modifier keys (224-231)
        LCtrl  = 224,
        LShift = 225,
        LAlt   = 226,
        LGui   = 227, // Super / Windows / Command
        RCtrl  = 228,
        RShift = 229,
        RAlt   = 230,
        RGui   = 231,

        // Application / system (101-102, 118-129)
        Application  = 101,
        Power        = 102,
        Menu         = 118,
        Mute         = 127,
        VolumeUp     = 128,
        VolumeDown   = 129,

        // International keys (135-139)
        International1 = 135,
        International2 = 136,
        International3 = 137, // Yen
        International4 = 138,
        International5 = 139,

        // Language keys (144-148)
        Lang1 = 144, // Hangul/English toggle
        Lang2 = 145, // Hanja
        Lang3 = 146, // Katakana
        Lang4 = 147, // Hiragana
        Lang5 = 148, // Zenkaku/Hankaku

        // Media keys (258-271)
        Sleep                = 258,
        Wake                 = 259,
        MediaPlay            = 262,
        MediaPause           = 263,
        MediaRecord          = 264,
        MediaFastForward     = 265,
        MediaRewind          = 266,
        MediaNextTrack       = 267,
        MediaPreviousTrack   = 268,
        MediaStop            = 269,
        MediaPlayPause       = 271,

        // Application control (273-290)
        AcNew     = 273,
        AcOpen    = 274,
        AcClose   = 275,
        AcExit    = 276,
        AcSave    = 277,
        AcSearch  = 280,
        AcHome    = 281,
        AcBack    = 282,
        AcForward = 283,

        // Mode
        Mode = 257,

        KeyCount = 512
    }; // enum class Key

    enum class Mod : std::uint32_t {
        None    = 0x0000,
        LShift  = 0x0001,
        RShift  = 0x0002,
        LCtrl   = 0x0040,
        RCtrl   = 0x0080,
        LAlt    = 0x0100,
        RAlt    = 0x0200,
        LGui    = 0x0400,
        RGui    = 0x0800,
        Num     = 0x1000,
        Caps    = 0x2000,
        Mode    = 0x4000,

        // Convenience combinations
        Ctrl  = LCtrl  | RCtrl,
        Shift = LShift | RShift,
        Alt   = LAlt   | RAlt,
        Gui   = LGui   | RGui,
    }; // enum class Mod

    constexpr auto operator|(Mod lhs, Mod rhs) noexcept -> Mod {
        return static_cast<Mod>(
            static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs)
        );
    }

    constexpr auto operator&(Mod lhs, Mod rhs) noexcept -> Mod {
        return static_cast<Mod>(
            static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs)
        );
    }

    constexpr auto operator^(Mod lhs, Mod rhs) noexcept -> Mod {
        return static_cast<Mod>(
            static_cast<std::uint32_t>(lhs) ^ static_cast<std::uint32_t>(rhs)
        );
    }

    constexpr auto operator~(Mod val) noexcept -> Mod {
        return static_cast<Mod>(~static_cast<std::uint32_t>(val));
    }

    constexpr auto operator==(Mod lhs, Mod rhs) noexcept -> bool {
        return static_cast<std::uint32_t>(lhs) == static_cast<std::uint32_t>(rhs);
    }

    constexpr auto operator!=(Mod lhs, Mod rhs) noexcept -> bool {
        return static_cast<std::uint32_t>(lhs) != static_cast<std::uint32_t>(rhs);
    }

    struct KeyChord {
        std::vector<Key> keys;
        uint32_t modifiers;
    };
} // namespace gkit::input
