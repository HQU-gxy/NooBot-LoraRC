#include "LoraLink.h"

namespace LoraLink
{
    constexpr auto CHECK_CONN_PERIOD = 50; // ms
    constexpr auto CHECK_STAT_PERIOD = 50; // ms
    constexpr auto MAX_UNLOCK_COUNT = 5;

    constexpr auto TAG = "LoraLink";

    static bool linkLost = false;

    static HardwareSerial *loraSerial;
    static uint16_t lockPin;

    callback_function_t onLinkLostCallback;
    callback_function_t onConnectedCallback;
    onStatusCallback_t onStatusCallback;

    static uint8_t calcSum(const uint8_t *data, size_t len)
    {
        uint8_t sum = 0;
        for (size_t i = 0; i < len; i++)
        {
            sum ^= data[i];
        }
        return sum;
    }

    static void checkConnection(TimerHandle_t)
    {
        static uint8_t unlockCount = 0;

        if (!digitalRead(lockPin))
        {
            if (!linkLost)
                if (++unlockCount >= MAX_UNLOCK_COUNT)
                {
                    linkLost = true;
                    ESP_LOGW(TAG, "Link lost");
                    if (onLinkLostCallback)
                    {
                        onLinkLostCallback();
                    }
                }
        }
        else if (linkLost)
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

    static void checkForStatusMsg(TimerHandle_t)
    {
        if (linkLost)
            return;

        while (loraSerial->available() && (loraSerial->peek() != 0x69)) // The header byte
        {
            loraSerial->read();
        }

        if (!loraSerial->available())
            return;

        uint8_t buf[sizeof(BotStatusMsg)];
        loraSerial->readBytes(buf, sizeof(BotStatusMsg));
        auto parsed = reinterpret_cast<const BotStatusMsg *>(buf);
        auto sum = calcSum(buf, sizeof(BotStatusMsg) - 1);

        if (sum != parsed->checksum)
        {
            ESP_LOGW(TAG, "Status message checksum error: %x, %x", parsed->checksum, sum);
            return;
        }

        if (onStatusCallback)
            onStatusCallback(parsed);
        else
            ESP_LOGW(TAG, "No callback set for status message");

        while (loraSerial->available())
            loraSerial->read();
    }

    void begin(HardwareSerial &ser, uint16_t pinTx, uint16_t pinRx, uint16_t pinLock)
    {
        lockPin = pinLock;
        loraSerial = &ser;
        loraSerial->setPins(pinRx, pinTx);
        loraSerial->begin(38400);
        loraSerial->setTimeout(20);
        pinMode(lockPin, INPUT_PULLDOWN);

        static auto checkConnectionTimer = xTimerCreate("checkConnectionTimer", pdMS_TO_TICKS(CHECK_CONN_PERIOD), pdTRUE, nullptr, checkConnection);
        xTimerStart(checkConnectionTimer, 0);

        static auto checkForStatusMsgTimer = xTimerCreate("checkForStatusMsgTimer", pdMS_TO_TICKS(CHECK_STAT_PERIOD), pdTRUE, nullptr, checkForStatusMsg);
        xTimerStart(checkForStatusMsgTimer, 0);
    }

    void sendCommand(float linear, float angular)
    {
        struct __attribute__((packed)) LoraLinkCommand
        {
            const uint8_t header = 0x7b;
            float targetLinear;  // Linear speed in m/s
            float targetAngular; // Angular speed in rad/s
            uint8_t checksum;
        } cmd = {
            .targetLinear = linear,
            .targetAngular = angular,
            .checksum = 0,
        };

        uint8_t *cmdPtr = reinterpret_cast<uint8_t *>(&cmd);
        cmd.checksum = calcSum(cmdPtr, sizeof(cmd) - 1);
        loraSerial->write(cmdPtr, sizeof(cmd));
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

    void setOnStatusCallback(onStatusCallback_t callback)
    {
        onStatusCallback = callback;
    }

}