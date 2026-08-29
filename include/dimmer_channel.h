#pragma once

#include <Arduino.h>

#include "config.h"

// Один канал диммера: короткое нажатие — toggle, удержание — fade вверх/вниз.
class DimmerChannel {
public:
    DimmerChannel(uint8_t channelId,
                  uint8_t ledPin,
                  uint8_t buttonPin,
                  bool buttonInternalPullup = true,
                  bool buttonActiveHigh = false);

    void begin();
    void update();

    bool isPowered() const;
    void setPower(bool powered);

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
    const bool buttonActiveHigh_;

    bool powered_ = false;
    uint8_t brightnessPercent_ = config::kBrightnessDefaultPercent;
    // После boot яркость по умолчанию 100%, поэтому первый hold — вниз.
    FadeDirection nextHoldDirection_ = FadeDirection::Down;

    bool rawPressed_ = false;
    bool stablePressed_ = false;
    uint32_t lastRawChangeMs_ = 0;
    uint32_t pressStartMs_ = 0;

    bool holdActive_ = false;
    FadeDirection activeHoldDirection_ = FadeDirection::Up;
    uint32_t lastFadeStepMs_ = 0;
};
