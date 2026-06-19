#pragma once

#include <cstdint>
#include "Hysteresis.h"
#include "GasAlarm.h"

namespace control {

// Threshold set for one climate zone. Defaults follow the comfort ranges
// from thesis section 1.1 (18-26 C, 40-60 %RH) and the gas levels from
// section 2.2 (alarm well below the 200 ppm system floor used for fire
// safety, so an everyday "stuffy room" gas alarm clears at a lower level).
struct ClimateThresholds {
    float tempMaxOn = 26.0f;
    float tempMinOff = 24.0f;
    float tempMinOn = 18.0f;
    float tempMaxOff = 20.0f;
    float humMinOn = 40.0f;
    float humMaxOff = 60.0f;
    float gasAlarmPpm = 200.0f;
    float gasSafePpm = 150.0f;
    uint32_t gasSafeHoldMs = 30000;
};

struct ClimateOutputs {
    bool ventilation = false;
    bool heating = false;
    bool humidifier = false;
    bool gasAlarm = false;
};

// Combines the three actuator channels from thesis Table 2.8: a gas alarm
// forces ventilation on and blocks heating/humidification regardless of the
// other readings, otherwise each channel runs its own independent hysteresis.
class ClimateController {
public:
    explicit ClimateController(const ClimateThresholds& t)
        : ventilation_(HysteresisChannel::Mode::ActivateAbove, t.tempMaxOn, t.tempMinOff),
          heating_(HysteresisChannel::Mode::ActivateBelow, t.tempMinOn, t.tempMaxOff),
          humidifier_(HysteresisChannel::Mode::ActivateBelow, t.humMinOn, t.humMaxOff),
          gasAlarm_(t.gasAlarmPpm, t.gasSafePpm, t.gasSafeHoldMs) {}

    ClimateOutputs update(float tempC, float humidityPct, float gasPpm, uint32_t nowMs) {
        const bool alarm = gasAlarm_.update(gasPpm, nowMs);
        const RelayState vent = ventilation_.update(tempC);
        const RelayState heat = heating_.update(tempC);
        const RelayState hum = humidifier_.update(humidityPct);

        ClimateOutputs out;
        out.gasAlarm = alarm;
        out.ventilation = alarm || (vent == RelayState::On);
        out.heating = !alarm && (heat == RelayState::On);
        out.humidifier = !alarm && (hum == RelayState::On);
        return out;
    }

private:
    HysteresisChannel ventilation_;
    HysteresisChannel heating_;
    HysteresisChannel humidifier_;
    GasAlarm gasAlarm_;
};

} // namespace control
