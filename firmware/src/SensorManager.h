#pragma once

#include <Adafruit_BME280.h>
#include "control_logic/WarmupTimer.h"
#include "Config.h"

// Owns the BME280 driver and the MQ-2 analog read + warm-up timer
// (thesis sections 2.1, 2.3, 2.4). Pure sensor I/O only -- the actual
// on/off decisions live in control::ClimateController.
class SensorManager {
public:
    bool begin() {
        Wire.begin(cfg::kI2cSda, cfg::kI2cScl);
        bme280Ok_ = bme_.begin(cfg::kBme280Address);
        warmup_.start(millis());
        return bme280Ok_;
    }

    bool bme280Ok() const { return bme280Ok_; }

    float readTemperatureC() { return bme280Ok_ ? bme_.readTemperature() : NAN; }
    float readHumidityPct() { return bme280Ok_ ? bme_.readHumidity() : NAN; }
    float readPressureHpa() { return bme280Ok_ ? bme_.readPressure() / 100.0f : NAN; }

    // Raw 0-4095 ADC reading converted to an approximate ppm value. The MQ-2
    // is not a calibrated instrument (thesis section 2.2: "+-20-30% after
    // calibration") -- this linear mapping is a placeholder; replace with a
    // calibration curve fitted against a known reference gas if precision
    // matters for your deployment.
    float readGasPpm() {
        const int raw = analogRead(cfg::kMq2Pin);
        constexpr float kMaxPpm = 10000.0f;
        constexpr float kAdcMax = 4095.0f;
        return (raw / kAdcMax) * kMaxPpm;
    }

    bool warmupComplete() const { return warmup_.isComplete(millis()); }

private:
    Adafruit_BME280 bme_;
    bool bme280Ok_ = false;
    control::WarmupTimer warmup_{cfg::kMq2WarmupMs};
};
