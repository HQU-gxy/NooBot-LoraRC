#include "LoraLink.h"

namespace LoraLink
{
    constexpr uint8_t CHECK_CONN_PERIOD = 50; // ms
    constexpr uint8_t MAX_UNLOCK_COUNT = 10;
    bool isLinkLost = false;

    callback_function_t onLinkLostCallback;

    HardwareSerial *LoraSerial;
    uint16_t lockPin;

    struct __attribute__((packed)) LoraLinkCommand
    {
        const uint8_t header = 0x12;
        float targetLinear;  // Linear speed in m/s
        float targetAngular; // Angular speed in rad/s
        uint8_t checksum;
    };

    void begin(HardwareSerial &ser, uint16_t pinTx, uint16_t pinRx, uint16_t pinLock)
    {
        lockPin = pinLock;
        LoraSerial = &ser;
        LoraSerial->setTx(pinTx);
        LoraSerial->setRx(pinRx);
        LoraSerial->begin(38400);
        LoraSerial->setTimeout(20);
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

    void setOnLinkLostCallback(callback_function_t callback)
    {
        onLinkLostCallback = callback;
    }

    void checkConnection()
    {
        static uint32_t lastCheckTime = 0;
        static uint8_t unlockCount = 0;
        if (millis() - lastCheckTime < CHECK_CONN_PERIOD)
        {
            return;
        }
        lastCheckTime = millis();

        if (!digitalRead(lockPin))
        {
            if (++unlockCount >= MAX_UNLOCK_COUNT)
            {
                unlockCount = MAX_UNLOCK_COUNT;
                if (!isLinkLost)
                {
                    isLinkLost = true;
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
            isLinkLost = false;
        }
    }
}