
#pragma once

#include <Arduino.h>
#include <functional>

namespace LoraLink
{
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
} // namespace LoraLink
