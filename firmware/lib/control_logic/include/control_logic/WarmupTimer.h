#pragma once

#include <cstdint>

namespace control {

// Tracks the MQ-2 120 s warm-up period (thesis section 2.4: "Warm-up" mode).
// Takes an externally supplied millisecond clock so it can be unit-tested
// on the host without depending on Arduino's millis().
class WarmupTimer {
public:
    explicit WarmupTimer(uint32_t warmupDurationMs = 120000) : duration_(warmupDurationMs) {}

    void start(uint32_t nowMs) {
        startMs_ = nowMs;
        started_ = true;
    }

    bool isStarted() const { return started_; }

    bool isComplete(uint32_t nowMs) const {
        if (!started_) return false;
        return (nowMs - startMs_) >= duration_;
    }

    uint32_t elapsedMs(uint32_t nowMs) const {
        if (!started_) return 0;
        return nowMs - startMs_;
    }

private:
    uint32_t duration_;
    uint32_t startMs_ = 0;
    bool started_ = false;
};

} // namespace control
