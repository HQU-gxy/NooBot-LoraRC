#include "LoraLink.h"

namespace LoraLink
{
    constexpr auto CHECK_CONN_PERIOD = 50; // ms
    constexpr auto MAX_UNLOCK_COUNT = 10;

    constexpr auto TAG = "LoraLink";

    static bool linkLost = false;

    static HardwareSerial *LoraSerial;
    static uint16_t lockPin;

    callback_function_t onLinkLostCallback;
    callback_function_t onConnectedCallback;

    struct __attribute__((packed)) LoraLinkCommand
    {
        const uint8_t header = 0x12;
        float targetLinear;  // Linear speed in m/s
        float targetAngular; // Angular speed in rad/s
        uint8_t checksum;
    };

    static void checkConnection(TimerHandle_t)
    {
        static uint8_t unlockCount = 0;

        if (!digitalRead(lockPin))
        {
            if (++unlockCount >= MAX_UNLOCK_COUNT)
            {
                unlockCount = MAX_UNLOCK_COUNT;
                if (!linkLost)
                {
                    linkLost = true;
                    ESP_LOGW(TAG, "Link lost");
                    if (onLinkLostCallback)
                    {
                        onLinkLostCallback();
                    }
                }
            }
        }
        else
        {
            unlockCount = 0;
            linkLost = false;
            ESP_LOGI(TAG, "Connected");
            if (onConnectedCallback)
            {
                onConnectedCallback();
            }
        }
    }

    void begin(HardwareSerial &ser, uint16_t pinTx, uint16_t pinRx, uint16_t pinLock)
    {
        lockPin = pinLock;
        LoraSerial = &ser;
        LoraSerial->setPins(pinRx, pinTx);
        LoraSerial->begin(38400);
        LoraSerial->setTimeout(20);

        auto checkConnectionTimer = xTimerCreate("checkConnectionTimer", pdMS_TO_TICKS(CHECK_CONN_PERIOD), pdTRUE, nullptr, checkConnection);
        xTimerStart(checkConnectionTimer, 0);
    }

    void sendCommand(float linear, float angular)
    {
        LoraLinkCommand cmd = {
            .targetLinear = linear,
            .targetAngular = angular,
            .checksum = 0,
        };

        uint8_t *cmdPtr = reinterpret_cast<uint8_t *>(&cmd);
        for (size_t i = 0; i < sizeof(cmd) - 1; i++)
        {
            cmd.checksum ^= cmdPtr[i];
        }

        LoraSerial->write(cmdPtr, sizeof(cmd));
    }

    bool isLinkLost()
    {
        return linkLost;
    }

    void setOnLinkLostCallback(callback_function_t callback)
    {
        onLinkLostCallback = callback;
    }

    void setOnConnectedCallback(callback_function_t callback)
    {
        onConnectedCallback = callback;
    }

}