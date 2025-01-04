#pragma once

#include <stdint.h>

namespace IMU
{
    struct IMUData
    {
        // x, y, z
        float accel[3];
        float gyro[3];
    };

    /**
     * @brief Initialize the IMU
     */
    bool begin();

    /**
     * @brief Read and store the IMU data once
     */
    void readOnce();

    /**
     * @brief Get the (a little bit) filtered IMU data
     *
     * @return IMUData The IMU data you want
     */
    IMUData getData();
} // namespace IMU
