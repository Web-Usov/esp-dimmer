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
    {5, config::kLedPins[4], config::kButtonPins[4],
     config::kButtonInternalPullup[4], config::kButtonActiveHigh[4]},
};

}  // namespace

void setup() {
    Serial.begin(config::kSerialBaud);
    delay(50);

    for (size_t i = 0; i < config::kChannelCount; ++i) {
        channels[i].begin();
    }

    Serial.println();
    Serial.println("esp-dimmer booted");
    Serial.print("channels: ");
    Serial.println(config::kChannelCount);
    Serial.println("short press: toggle | hold: fade up/down");
    Serial.println("Initial state: all OFF");
}

void loop() {
    for (size_t i = 0; i < config::kChannelCount; ++i) {
        channels[i].update();
    }
}
