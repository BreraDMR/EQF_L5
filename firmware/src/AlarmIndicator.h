#pragma once

#include <Arduino.h>
#include "Config.h"

// Buzzer + status LEDs (thesis section 1.1: buzzer activates and the
// display shows "Fire" on critical gas concentration).
class AlarmIndicator {
public:
    void begin() {
        pinMode(cfg::kBuzzerPin, OUTPUT);
        pinMode(cfg::kLedGreenPin, OUTPUT);
        pinMode(cfg::kLedRedPin, OUTPUT);
        digitalWrite(cfg::kLedGreenPin, HIGH); // power indicator, on whenever the firmware is running
    }

    void setAlarm(bool active) {
        digitalWrite(cfg::kBuzzerPin, active ? HIGH : LOW);
        digitalWrite(cfg::kLedRedPin, active ? HIGH : LOW);
    }
};
