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

    bool begin();

    IMUData getData();
} // namespace IMU