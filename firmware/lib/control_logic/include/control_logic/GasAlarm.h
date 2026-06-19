#pragma once

#include <cstdint>

namespace control {

// Gas-alarm state machine (thesis section 2.4): enters the alarm state once
// the MQ-2 reading exceeds alarmThresholdPpm, and only leaves it after the
// reading has stayed continuously at or below safeThresholdPpm for at least
// safeHoldMs (30 s in the thesis) to avoid alarm "chatter" near the threshold.
class GasAlarm {
public:
    GasAlarm(float alarmThresholdPpm, float safeThresholdPpm, uint32_t safeHoldMs = 30000)
        : alarmThreshold_(alarmThresholdPpm), safeThreshold_(safeThresholdPpm), safeHoldMs_(safeHoldMs) {}

    bool update(float gasPpm, uint32_t nowMs) {
        if (!active_) {
            if (gasPpm > alarmThreshold_) {
                active_ = true;
                safeSinceMs_ = 0;
            }
            return active_;
        }

        if (gasPpm <= safeThreshold_) {
            if (safeSinceMs_ == 0) {
                safeSinceMs_ = nowMs;
            } else if (nowMs - safeSinceMs_ >= safeHoldMs_) {
                active_ = false;
                safeSinceMs_ = 0;
            }
        } else {
            safeSinceMs_ = 0;
        }
        return active_;
    }

    bool isActive() const { return active_; }

private:
    float alarmThreshold_;
    float safeThreshold_;
    uint32_t safeHoldMs_;
    bool active_ = false;
    uint32_t safeSinceMs_ = 0;
};

} // namespace control
