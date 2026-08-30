#include "dimmer_channel.h"

#include "pwm_backend.h"

DimmerChannel::DimmerChannel(uint8_t channelId,
                             uint8_t ledPin,
                             uint8_t buttonPin,
                             bool buttonInternalPullup,
                             bool buttonActiveHigh)
    : channelId_(channelId),
      ledPin_(ledPin),
      buttonPin_(buttonPin),
      buttonInternalPullup_(buttonInternalPullup),
      buttonActiveHigh_(buttonActiveHigh) {}

void DimmerChannel::begin() {
    pwm_backend::beginPin(ledPin_);

#if defined(ESP8266)
    if (buttonInternalPullup_) {
        pinMode(buttonPin_, INPUT_PULLUP);
    } else if (buttonPin_ == 16) {
        pinMode(buttonPin_, INPUT_PULLDOWN_16);
    } else {
        pinMode(buttonPin_, INPUT);
    }
#else
    pinMode(buttonPin_, buttonInternalPullup_ ? INPUT_PULLUP : INPUT);
#endif

    powered_ = false;
    brightnessPercent_ = config::kBrightnessDefaultPercent;
    ignoreUntilRelease_ = false;
    applyOutput();
}

void DimmerChannel::update() {
    const uint32_t nowMs = millis();
    const int buttonLevel = digitalRead(buttonPin_);
    const bool pressedNow = buttonActiveHigh_ ? (buttonLevel == HIGH)
                                              : (buttonLevel == LOW);

    if (pressedNow != rawPressed_) {
        rawPressed_ = pressedNow;
        lastRawChangeMs_ = nowMs;
    }

    if ((nowMs - lastRawChangeMs_) >= config::kButtonDebounceMs &&
        pressedNow != stablePressed_) {
        handlePressEdge(pressedNow, nowMs);
    }

    if (ignoreUntilRelease_) {
        return;
    }

    if (holdActive_) {
        handleHoldFade(nowMs);
    } else if (stablePressed_ &&
               (nowMs - pressStartMs_) >= config::kLongPressMs) {
        startHoldFade();
    }
}

bool DimmerChannel::isPowered() const {
    return powered_;
}

void DimmerChannel::setPower(bool powered) {
    powered_ = powered;
    applyOutput();
}

void DimmerChannel::cancelInteractionUntilRelease() {
    holdActive_ = false;
    // Игнор нужен только если кнопка сейчас зажата (иначе первое
    // короткое нажатие «съедается» и канал включается со второго раза).
    ignoreUntilRelease_ = stablePressed_ || rawPressed_;
}

void DimmerChannel::handlePressEdge(bool pressed, uint32_t nowMs) {
    stablePressed_ = pressed;

    if (ignoreUntilRelease_) {
        if (!pressed) {
            ignoreUntilRelease_ = false;
            pressStartMs_ = nowMs;
        }
        return;
    }

    if (pressed) {
        pressStartMs_ = nowMs;
        return;
    }

    if (holdActive_) {
        stopHoldFade();
        return;
    }

    if ((nowMs - pressStartMs_) < config::kLongPressMs) {
        togglePower();
    }
}

void DimmerChannel::startHoldFade() {
    holdActive_ = true;
    activeHoldDirection_ = nextHoldDirection_;
    lastFadeStepMs_ = millis();

    if (!powered_) {
        powered_ = true;
        brightnessPercent_ = config::kBrightnessMinPercent;
        activeHoldDirection_ = FadeDirection::Up;
        applyOutput();
        logPrefix();
        Serial.println("hold: ON from OFF @ 10%");
    } else {
        logPrefix();
        Serial.print("hold: fade ");
        Serial.println(activeHoldDirection_ == FadeDirection::Up ? "UP" : "DOWN");
    }
}

void DimmerChannel::stopHoldFade() {
    holdActive_ = false;
    nextHoldDirection_ = (activeHoldDirection_ == FadeDirection::Up)
                             ? FadeDirection::Down
                             : FadeDirection::Up;
    logPrefix();
    Serial.print("hold end @ ");
    Serial.print(brightnessPercent_);
    Serial.println("%");
}

void DimmerChannel::handleHoldFade(uint32_t nowMs) {
    if ((nowMs - lastFadeStepMs_) < config::kFadeStepIntervalMs) {
        return;
    }
    lastFadeStepMs_ = nowMs;

    if (activeHoldDirection_ == FadeDirection::Up) {
        if (brightnessPercent_ < config::kBrightnessMaxPercent) {
            brightnessPercent_ = static_cast<uint8_t>(
                brightnessPercent_ + config::kFadeStepPercent);
            if (brightnessPercent_ > config::kBrightnessMaxPercent) {
                brightnessPercent_ = config::kBrightnessMaxPercent;
            }
            applyOutput();
        }
    } else if (brightnessPercent_ > config::kBrightnessMinPercent) {
        brightnessPercent_ = static_cast<uint8_t>(
            brightnessPercent_ - config::kFadeStepPercent);
        if (brightnessPercent_ < config::kBrightnessMinPercent) {
            brightnessPercent_ = config::kBrightnessMinPercent;
        }
        applyOutput();
    }
}

void DimmerChannel::togglePower() {
    powered_ = !powered_;
    applyOutput();
    logPrefix();
    Serial.print(powered_ ? "ON @ " : "OFF (saved ");
    Serial.print(brightnessPercent_);
    Serial.println("%)");
}

void DimmerChannel::applyOutput() {
    if (!powered_) {
        pwm_backend::writePercent(ledPin_, 0);
        return;
    }

    if (brightnessPercent_ < config::kBrightnessMinPercent) {
        brightnessPercent_ = config::kBrightnessMinPercent;
    } else if (brightnessPercent_ > config::kBrightnessMaxPercent) {
        brightnessPercent_ = config::kBrightnessMaxPercent;
    }

    pwm_backend::writePercent(ledPin_, brightnessPercent_);
}

void DimmerChannel::logPrefix() const {
    Serial.print("ch");
    Serial.print(channelId_);
    Serial.print(' ');
}
