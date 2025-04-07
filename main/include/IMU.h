#pragma once

#include <stdint.h>
#include <utility>

namespace IMU
{
    bool begin();

    std::tuple<float,float,float> getEuler();
} // namespace IMU