#pragma once

#include <Arduino.h>
#include <vector>

class LED
{
public:
    /**
     * @brief A struct to define a blink pattern
     *
     * @param oneshot If true, the pattern will only run once
     * @param pattern A vector of pairs of value and duration
     */
    struct BlinkPattern
    {
        bool oneshot = false;
        std::vector<std::pair<bool, uint16_t>> pattern; // value, duration
    };

private:
    uint8_t _pin;

    static bool initialized;
    static std::vector<LED *> leds;

    // For blink pattern
    uint32_t lastPatternTime = 0;
    uint16_t patternIndex = 0;

    BlinkPattern currentPattern;

    /**
     * @brief Handle the blink pattern
     *
     */

    static void timerHandler(TimerHandle_t)
    {
        for (auto led : leds)
        {
            if (led->currentPattern.pattern.size() == 0)
            {
                return;
            }

            digitalWrite(led->_pin, led->currentPattern.pattern[led->patternIndex].first);

            // Check if it's time to change to the next index
            if (millis() - led->lastPatternTime >= led->currentPattern.pattern[led->patternIndex].second)
            {
                led->lastPatternTime = millis();
                // Check if the pattern is finished
                if (++led->patternIndex >= led->currentPattern.pattern.size())
                {
                    if (led->currentPattern.oneshot)
                    {
                        led->currentPattern.pattern.clear();
                    }
                    else
                    {
                        led->patternIndex = 0;
                    }
                }
            }
        }
    }

public:
    LED(uint8_t pin) : _pin(pin)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, 0);
        leds.push_back(this);
        if (!LED::initialized)
        {
            LED::initialized = true;
            auto timer = xTimerCreate("LEDTimer", pdMS_TO_TICKS(10), pdTRUE, nullptr, timerHandler);
            xTimerStart(timer, 0);
        }
    }

    /**
     * @brief Set the blink pattern
     *
     * @param pattern The pattern to set
     */
    void setBlinkPattern(const BlinkPattern &pattern)
    {
        currentPattern = pattern;
        lastPatternTime = millis();
        patternIndex = 0;
    }
};