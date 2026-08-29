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
// Соответствует docs/wiring.md — пятиканальный стенд NodeMCU.
// Канал 5: LED на D8 (PWM; при OFF пин LOW — совместимо с boot).
// Кнопка на D0 с INPUT_PULLDOWN_16, нажатие на 3V3 (active HIGH).
// Так избегаем кнопку на D8 (сильный board pull-down) и PWM на D0
// (у GPIO16 на ESP8266 нет нормального software PWM).
constexpr uint8_t kLedPins[kChannelCount] = {D1, D2, D5, D6, D8};
constexpr uint8_t kButtonPins[kChannelCount] = {D3, D4, D7, D9, D0};
// true = INPUT_PULLUP, false = INPUT_PULLDOWN_16 (только GPIO16).
constexpr bool kButtonInternalPullup[kChannelCount] = {
    true, true, true, true, false};
constexpr bool kButtonActiveHigh[kChannelCount] = {
    false, false, false, false, true};
#elif defined(ESP32)
// Черновые пины для ESP32-C3. Перед пайкой сверить финальную распиновку.
constexpr uint8_t kLedPins[kChannelCount] = {4, 5, 6, 7, 10};
constexpr uint8_t kButtonPins[kChannelCount] = {3, 2, 1, 0, 8};
constexpr bool kButtonInternalPullup[kChannelCount] = {
    true, true, true, true, true};
constexpr bool kButtonActiveHigh[kChannelCount] = {
    false, false, false, false, false};
#else
#error "Неподдерживаемая платформа"
#endif

}  // namespace config
