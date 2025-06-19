#pragma once

#include <BluetoothSerial.h>
#include "Config.h"
#include "SharedState.h"

// Bluetooth SPP telemetry (thesis section 2.4): broadcasts a text line every
// 5 s in the format documented in Table 1.1.
class BluetoothTelemetry {
public:
    void begin() { bt_.begin(cfg::kBluetoothDeviceName); }

    bool connected() { return bt_.hasClient(); }

    void sendSnapshot(const SystemSnapshot& s) {
        if (!bt_.hasClient()) return;
        bt_.printf(
            "Gas contamination: %.0f / Temperature: %.1f C / Humidity: %.0f %% / "
            "Relay1(Vent): %s / Relay2(Heat): %s / Relay3(Humid): %s / Alarm: %s\n",
            s.gasPpm, s.temperatureC, s.humidityPct,
            s.outputs.ventilation ? "ON" : "OFF",
            s.outputs.heating ? "ON" : "OFF",
            s.outputs.humidifier ? "ON" : "OFF",
            s.outputs.gasAlarm ? "ON" : "OFF");
    }

private:
    BluetoothSerial bt_;
};
