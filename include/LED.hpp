#pragma once
#include <Arduino.h>
#include <vector>
#include <utility>

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
    uint16_t _pin;

    // For blink pattern
    uint32_t lastPatternTime = 0;
    uint16_t patternIndex = 0;

    BlinkPattern currentPattern;

public:
    LED(uint16_t pin) : _pin(pin)
    {
        pinMode(_pin, OUTPUT);
    }

    /**
     * @brief Turn on the LED, may be overriden by the blink pattern
     *
     */
    void on()
    {
        digitalWrite(_pin, HIGH);
    }

    /**
     * @brief Turn off the LED, may be overriden by the blink pattern
     *
     */
    void off()
    {
        digitalWrite(_pin, LOW);
    }

    /**
     * @brief Handle the blink pattern, should be called in a loop frequently
     *
     */
    void handleBlink()
    {
        if (currentPattern.pattern.size() == 0)
        {
            return;
        }

        digitalWrite(_pin, currentPattern.pattern[patternIndex].first);

        // Check if it's time to change to the next index
        if (millis() - lastPatternTime >= currentPattern.pattern[patternIndex].second)
        {
            lastPatternTime = millis();
            // Check if the pattern is finished
            if (++patternIndex >= currentPattern.pattern.size())
            {
                if (currentPattern.oneshot)
                {
                    currentPattern.pattern.clear();
                }
                else
                {
                    patternIndex = 0;
                }
            }
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