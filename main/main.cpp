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
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 48,
        .min_freq_mhz = 8,
        .light_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    
    if (!IMU::begin())
    {
        ESP_LOGE(TAG, "IMU initialization failed");
        while (1)
            vTaskDelay(100);
    }

    if (!KeyPad::begin())
    {
        ESP_LOGE(TAG, "KeyPad initialization failed");
        while (1)
            vTaskDelay(100);
    }

    LoraLink::setOnLinkLostCallback([]()
                                    { ESP_LOGI(TAG, "Link lost"); });

    LoraLink::setOnConnectedCallback([]()
                                     { ESP_LOGI(TAG, "Link connected"); });
    LoraLink::begin(Serial1, LORA_TX_PIN, LORA_RX_PIN, LORA_LOCK_PIN);
}
