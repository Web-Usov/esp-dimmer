#include <Arduino.h>

#include "config.h"

void setup() {
    Serial.begin(config::kSerialBaud);

    pinMode(config::kLedPin, OUTPUT);
    digitalWrite(config::kLedPin, LOW);

    pinMode(config::kButtonPin, INPUT_PULLUP);

    Serial.println();
    Serial.println("esp-dimmer booted");
    Serial.println("Initial state: OFF");
}

void loop() {
    // The first functional step will add button handling and 1 kHz PWM.
}
