#include "dimmer_channel.h"

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
    pinMode(buttonPin_, buttonInternalPullup_ ? INPUT_PULLUP : INPUT);

#if defined(ESP8266)
    static bool pwmReady = false;
    if (!pwmReady) {
        analogWriteFreq(config::kPwmFrequencyHz);
        analogWriteRange(config::kPwmMaxDuty);
        pwmReady = true;
    }
    pinMode(ledPin_, OUTPUT);
#elif defined(ESP32)
    // 10-bit resolution covers 0..1000 duty values used by this firmware.
    ledcAttach(ledPin_, config::kPwmFrequencyHz, 10);
#else
#error "Unsupported platform"
#endif

    powered_ = false;
    brightnessPercent_ = config::kBrightnessDefaultPercent;
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

    if (holdActive_) {
        handleHoldFade(nowMs);
    } else if (stablePressed_ &&
               (nowMs - pressStartMs_) >= config::kLongPressMs) {
        startHoldFade();
    }
}

void DimmerChannel::handlePressEdge(bool pressed, uint32_t nowMs) {
    stablePressed_ = pressed;

    if (pressed) {
        pressStartMs_ = nowMs;
        return;
    }

    // Release.
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
        setPwmDuty(0);
        return;
    }

    if (brightnessPercent_ < config::kBrightnessMinPercent) {
        brightnessPercent_ = config::kBrightnessMinPercent;
    } else if (brightnessPercent_ > config::kBrightnessMaxPercent) {
        brightnessPercent_ = config::kBrightnessMaxPercent;
    }

    setPwmDuty(dutyFromPercent(brightnessPercent_));
}

uint16_t DimmerChannel::dutyFromPercent(uint8_t percent) const {
    return static_cast<uint16_t>(
        (static_cast<uint32_t>(percent) * config::kPwmMaxDuty) / 100U);
}

void DimmerChannel::setPwmDuty(uint16_t duty) {
#if defined(ESP8266)
    analogWrite(ledPin_, duty);
#elif defined(ESP32)
    ledcWrite(ledPin_, duty);
#endif
}

void DimmerChannel::logPrefix() const {
    Serial.print("ch");
    Serial.print(channelId_);
    Serial.print(' ');
}
