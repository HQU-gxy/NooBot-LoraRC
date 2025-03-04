#include <vector>

#include "Arduino.h"
#include "esp_log.h"
#include "Preferences.h"
#include "esp_pm.h"
#include "PCF8574.h"

#include "config.h"
#include "IMU.h"
#include "LoraLink.h"
#include "KeyPad.h"
#include "LED.hpp"

static const LED::BlinkPattern GOOD_PATTERN = {
    .oneshot = true,
    .pattern = {{true, 1}}};

static const LED::BlinkPattern ERROR_PATTERN = {
    .oneshot = false,
    .pattern = {{true, 200}, {false, 200}}};

static const LED::BlinkPattern LINK_LOST_PATTERN = {
    .oneshot = false,
    .pattern = {{true, 100}, {false, 100}, {true, 100}, {false, 700}}};

extern "C" void app_main()
{
    constexpr auto TAG = "app_main";
    initArduino();

    // Configure dynamic frequency scaling
    // automatic light sleep is enabled
    // esp_pm_config_t pm_config = {
    //     .max_freq_mhz = 48,
    //     .min_freq_mhz = 8,
    //     .light_sleep_enable = true,
    // };
    // ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    LED led(LED_PIN);
    led.setBlinkPattern(LINK_LOST_PATTERN);
    if (!IMU::begin())
    {
        ESP_LOGE(TAG, "IMU initialization failed");
        led.setBlinkPattern(ERROR_PATTERN);
        vTaskDelay(portMAX_DELAY);
    }

    if (!KeyPad::begin())
    {
        ESP_LOGE(TAG, "KeyPad initialization failed");
        led.setBlinkPattern(ERROR_PATTERN);
        vTaskDelay(portMAX_DELAY);
    }

    bool connected = true;
    LoraLink::setOnConnectedCallback([&led, &connected]()
                                     {connected = true;
                                         led.setBlinkPattern(GOOD_PATTERN); });

    LoraLink::setOnLinkLostCallback([&led, &connected]()
                                    {connected = false;
                                         led.setBlinkPattern(LINK_LOST_PATTERN); });

    LoraLink::begin(Serial1, LORA_TX_PIN, LORA_RX_PIN, LORA_LOCK_PIN);

    constexpr auto CONTROLLER_PERIOD = 50;
    while (1)
    {
        static float linear = 0;
        static float angular = 0;

        // Naturally decelerate
        if (linear > 0)
            linear -= IDLE_ACC * CONTROLLER_PERIOD / 1000;
        else if (linear < 0)
            linear += IDLE_ACC * CONTROLLER_PERIOD / 1000;

        // Forward/backward control
        auto rPressed = KeyPad::isPressed(KeyPad::KEY_R);
        auto lPressed = KeyPad::isPressed(KeyPad::KEY_L);
        switch (lPressed << 1 | rPressed)
        {
        case 0b01: // Go forward
            if (linear < MAX_LIN_SPEED)
                linear += THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
            break;

        case 0b10: // Go backward
            if (linear > -MAX_LIN_SPEED)
                linear -= THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
            break;

        case 0b11: // Brake
            linear = 0;
            break;

        default:
            break;
        }

        // Angular control
        if (KeyPad::isPressed(KeyPad::KEY_LEFT))
            angular = 1.0;
        else if (KeyPad::isPressed(KeyPad::KEY_RIGHT))
            angular = -1.0;
        else
            angular = 0;

        if (connected)
            LoraLink::sendCommand(linear, angular);
        vTaskDelay(CONTROLLER_PERIOD);
    }
}
