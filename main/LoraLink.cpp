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

    /**
     * @brief Calculate the checksum of the data by XORing all the bytes
     * 
     * @param data The pointer to the data
     * @param len The length of the data
     * @return uint8_t The checksum byte 
     */
    static uint8_t calcSum(const uint8_t *data, size_t len)
    {
        uint8_t sum = 0;
        for (size_t i = 0; i < len; i++)
        {
            sum ^= data[i];
        }
        return sum;
    }

    /**
     * @brief Check the connection status by reading the lock pin and calling the appropriate callback
     * 
     * @note This function is called by a timer
     */
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

    /**
     * @brief Check for status messages from the bot and call the callback if a message is received
     * 
     * @note This function is called by a timer
     */
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

    /**
     * @brief Initialize the LoraLink module
     * 
     * @param ser The serial port to use
     * @param pinTx The TX pin
     * @param pinRx The RX pin
     * @param pinLock The lock pin
     * 
     * @note This function should be called before using the LoraLink module
     */
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

    /**
     * @brief Send a command to the bot
     * 
     * @param linear Target linear speed in m/s
     * @param angular Target angular speed in rad/s
     */
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

    /**
     * @brief Check if the link is lost
     * 
     * @return true if the link is lost
     */
    bool isLinkLost()
    {
        return linkLost;
    }

    /**
     * @brief Set the callback function to be called when the link is lost
     * 
     * @param callback The callback function
     */
    void setOnLinkLostCallback(callback_function_t callback)
    {
        onLinkLostCallback = callback;
    }

    /**
     * @brief Set the callback function to be called when the link is established
     * 
     * @param callback The callback function
     */
    void setOnConnectedCallback(callback_function_t callback)
    {
        onConnectedCallback = callback;
    }

    /**
     * @brief Set the callback function to be called when a status message is received
     * 
     * @param callback The callback function
     */
    void setOnStatusCallback(onStatusCallback_t callback)
    {
        onStatusCallback = callback;
    }

}