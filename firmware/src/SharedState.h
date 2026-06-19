#pragma once

#include <Arduino.h>
#include "control_logic/ClimateController.h"

// Snapshot of the latest sensor readings and control outputs, shared between
// the Core 1 sensing/control tasks and the Core 0 telemetry/logging tasks
// (thesis section 2.4, "exchange between tasks ... via FreeRTOS queues
// protected by mutexes"). Access only through SharedState::with().
struct SystemSnapshot {
    float temperatureC = NAN;
    float humidityPct = NAN;
    float pressureHpa = NAN;
    float gasPpm = NAN;
    bool bme280Ok = false;
    bool warmupComplete = false;

    control::ClimateOutputs outputs;

    bool manualOverrideActive = false;
};

class SharedState {
public:
    SharedState() { mutex_ = xSemaphoreCreateMutex(); }

    template <typename Fn>
    void with(Fn&& fn) {
        xSemaphoreTake(mutex_, portMAX_DELAY);
        fn(snapshot_);
        xSemaphoreGive(mutex_);
    }

    SystemSnapshot read() {
        SystemSnapshot copy;
        xSemaphoreTake(mutex_, portMAX_DELAY);
        copy = snapshot_;
        xSemaphoreGive(mutex_);
        return copy;
    }

private:
    SystemSnapshot snapshot_;
    SemaphoreHandle_t mutex_;
};

// One process-wide instance, defined in main.cpp.
extern SharedState gState;
