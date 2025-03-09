#include "Display.h"
#include "TFT_eSPI.h"
#include "Arduino.h"

#include "config.h"

namespace Display
{
    static TFT_eSPI screen;

    constexpr auto BL_PWM_FREQ = 1e4;
    constexpr auto BL_PWM_RES = 8;

    void begin()
    {
        ledcAttach(SCREEN_BL_PIN, BL_PWM_FREQ, BL_PWM_RES);
        screen.init();
        screen.setRotation(1);
        screen.fillScreen(TFT_BLACK);
        setBrightness(128);
    }

    void setBrightness(uint8_t brightness)
    {
        ledcWrite(SCREEN_BL_PIN, brightness);
    }

    void test()
    {
        screen.fillScreen(TFT_RED);
        delay(1000);
        screen.fillScreen(TFT_GREEN);
        delay(1000);
        screen.fillScreen(TFT_BLUE);
        delay(1000);
        screen.fillScreen(TFT_WHITE);
        delay(1000);
    }

} // namespace Display