#include <Arduino.h>

#include "config.h"
#include "dimmer_channel.h"

namespace {

DimmerChannel channel(config::kLedPin, config::kButtonPin);

}  // namespace

void setup() {
    Serial.begin(config::kSerialBaud);
    delay(50);

    channel.begin();

    Serial.println();
    Serial.println("esp-dimmer booted");
    Serial.println("short press: toggle | hold: fade up/down");
    Serial.println("Initial state: OFF");
}

void loop() {
    channel.update();
}
