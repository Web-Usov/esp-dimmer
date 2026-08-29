#include <Arduino.h>

#include "config.h"
#include "dimmer_channel.h"

namespace {

DimmerChannel channels[config::kChannelCount] = {
    {1, config::kLedPins[0], config::kButtonPins[0],
     config::kButtonInternalPullup[0], config::kButtonActiveHigh[0]},
    {2, config::kLedPins[1], config::kButtonPins[1],
     config::kButtonInternalPullup[1], config::kButtonActiveHigh[1]},
    {3, config::kLedPins[2], config::kButtonPins[2],
     config::kButtonInternalPullup[2], config::kButtonActiveHigh[2]},
    {4, config::kLedPins[3], config::kButtonPins[3],
     config::kButtonInternalPullup[3], config::kButtonActiveHigh[3]},
};

bool masterRawPressed = false;
bool masterStablePressed = false;
uint32_t masterLastRawChangeMs = 0;

// Как можно раньше прижимаем PWM-выходы в OFF (до Serial/delay).
void holdLedOutputsOff() {
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        pinMode(config::kLedPins[i], OUTPUT);
        digitalWrite(config::kLedPins[i], LOW);
    }
}

void setupMasterButton() {
    pinMode(config::kMasterButtonPin,
            config::kMasterButtonInternalPullup ? INPUT_PULLUP : INPUT);
}

bool readMasterPressed() {
    const int level = digitalRead(config::kMasterButtonPin);
    return config::kMasterButtonActiveHigh ? (level == HIGH) : (level == LOW);
}

void toggleMaster() {
    bool anyPowered = false;
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        anyPowered = anyPowered || channels[i].isPowered();
    }

    const bool targetPowered = !anyPowered;
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        channels[i].setPower(targetPowered);
    }

    Serial.println(targetPowered ? "master: ALL ON" : "master: ALL OFF");
}

void updateMasterButton() {
    const uint32_t nowMs = millis();
    const bool pressedNow = readMasterPressed();

    if (pressedNow != masterRawPressed) {
        masterRawPressed = pressedNow;
        masterLastRawChangeMs = nowMs;
    }

    if ((nowMs - masterLastRawChangeMs) < config::kButtonDebounceMs ||
        pressedNow == masterStablePressed) {
        return;
    }

    masterStablePressed = pressedNow;
    if (masterStablePressed) {
        toggleMaster();
    }
}

}  // анонимный namespace

#if defined(ESP8266)
// Вызывается ядром ESP8266 до setup() — сокращает окно boot-глитча на выходах.
void preinit() {
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        pinMode(config::kLedPins[i], OUTPUT);
        digitalWrite(config::kLedPins[i], LOW);
    }
}
#endif

void setup() {
    holdLedOutputsOff();

    Serial.begin(config::kSerialBaud);
    delay(50);

    holdLedOutputsOff();

    for (size_t i = 0; i < config::kChannelCount; ++i) {
        channels[i].begin();
    }
    setupMasterButton();

    Serial.println();
    Serial.println("esp-dimmer booted");
    Serial.println("channels: 4 + master");
    Serial.println("channel: short press toggle | hold fade up/down");
    Serial.println("master: any ON -> ALL OFF | all OFF -> ALL ON");
    Serial.println("Initial state: all OFF");
}

void loop() {
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        channels[i].update();
    }
    updateMasterButton();
}
