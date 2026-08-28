#pragma once

#include <Arduino.h>

namespace config {

constexpr uint32_t kSerialBaud = 115200;
constexpr uint16_t kPwmFrequencyHz = 1000;
constexpr uint8_t kBrightnessMinPercent = 10;
constexpr uint8_t kBrightnessMaxPercent = 100;

#if defined(ESP8266)
constexpr uint8_t kLedPin = D1;
constexpr uint8_t kButtonPin = D2;
#elif defined(ESP32)
// Prototype defaults for ESP32-C3. Verify the final pinout before wiring.
constexpr uint8_t kLedPin = 4;
constexpr uint8_t kButtonPin = 3;
#else
#error "Unsupported platform"
#endif

}  // namespace config
