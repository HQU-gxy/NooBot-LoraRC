#pragma once

#include <utility>

namespace KeyPad
{
    // Keypad pins on GPIO expander
    enum KeyPadPin : uint8_t
    {
        KEY_L = 0,
        KEY_R,
        KEY_A,
        KEY_B,
        KEY_RIGHT,
        KEY_DOWN,
        KEY_LEFT,
        KEY_UP
    };
    constexpr KeyPadPin KEY_PINS[] = {KEY_L, KEY_R, KEY_A, KEY_B, KEY_RIGHT, KEY_DOWN, KEY_LEFT, KEY_UP};

    bool begin();
    std::pair<KeyPadPin, bool> getLastKeyEvent();
    bool isPressed(KeyPadPin key);
} // namespace KeyPad
