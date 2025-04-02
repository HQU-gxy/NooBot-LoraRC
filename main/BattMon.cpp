#include "Arduino.h"
#include "BattMon.h"
#include "config.h"

namespace BattMon
{
    constexpr auto N_SAMPLES = 10; // Number of samples to average
    static uint16_t readBuf[N_SAMPLES]{0};
    static uint8_t readIndex = 0;

    static void readVoltage(TimerHandle_t)
    {
        auto rcBatteryValue = analogRead(VBAT_SENSE_PIN);
        readBuf[readIndex++] = rcBatteryValue;
        if (readIndex >= N_SAMPLES)
        {
            readIndex = 0;
        }
    }

    void init()
    {
        analogSetAttenuation(ADC_11db); // Set the ADC attenuation to 11dB for better range
        analogReadResolution(ADC_RES);  // Set the ADC resolution to 10 bits

        static auto readVoltageTimer = xTimerCreate("BattMonTimer", pdMS_TO_TICKS(50), pdTRUE, NULL, readVoltage);
        xTimerStart(readVoltageTimer, 0);
    }

    uint8_t getBatteryLevel()
    {
        uint32_t sum = 0;
        for (int i = 0; i < N_SAMPLES; ++i)
        {
            sum += readBuf[i];
        }
        sum /= N_SAMPLES; // Average the samples
        auto voltage = sum * VBAT_MULTIPLIER / (1 << ADC_RES);

        return static_cast<uint8_t>(round(voltage * 100 / 4.2f)); // 4.2V is the maximum voltage of the battery
    }
} // namespace BattMon
