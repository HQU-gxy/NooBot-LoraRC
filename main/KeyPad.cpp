#include "Arduino.h"
#include "PCF8574.h"

#include "KeyPad.h"
#include "config.h"

#include <unordered_map>

namespace KeyPad
{
    static KeyPadPin lastKeyPin;
    static bool lastKeyState;
    static std::unordered_map<KeyPadPin, bool> keyStateMap;
    static PCF8574 keyPad(0x20, PCF_SDA_PIN, PCF_SCL_PIN);

    constexpr auto ACTIVE_STATE = LOW;

    /**
     * @brief Check the state of the keys and update the last key state and pin
     *
     * @note This function is called by a timer
     */
    static void checkPins(TimerHandle_t)
    {
        for (auto &key : KEY_PINS)
        {
            auto state = isPressed(key);
            if (keyStateMap[key] != state)
            {
                keyStateMap[key] = state;
                lastKeyPin = key;
                lastKeyState = state;
            }
        }
    }

    /**
     * @brief Initialize the PCF8574 and start the timer to check the keys
     * 
     * @return true if the initialization was successful
     */
    bool begin()
    {
        for (auto &key : KEY_PINS)
        {
            keyPad.pinMode(key, INPUT_PULLUP);
        }
        if (!keyPad.begin())
            return false;

        auto keyPadTimer = xTimerCreate("KeyPadTimer", pdMS_TO_TICKS(50), pdTRUE, nullptr, checkPins);
        xTimerStart(keyPadTimer, 0);
        return true;
    }

    /**
     * @brief Get the last key event
     * 
     * @return std::pair<KeyPadPin, bool> the last key pin and its state
     */
    std::pair<KeyPadPin, bool> getLastKeyEvent()
    {
        return {lastKeyPin, lastKeyState};
    }

    /**
     * @brief Get the state of a key
     * 
     * @param key the key to check
     * @return true if the key is pressed
     */
    bool isPressed(KeyPadPin key)
    {
        return keyPad.digitalRead(key) == ACTIVE_STATE;
    }

} // namespace KeyPad
