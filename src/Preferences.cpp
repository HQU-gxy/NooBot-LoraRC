#include <Arduino.h>
#include <utility/stm32_eeprom.h>

#include "Preferences.h"

namespace Preferences
{
    void write(const PreferencesData &data)
    {
        eeprom_buffer_fill();
        uint8_t sum = 0;
        for (uint8_t i = 0; i < sizeof(PreferencesData) - 1; i++)
        {
            eeprom_buffered_write_byte(i, reinterpret_cast<const uint8_t *>(&data)[i]);
            sum ^= reinterpret_cast<const uint8_t *>(&data)[i];
        }
        eeprom_buffered_write_byte(sizeof(PreferencesData) - 1, sum);
        eeprom_buffer_flush();
    }

    PreferencesData read()
    {
        eeprom_buffer_fill();
        PreferencesData data;
        uint8_t sum = 0;
        for (uint8_t i = 0; i < sizeof(PreferencesData); i++)
        {
            reinterpret_cast<uint8_t *>(&data)[i] = eeprom_buffered_read_byte(i);
            sum ^= reinterpret_cast<uint8_t *>(&data)[i];
        }

        // Checksum error, return nothing
        if (sum != data.checksum)
        {
            return PreferencesData{0};
        }

        return data;
    }
} // namespace Preferences