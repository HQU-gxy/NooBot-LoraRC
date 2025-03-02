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

    // if (!IMU::begin())
    // {
    //     ESP_LOGE(TAG, "IMU initialization failed");
    //     while (1)
    //         vTaskDelay(100);
    // }

    if (!KeyPad::begin())
    {
        ESP_LOGE(TAG, "KeyPad initialization failed");
        while (1)
            vTaskDelay(100);
    }

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

        // Forward and backward
        if (KeyPad::isPressed(KeyPad::KEY_R))
        {
            if (linear < MAX_LIN_SPEED)
                linear += THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
        }
        else if (KeyPad::isPressed(KeyPad::KEY_L))
        {
            if (linear > -MAX_LIN_SPEED)
                linear -= THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
        }

        if (KeyPad::isPressed(KeyPad::KEY_LEFT))
            angular = 1.0;
        else if (KeyPad::isPressed(KeyPad::KEY_RIGHT))
            angular = -1.0;
        else
            angular = 0;

        LoraLink::sendCommand(linear, angular);
        vTaskDelay(CONTROLLER_PERIOD);
    }
}
