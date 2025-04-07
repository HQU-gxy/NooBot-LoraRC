#include <BMI088.h>
#include <Arduino.h>

#include "IMU.h"
#include "config.h"

namespace IMU
{
    static SPIClass SPIIMU(HSPI);
    static Bmi088Accel accel(SPIIMU, ACC_CS_PIN);
    static Bmi088Gyro gyro(SPIIMU, GYRO_CS_PIN);

    constexpr uint8_t AVR_SAMPLES_COUNT = 10;
    static IMUData dataBuffer[AVR_SAMPLES_COUNT];

    /**
     * @brief Read and store the IMU data once
     */
    static void readOnce(TimerHandle_t)
    {
        if (!accel.getDrdyStatus())
        {
            return;
        }
        accel.readSensor();
        gyro.readSensor();
        for (uint8_t i = 0; i < AVR_SAMPLES_COUNT - 1; i++)
        {
            dataBuffer[i] = dataBuffer[i + 1];
        }
        dataBuffer[AVR_SAMPLES_COUNT - 1] = IMUData{
            accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss(),
            gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads()};
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

        static auto readTimer = xTimerCreate("IMURead", 10, pdTRUE, nullptr, readOnce);
        xTimerStart(readTimer, 0);
        return true;
    }

    /**
     * @brief Get the IMU data from the buffer
     *
     * @return IMUData the IMU data
     */
    IMUData getData()
    {
        IMUData data;
        for (uint8_t i = 0; i < AVR_SAMPLES_COUNT; i++)
        {
            data.accel[0] += dataBuffer[i].accel[0];
            data.accel[1] += dataBuffer[i].accel[1];
            data.accel[2] += dataBuffer[i].accel[2];
            data.gyro[0] += dataBuffer[i].gyro[0];
            data.gyro[1] += dataBuffer[i].gyro[1];
            data.gyro[2] += dataBuffer[i].gyro[2];
        }
        data.accel[0] /= AVR_SAMPLES_COUNT;
        data.accel[1] /= AVR_SAMPLES_COUNT;
        data.accel[2] /= AVR_SAMPLES_COUNT;
        data.gyro[0] /= AVR_SAMPLES_COUNT;
        data.gyro[1] /= AVR_SAMPLES_COUNT;
        data.gyro[2] /= AVR_SAMPLES_COUNT;
        return data;
    }

} // namespace IMU