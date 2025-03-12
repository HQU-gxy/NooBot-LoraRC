
#pragma once

#include <Arduino.h>
#include <functional>
#include "IMU.h"

namespace LoraLink
{
    struct BotStatusMsg
    {
        uint8_t header;          // Should be 0x69;
        float currentLinear;     // Linear speed in m/s
        float currentAngular;    // Angular speed in rad/s
        uint8_t powerPercentage; // Battery percentage
        // Accel in m/s^2
        IMU::IMUData imu;
        uint8_t checksum;
    } __attribute__((packed));

    using callback_function_t = std::function<void()>;
    using onStatusCallback_t = std::function<void(const BotStatusMsg *)>;
    /**
     * @brief Initialize the LoraLink UART and start the command reading task
     *
     * @param ser The serial port to use
     * @param pinTx The TX pin
     * @param pinRx The RX pin
     * @param pinLock The pin indicating the LoraLink connection status
     */
    void begin(HardwareSerial &ser, uint16_t pinTx, uint16_t pinRx, uint16_t pinLock);

    /**
     * @brief Send a speed command to NooBot
     *
     * @param linear The target linear speed in m/s
     * @param angular The target angular speed in rad/s
     */
    void sendCommand(float linear, float angular);

    /**
     * @brief Set the callback function to call when the LoraLink connection is lost
     *
     *  It's a good idea to set this function before calling `begin()`
     *
     * @param callback The callback function
     */
    void setOnLinkLostCallback(callback_function_t callback);

    /**
     * @brief Set the callback function to call when the LoraLink connection is established
     *
     * It's a good idea to set this function before calling `begin()`
     *
     * @param callback The callback function
     */
    void setOnConnectedCallback(callback_function_t callback);

    /**
     * @brief Set the callback function to call when a status message is received
     *
     * It's a good idea to set this function before calling `begin()`
     *
     * @param callback The callback function
     */
    void setOnStatusCallback(onStatusCallback_t callback);
} // namespace LoraLink