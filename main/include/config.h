#pragma once

// LED pins

// GPIO expander pins
constexpr auto PCF_SCL_PIN = 13;
constexpr auto PCF_SDA_PIN = 14;

// IMU pins
constexpr auto ACC_CS_PIN = 5;
constexpr auto GYRO_CS_PIN = 8;
constexpr auto SPI2_SCK_PIN = 4;
constexpr auto SPI2_MISO_PIN = 7;
constexpr auto SPI2_MOSI_PIN = 3;

// Screen pins
constexpr auto I2C1_SCL_PIN = 11;
constexpr auto I2C1_SDA_PIN = 12;

// LoraLink pins
constexpr auto LORA_TX_PIN = 22;
constexpr auto LORA_RX_PIN = 10;
constexpr auto LORA_LOCK_PIN = 1;

// Battery voltage sense pin
constexpr auto VBAT_SENSE_PIN = 0;

constexpr auto MAX_LINEAR = 3.0f;
constexpr auto MAX_ANGULAR = 1.0f;

// Throttle
constexpr auto IDLE_ACC = 0.1f;      // Deceleration, m/s^2
constexpr auto THROTTLE_ACC = 0.3f;  // Acceleration, m/s^2
constexpr auto MAX_LIN_SPEED = 3.0f; // Maximum speed, m/s
constexpr auto MAX_ANG_SPEED = 2.0f; // Maximum angular speed, rad/s

constexpr auto ANGLE_SENSITIVITY = 0.1f; // Sensitivity of the gravity-controlled mode