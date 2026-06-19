#pragma once

namespace control {

enum class RelayState { Off, On };

// Two-threshold (Schmitt-trigger style) hysteresis for a single relay channel.
// ActivateAbove: turns ON once value rises above onThreshold, turns OFF once it
// falls below offThreshold (onThreshold > offThreshold).
// ActivateBelow: turns ON once value falls below onThreshold, turns OFF once it
// rises above offThreshold (onThreshold < offThreshold).
// See thesis section 2.4 / Table 2.8 for the channel-specific threshold values.
class HysteresisChannel {
public:
    enum class Mode { ActivateAbove, ActivateBelow };

    HysteresisChannel(Mode mode, float onThreshold, float offThreshold)
        : mode_(mode), onThreshold_(onThreshold), offThreshold_(offThreshold) {}

    RelayState update(float value) {
        if (mode_ == Mode::ActivateAbove) {
            if (state_ == RelayState::Off && value > onThreshold_) {
                state_ = RelayState::On;
            } else if (state_ == RelayState::On && value < offThreshold_) {
                state_ = RelayState::Off;
            }
        } else {
            if (state_ == RelayState::Off && value < onThreshold_) {
                state_ = RelayState::On;
            } else if (state_ == RelayState::On && value > offThreshold_) {
                state_ = RelayState::Off;
            }
        }
        return state_;
    }

    RelayState state() const { return state_; }
    void forceState(RelayState s) { state_ = s; }

private:
    Mode mode_;
    float onThreshold_;
    float offThreshold_;
    RelayState state_ = RelayState::Off;
};

} // namespace control
