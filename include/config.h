#pragma once

#include <Arduino.h>

// LED pins
constexpr auto LED1_PIN = PD0;
constexpr auto LED2_PIN = PD1;

// Key pins
constexpr auto KEY_A_PIN = PD2;
constexpr auto KEY_B_PIN = PD3;
constexpr auto KEY_UP_PIN = PA4;
constexpr auto KEY_LEFT_PIN = PA5;
constexpr auto KEY_DOWN_PIN = PA6;
constexpr auto KEY_RIGHT_PIN = PA7;
constexpr auto KEY_L_PIN = PB1;
constexpr auto KEY_R_PIN = PA15;

// IMU pins
constexpr auto ACC_CS_PIN = PB11;
constexpr auto GYRO_CS_PIN = PB12;
constexpr auto SPI2_SCK_PIN = PB13;
constexpr auto SPI2_MISO_PIN = PB14;
constexpr auto SPI2_MOSI_PIN = PB15;

// Screen pins
constexpr auto I2C1_SCL_PIN = PB8;
constexpr auto I2C1_SDA_PIN = PB9;

// LoraLink pins
constexpr auto UART2_TX_PIN = PA2;
constexpr auto UART2_RX_PIN = PA3;
constexpr auto LORA_LOCK_PIN = PC13;

// Battery voltage sense pin
constexpr auto VBAT_SENSE_PIN = PA0;

constexpr auto MAX_LINEAR = 2.0f;
constexpr auto MAX_ANGULAR = 1.0f;