#pragma once

// LED pins
constexpr auto LED_PIN = 3;

// GPIO expander pins
constexpr auto PCF_SCL_PIN = 18;
constexpr auto PCF_SDA_PIN = 17;

// IMU pins
constexpr auto ACC_CS_PIN = 11;
constexpr auto GYRO_CS_PIN = 14;
constexpr auto IMU_SCK_PIN = 13;
constexpr auto IMU_MISO_PIN = 10;
constexpr auto IMU_MOSI_PIN = 12;

// Screen pins are defined in the sdkconfig
constexpr auto SCREEN_BL_PIN = 33;

// LoraLink pins
constexpr auto LORA_TX_PIN = 7;
constexpr auto LORA_RX_PIN = 8;
constexpr auto LORA_LOCK_PIN = 6;

// Battery voltage sense pin
constexpr auto VBAT_SENSE_PIN = 1;
constexpr auto VBAT_MULTIPLIER = 5.13f; // Voltage divider multiplier
constexpr auto ADC_RES = 12;           // ADC resolution in bits

#define ANG_MODE 1 // 0: Button, 1: Gravity
#define LIN_MODE 0 // 0: Const Accel, 1: Const Vel

// Throttle
#if (LIN_MODE == 0)
constexpr auto IDLE_ACC = 0.1f;     // Deceleration, m/s^2
constexpr auto THROTTLE_ACC = 0.3f; // Acceleration, m/s^2
#endif
constexpr auto MAX_LIN_SPEED = 3.0f; // Maximum speed, m/s

#if (ANG_MODE == 1)
constexpr auto ANGLE_SENSITIVITY = 1.0f; // Sensitivity of the gravity-controlled mode
#endif
constexpr auto MAX_ANG_SPEED = 2.0f; // Maximum angular speed, rad/s
