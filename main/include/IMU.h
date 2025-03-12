#pragma once

#include <stdint.h>

namespace IMU
{
    struct __attribute__((packed)) IMUData
    {
        // x, y, z
        float accel[3]{0};
        float gyro[3]{0};
    };

    /**
     * @brief Initialize the IMU
     */
    bool begin();

    /**
     * @brief Get the (a little bit) filtered IMU data
     *
     * @return IMUData The IMU data you want
     */
    IMUData getData();
} // namespace IMU