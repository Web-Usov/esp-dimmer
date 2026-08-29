#pragma once

#include <Arduino.h>

#include "config.h"

// One dimmer channel: short press toggles, hold fades up/down alternately.
class DimmerChannel {
public:
    DimmerChannel(uint8_t channelId,
                  uint8_t ledPin,
                  uint8_t buttonPin,
                  bool buttonInternalPullup = true);

    void begin();
    void update();

private:
    enum class FadeDirection : uint8_t { Up, Down };

    void applyOutput();
    void setPwmDuty(uint16_t duty);
    uint16_t dutyFromPercent(uint8_t percent) const;
    void handlePressEdge(bool pressed, uint32_t nowMs);
    void handleHoldFade(uint32_t nowMs);
    void togglePower();
    void startHoldFade();
    void stopHoldFade();
    void logPrefix() const;

    const uint8_t channelId_;
    const uint8_t ledPin_;
    const uint8_t buttonPin_;
    const bool buttonInternalPullup_;

    bool powered_ = false;
    uint8_t brightnessPercent_ = config::kBrightnessDefaultPercent;
    FadeDirection nextHoldDirection_ = FadeDirection::Up;

    bool rawPressed_ = false;
    bool stablePressed_ = false;
    uint32_t lastRawChangeMs_ = 0;
    uint32_t pressStartMs_ = 0;

    bool holdActive_ = false;
    FadeDirection activeHoldDirection_ = FadeDirection::Up;
    uint32_t lastFadeStepMs_ = 0;
};
