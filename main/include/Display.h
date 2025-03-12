#pragma once
#include <stdint.h>

namespace Display
{
    void begin();
    void setBrightness(uint8_t brightness);

    void test();

    void updateSpeed(float speed);
    void updateVoltage(float voltage);
} // namespace Display
