#include <vector>
#include <cmath>

#include "Arduino.h"
#include "esp_log.h"
#include "Preferences.h"
#include "esp_pm.h"
#include "PCF8574.h"

#include "config.h"
#include "IMU.h"
#include "LoraLink.h"
#include "KeyPad.h"
#include "LED.hpp"
#include "Display.h"
#include "BattMon.h"

static const LED::BlinkPattern GOOD_PATTERN = {
    .oneshot = true,
    .pattern = {{true, 1}}};

static const LED::BlinkPattern ERROR_PATTERN = {
    .oneshot = false,
    .pattern = {{true, 200}, {false, 200}}};

static const LED::BlinkPattern LINK_LOST_PATTERN = {
    .oneshot = false,
    .pattern = {{true, 100}, {false, 100}, {true, 100}, {false, 700}}};

extern "C" void app_main()
{
    constexpr auto TAG = "app_main";
    initArduino();

    Display::begin();

    BattMon::init();
    LED led(LED_PIN);
    led.setBlinkPattern(GOOD_PATTERN);
    if (!IMU::begin())
    {
        ESP_LOGE(TAG, "IMU initialization failed");
        led.setBlinkPattern(ERROR_PATTERN);
        vTaskDelay(portMAX_DELAY);
    }

    if (!KeyPad::begin())
    {
        ESP_LOGE(TAG, "KeyPad initialization failed");
        led.setBlinkPattern(ERROR_PATTERN);
        vTaskDelay(portMAX_DELAY);
    }

    auto onBotStatus = [](const LoraLink::BotStatusMsg *msg)
    {
        Display::updateSpeed(msg->currentLinear);
        // Display::updateBotBatteryLevel(msg->voltage);
    };

    bool connected = true;
    LoraLink::setOnConnectedCallback([&led, &connected]()
                                     {connected = true;
                                         led.setBlinkPattern(GOOD_PATTERN); });

    LoraLink::setOnLinkLostCallback([&led, &connected]()
                                    {connected = false;
                                         led.setBlinkPattern(LINK_LOST_PATTERN); });

    LoraLink::setOnStatusCallback(onBotStatus);
    LoraLink::begin(Serial1, LORA_TX_PIN, LORA_RX_PIN, LORA_LOCK_PIN);

    constexpr auto CONTROLLER_PERIOD = 50;
    while (1)
    {
        static float linearMsg = 0;
        static float angularMsg = 0;

        auto rPressed = KeyPad::isPressed(KeyPad::KEY_R);
        auto lPressed = KeyPad::isPressed(KeyPad::KEY_L);
#if (LIN_MODE == 0)
        // Naturally decelerate
        if (linearMsg > 0)
            linearMsg -= IDLE_ACC * CONTROLLER_PERIOD / 1000;
        else if (linearMsg < 0)
            linearMsg += IDLE_ACC * CONTROLLER_PERIOD / 1000;

        // Forward/backward control
        switch (lPressed << 1 | rPressed)
        {
        case 0b01: // Go forward
            if (linearMsg < MAX_LIN_SPEED)
                linearMsg += THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
            break;

        case 0b10: // Go backward
            if (linearMsg > -MAX_LIN_SPEED)
                linearMsg -= THROTTLE_ACC * CONTROLLER_PERIOD / 1000;
            break;

        case 0b11: // Brake
            linearMsg = 0;
            break;

        default:
            break;
        }
#else
        // Forward/backward control
        if (lPressed)
            linearMsg = MAX_LIN_SPEED;
        else if (rPressed)
            linearMsg = -MAX_LIN_SPEED;
        else
            linearMsg = 0;
#endif

// Angular control
#if (ANG_MODE == 0)
        if (KeyPad::isPressed(KeyPad::KEY_LEFT))
            angularMsg = 1.0;
        else if (KeyPad::isPressed(KeyPad::KEY_RIGHT))
            angularMsg = -1.0;
        else
            angularMsg = 0;
#else
        auto data = IMU::getData();
        // ESP_LOGI(TAG, "Accel: %f, %f, %f", data.accel[0], data.accel[1], data.accel[2]);
        if (data.accel[2] < -8 || data.accel[2] > 1)
        { // It's put horizontally or upside down
            angularMsg = 0;
        }
        else
        {
            auto angle = -std::atan(data.accel[0] / data.accel[1]);
            if (data.accel[1] < 0)
            {
                if (data.accel[0] > 0)
                    angle -= M_PI;
                else
                    angle += M_PI;
            }

            angularMsg = angle * ANGLE_SENSITIVITY;
            if (angularMsg > MAX_ANG_SPEED)
                angularMsg = MAX_ANG_SPEED;
            else if (angularMsg < -MAX_ANG_SPEED)
                angularMsg = -MAX_ANG_SPEED;
        }
#endif
        if (connected)
            LoraLink::sendCommand(linearMsg, angularMsg);
        vTaskDelay(CONTROLLER_PERIOD);

        // Update the battery display every CONTROLLER_PERIOD *20ms
        static uint8_t cnt = 0;
        if (cnt++ == 20)
        {
            cnt = 0;
            auto rcBatteryLevel = BattMon::getBatteryLevel();
            Display::updateRCBatteryLevel(rcBatteryLevel);
        }
    }
}
