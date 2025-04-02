#pragma once
#include <stdint.h>

namespace Display
{
    void begin();
    void setBrightness(uint8_t brightness);

    void test();

    void updateSpeed(float speed);

    void updateBotBatteryLevel(uint8_t level);
    
    void updateRCBatteryLevel(uint8_t level);
} // namespace Display
