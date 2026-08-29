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

constexpr size_t kChannelCount = 4;

#if defined(ESP8266)
// 4 канала + master, все кнопки на GND с INPUT_PULLUP (без подключения к 3V3).
// LED ch4 на D8: при OFF = LOW, совместимо с boot (GPIO15 должен быть LOW).
// Master на D9/RX: Serial только на вывод через TX.
// D0 не используем — у GPIO16 нет INPUT_PULLUP.
constexpr uint8_t kLedPins[kChannelCount] = {D1, D2, D5, D8};
constexpr uint8_t kButtonPins[kChannelCount] = {D3, D4, D7, D6};
constexpr bool kButtonInternalPullup[kChannelCount] = {true, true, true, true};
constexpr bool kButtonActiveHigh[kChannelCount] = {false, false, false, false};

constexpr uint8_t kMasterButtonPin = D9;
constexpr bool kMasterButtonInternalPullup = true;
constexpr bool kMasterButtonActiveHigh = false;
#elif defined(ESP32)
// Черновые пины для ESP32-C3. Перед пайкой сверить финальную распиновку.
constexpr uint8_t kLedPins[kChannelCount] = {4, 5, 6, 7};
constexpr uint8_t kButtonPins[kChannelCount] = {3, 2, 1, 0};
constexpr bool kButtonInternalPullup[kChannelCount] = {true, true, true, true};
constexpr bool kButtonActiveHigh[kChannelCount] = {false, false, false, false};
constexpr uint8_t kMasterButtonPin = 8;
constexpr bool kMasterButtonInternalPullup = true;
constexpr bool kMasterButtonActiveHigh = false;
#else
#error "Неподдерживаемая платформа"
#endif

}  // namespace config
