#pragma once

#include <Arduino.h>
#include <stddef.h>

namespace config {

constexpr uint32_t kSerialBaud = 115200;
constexpr uint16_t kPwmFrequencyHz = 1000;
constexpr uint16_t kPwmMaxDuty = 1000;
constexpr uint8_t kBrightnessMinPercent = 10;
constexpr uint8_t kBrightnessMaxPercent = 100;
constexpr uint8_t kBrightnessDefaultPercent = 100;

constexpr uint16_t kButtonDebounceMs = 40;
constexpr uint16_t kLongPressMs = 450;
constexpr uint16_t kFadeStepIntervalMs = 30;
constexpr uint8_t kFadeStepPercent = 1;

constexpr size_t kChannelCount = 5;

#if defined(ESP8266)
// Matches docs/wiring.md — 5-channel NodeMCU stand.
// CH5 intentionally uses D0 for PWM and D8 for the button:
// - D0/GPIO16 can generate PWM but has no internal pull-up;
// - D8/GPIO15 already has the NodeMCU boot pull-down, so BTN5 is active HIGH
//   and connects to 3V3. This avoids an extra external pull-up resistor.
constexpr uint8_t kLedPins[kChannelCount] = {D1, D2, D5, D6, D0};
constexpr uint8_t kButtonPins[kChannelCount] = {D3, D4, D7, D9, D8};
constexpr bool kButtonInternalPullup[kChannelCount] = {
    true, true, true, true, false};
constexpr bool kButtonActiveHigh[kChannelCount] = {
    false, false, false, false, true};
#elif defined(ESP32)
// Prototype defaults for ESP32-C3. Verify the final pinout before wiring.
constexpr uint8_t kLedPins[kChannelCount] = {4, 5, 6, 7, 10};
constexpr uint8_t kButtonPins[kChannelCount] = {3, 2, 1, 0, 8};
constexpr bool kButtonInternalPullup[kChannelCount] = {
    true, true, true, true, true};
constexpr bool kButtonActiveHigh[kChannelCount] = {
    false, false, false, false, false};
#else
#error "Unsupported platform"
#endif

}  // namespace config
