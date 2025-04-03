#pragma once
#include <stdint.h>

namespace Display
{
    /**
     * @brief Initialize the display and the LVGL library
     *
     */
    void begin();

    /**
     * @brief Set the brightness of the display
     *
     * @param brightness Brightness value (0-255).
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Test the display by filling it with different colors
     *
     * 
     * @note This function is for testing purposes only and should be removed in production code.
     */
    void test();

    /**
     * @brief Update the speed value on the display
     *
     * @param speed Speed value in m/s. If the speed is not available, set it to MAXFLOAT.
     */
    void updateSpeed(float speed);

    /**
     * @brief Update the RC battery level of the bot on the display
     *
     * @param level Battery level in percentage (0-100).
     */
    void updateRCBatteryLevel(uint8_t level);

    /**
     * @brief Update the bot battery level of the RC on the display
     *
     * @param level Battery level in percentage (0-100).
     */
    void updateBotBatteryLevel(uint8_t level);
} // namespace Display
