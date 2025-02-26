#include "Arduino.h"
#include "PCF8574.h"

#include "KeyPad.h"
#include "config.h"

namespace KeyPad
{
    static KeyPadPin lastKeyPin;
    static bool lastKeyState;
    static PCF8574 keyPad(0x38);

    constexpr auto ACTIVE_STATE = LOW;
    
    static void checkPins(TimerHandle_t)
    {
        for (auto &key : KEY_PINS)
        {
            bool state = (keyPad.digitalRead(key) == ACTIVE_STATE);
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

} // namespace KeyPad
