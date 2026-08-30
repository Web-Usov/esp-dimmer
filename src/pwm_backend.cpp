#include "pwm_backend.h"

#include "config.h"

namespace pwm_backend {
namespace {

bool globalReady = false;

#if defined(ESP32)
#if !defined(ESP_ARDUINO_VERSION_MAJOR) || (ESP_ARDUINO_VERSION_MAJOR < 3)
constexpr uint8_t kLedcResolutionBits = 10;
constexpr size_t kMaxLedcBindings = 8;

struct LedcBinding {
    uint8_t pin;
    uint8_t channel;
    bool used;
};

LedcBinding ledcBindings[kMaxLedcBindings] = {};
uint8_t nextLedcChannel = 0;

uint8_t channelForPin(uint8_t pin) {
    for (size_t i = 0; i < kMaxLedcBindings; ++i) {
        if (ledcBindings[i].used && ledcBindings[i].pin == pin) {
            return ledcBindings[i].channel;
        }
    }
    for (size_t i = 0; i < kMaxLedcBindings; ++i) {
        if (!ledcBindings[i].used) {
            ledcBindings[i].used = true;
            ledcBindings[i].pin = pin;
            ledcBindings[i].channel = nextLedcChannel++;
            return ledcBindings[i].channel;
        }
    }
    return 0;
}
#endif
#endif

}  // namespace

uint16_t maxDuty() {
#if defined(ESP8266)
    return 1000;
#elif defined(ESP32)
    return 1023;  // 10-bit LEDC
#else
#error "Неподдерживаемая платформа"
#endif
}

void beginGlobal(uint32_t frequencyHz) {
    if (globalReady) {
        return;
    }

#if defined(ESP8266)
    analogWriteFreq(frequencyHz);
    analogWriteRange(maxDuty());
#elif defined(ESP32)
    (void)frequencyHz;
#else
#error "Неподдерживаемая платформа"
#endif

    globalReady = true;
}

void holdPinLow(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void beginPin(uint8_t pin) {
    beginGlobal(config::kPwmFrequencyHz);

#if defined(ESP8266)
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
#elif defined(ESP32)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcAttach(pin, config::kPwmFrequencyHz, 10);
    ledcWrite(pin, 0);
#else
    const uint8_t channel = channelForPin(pin);
    ledcSetup(channel, config::kPwmFrequencyHz, kLedcResolutionBits);
    ledcAttachPin(pin, channel);
    ledcWrite(channel, 0);
#endif
#else
#error "Неподдерживаемая платформа"
#endif
}

void writeDuty(uint8_t pin, uint16_t duty) {
    const uint16_t max = maxDuty();
    if (duty > max) {
        duty = max;
    }

#if defined(ESP8266)
    analogWrite(pin, duty);
#elif defined(ESP32)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
    ledcWrite(pin, duty);
#else
    ledcWrite(channelForPin(pin), duty);
#endif
#endif
}

void writePercent(uint8_t pin, uint8_t percent) {
    if (percent == 0) {
        writeDuty(pin, 0);
        return;
    }
    if (percent > 100) {
        percent = 100;
    }
    const uint16_t duty = static_cast<uint16_t>(
        (static_cast<uint32_t>(percent) * maxDuty()) / 100U);
    writeDuty(pin, duty);
}

}  // namespace pwm_backend
