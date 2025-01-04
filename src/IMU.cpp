#include <Arduino.h>
#include <BMI088.h>

#include "IMU.h"
#include "config.h"

namespace IMU
{
    SPIClass SPIIMU(SPI2_MOSI_PIN, SPI2_MISO_PIN, SPI2_SCK_PIN);
    Bmi088Accel accel(SPIIMU, ACC_CS_PIN);
    Bmi088Gyro gyro(SPIIMU, GYRO_CS_PIN);

    constexpr uint8_t AVR_SAMPLES_COUNT = 10;

    IMUData dataBuffer[AVR_SAMPLES_COUNT];

    bool begin()
    {
        if (accel.begin() < 0)
        {
            return false;
        }
        // if (gyro.begin() < 0)
        // {
        //     return false;
        // }

        accel.setOdr(Bmi088Accel::ODR_100HZ_BW_40HZ);
        accel.setRange(Bmi088Accel::RANGE_6G);
        gyro.setOdr(Bmi088Gyro::ODR_100HZ_BW_32HZ);
        gyro.setRange(Bmi088Gyro::RANGE_1000DPS);
        return true;
    }

    void readOnce()
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

    IMUData getData()
    {
        IMUData data = {0};
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
