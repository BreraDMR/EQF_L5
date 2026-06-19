#pragma once

#include <Arduino.h>
#include "control_logic/ClimateController.h"
#include "Config.h"

// Drives the three opto-isolated relay channels from a ClimateOutputs
// decision (hardware/diagrams.drawio, sheet 3 -- electrical schematic).
class RelayDriver {
public:
    void begin() {
        pinMode(cfg::kRelayVentilationPin, OUTPUT);
        pinMode(cfg::kRelayHeatingPin, OUTPUT);
        pinMode(cfg::kRelayHumidifierPin, OUTPUT);
        apply({});
    }

    void apply(const control::ClimateOutputs& outputs) {
        digitalWrite(cfg::kRelayVentilationPin, outputs.ventilation ? HIGH : LOW);
        digitalWrite(cfg::kRelayHeatingPin, outputs.heating ? HIGH : LOW);
        digitalWrite(cfg::kRelayHumidifierPin, outputs.humidifier ? HIGH : LOW);
    }
};
