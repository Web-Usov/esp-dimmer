#pragma once

#include <Arduino.h>

namespace pwm_backend {

// Платформенный максимум duty (ESP8266: 1000, ESP32 10-bit LEDC: 1023).
uint16_t maxDuty();

// Один раз: частота PWM (1 кГц) и range/resolution платформы.
void beginGlobal(uint32_t frequencyHz);

// Подключить пин и сразу выставить OFF (LOW / duty 0).
void beginPin(uint8_t pin);

// Запись сырого duty 0…maxDuty().
void writeDuty(uint8_t pin, uint16_t duty);

// Яркость 0…100% → платформенный duty (100% = full ON).
void writePercent(uint8_t pin, uint8_t percent);

// Безопасный OFF без полной инициализации LEDC (для preinit/раннего setup).
void holdPinLow(uint8_t pin);

}  // namespace pwm_backend
