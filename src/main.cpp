#include <Arduino.h>

#include "IMU.h"
#include "LED.hpp"
#include "LoraLink.h"

#include "config.h"

const LED::BlinkPattern imuErrorPattern = {
    .oneshot = false,
    .pattern = {
        {1, 100},
        {0, 100},
        {1, 100},
        {0, 700},
    }};

const LED::BlinkPattern runningPattern = {
    .oneshot = false,
    .pattern = {
        {1, 200},
        {0, 200},
    }};

LED led1(LED1_PIN);
LED led2(LED2_PIN);

void setup()
{
  pinMode(KEY_A_PIN, INPUT_PULLUP);
  pinMode(KEY_B_PIN, INPUT_PULLUP);
  pinMode(KEY_UP_PIN, INPUT_PULLUP);
  pinMode(KEY_LEFT_PIN, INPUT_PULLUP);
  pinMode(KEY_DOWN_PIN, INPUT_PULLUP);
  pinMode(KEY_RIGHT_PIN, INPUT_PULLUP);
  pinMode(KEY_L_PIN, INPUT_PULLUP);
  pinMode(KEY_R_PIN, INPUT_PULLUP);

  uint8_t errorCnt = 0;
  while (true)
    if (IMU::begin())
      break;
    else if (++errorCnt == 5)
    {
      led1.setBlinkPattern(imuErrorPattern);
      return;
    }

  led2.setBlinkPattern(runningPattern);
  LoraLink::begin(Serial2, UART2_TX_PIN, UART2_RX_PIN, LORA_LOCK_PIN);
}

void loop()
{
  // put your main code here, to run repeatedly:
  static float targetLinear = 0.0f;

  led1.handleBlink();
  led2.handleBlink();
  IMU::readOnce();
  IMU::IMUData data = IMU::getData();

  if (digitalRead(KEY_R_PIN) == LOW)
  {
    if (targetLinear < MAX_LINEAR)
    {
      targetLinear += 0.05;
    }
  }
  else if (digitalRead(KEY_L_PIN) == LOW)
  {
    if (targetLinear > -MAX_LINEAR)
    {
      targetLinear -= 0.05;
    }
  }
  else
  {
    if (targetLinear > 0.0f)
      targetLinear -= 0.01;
    else if (targetLinear < 0.0f)
      targetLinear += 0.01;
  }

  LoraLink::sendCommand(targetLinear, 0);

  delay(50);
}
