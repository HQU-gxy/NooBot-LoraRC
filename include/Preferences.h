#pragma once
#include <stdint.h>

namespace Preferences
{
    struct __attribute__((packed)) PreferencesData
    {
        uint8_t foo;
        uint8_t bar;
        uint8_t checksum; // Will be calculated by the write function, so it's not necessary to set it
    };

    /**
     * @brief Write the preferences data to the EEPROM
     *
     * @param data The data to write
     */
    void write(const PreferencesData &data);

    /**
     * @brief Read the preferences data from the EEPROM
     *
     * @return PreferencesData The read data
     */
    PreferencesData read();

} // namespace Preferences
