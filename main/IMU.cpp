#include <BMI088.h>
#include <Arduino.h>
#include "madgwick_filter.hpp"

#include "IMU.h"
#include "config.h"

namespace IMU
{
    static SPIClass SPIIMU(HSPI);
    static Bmi088Accel accel(SPIIMU, ACC_CS_PIN);
    static Bmi088Gyro gyro(SPIIMU, GYRO_CS_PIN);

    static uint32_t lastReadTime;
    constexpr auto READ_PERIOD = 10; // ms
    constexpr auto G = 9.807f;

    espp::MadgwickFilter madgwickFilter(0.5f);

    /**
     * @brief Read and store the IMU data once
     *
     * @note This function is called by the timer
     */
    static void readOnce(TimerHandle_t)
    {
        auto timeNow = millis();
        if (!accel.getDrdyStatus())
        {
            return;
        }

        accel.readSensor();
        gyro.readSensor();

        madgwickFilter.update(static_cast<float>(timeNow - lastReadTime) / 1000,
                              accel.getAccelX_mss() / G, accel.getAccelY_mss() / G, accel.getAccelZ_mss() / G,
                              gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());

        lastReadTime = timeNow;
    }

    /**
     * @brief Initialize the IMU and start the timer to read the data
     *
     * @return true if the initialization was successful
     */
    bool begin()
    {
        SPIIMU.begin(IMU_SCK_PIN, IMU_MISO_PIN, IMU_MOSI_PIN);
        auto ret = accel.begin();
        if (ret < 0)
        {
            ESP_LOGE("IMU", "Failed to initialize accelerometer: %d", ret);
            return false;
        }
        ret = gyro.begin();
        if (ret < 0)
        {
            ESP_LOGE("IMU", "Failed to initialize gyroscope");
            return false;
        }

        accel.setOdr(Bmi088Accel::ODR_100HZ_BW_40HZ);
        accel.setRange(Bmi088Accel::RANGE_6G);
        gyro.setOdr(Bmi088Gyro::ODR_100HZ_BW_32HZ);
        gyro.setRange(Bmi088Gyro::RANGE_1000DPS);

        lastReadTime = millis();
        static auto readTimer = xTimerCreate("IMURead", READ_PERIOD, pdTRUE, nullptr, readOnce);
        xTimerStart(readTimer, 0);
        return true;
    }

    /**
     * @brief Get the IMU data from the buffer
     *
     * @return std::tuple<float> A tuple containing the roll, pitch and yaw angles in radians
     * @note The angles are in degrees
     */
    std::tuple<float, float, float> getEuler()
    {
        float r, p, y;
        madgwickFilter.get_euler(p, r, y);

        return {r, p, y};
    }

} // namespace IMU