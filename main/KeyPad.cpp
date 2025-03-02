#include "Arduino.h"
#include "PCF8574.h"

#include "KeyPad.h"
#include "config.h"

namespace KeyPad
{
    static KeyPadPin lastKeyPin;
    static bool lastKeyState;
    static PCF8574 keyPad(0x20, PCF_SDA_PIN, PCF_SCL_PIN);

    constexpr auto ACTIVE_STATE = LOW;

    static void checkPins(TimerHandle_t)
    {
        for (auto &key : KEY_PINS)
        {
            auto state = isPressed(key);
            if (state != lastKeyState)
            {
                lastKeyState = state;
                lastKeyPin = key;
            }
        }
    }

    bool begin()
    {
        for (auto &key : KEY_PINS)
        {
            keyPad.pinMode(key, INPUT_PULLUP);
        }
        return keyPad.begin();
        auto keyPadTimer = xTimerCreate("KeyPadTimer", pdMS_TO_TICKS(50), pdTRUE, nullptr, checkPins);
        xTimerStart(keyPadTimer, 0);
    }

    std::pair<KeyPadPin, bool> getLastKeyEvent()
    {
        return {lastKeyPin, lastKeyState};
    }

    bool isPressed(KeyPadPin key)
    {
        return keyPad.digitalRead(key) == ACTIVE_STATE;
    }

} // namespace KeyPad
