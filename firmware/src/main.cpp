// ESP32 air-quality monitoring/control firmware.
// Implements the architecture described in ../docs/thesis.md sections 2.1-2.4:
// FreeRTOS tasks split across both cores (Table 2.9), BME280 + MQ-2 sensing,
// hysteresis-based relay control (Table 2.8), LCD + buzzer indication,
// Bluetooth SPP and MQTT telemetry, and LittleFS event logging.
#include <Arduino.h>

#include "control_logic/ClimateController.h"
#include "Config.h"
#include "SharedState.h"
#include "SensorManager.h"
#include "RelayDriver.h"
#include "AlarmIndicator.h"
#include "DisplayManager.h"
#include "EventLogger.h"
#include "BluetoothTelemetry.h"
#include "MqttTelemetry.h"

SharedState gState;

namespace {

SensorManager gSensors;
RelayDriver gRelays;
AlarmIndicator gAlarmIndicator;
DisplayManager gDisplay;
EventLogger gLogger;
BluetoothTelemetry gBluetooth;
MqttTelemetry gMqtt;
control::ClimateController gController(control::ClimateThresholds{});

bool gAlarmWasActive = false;

void taskSensors(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kSensorPollIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        const float t = gSensors.readTemperatureC();
        const float h = gSensors.readHumidityPct();
        const float p = gSensors.readPressureHpa();
        const float gas = gSensors.readGasPpm();
        const bool warm = gSensors.warmupComplete();
        const bool bmeOk = gSensors.bme280Ok();

        gState.with([&](SystemSnapshot& s) {
            s.temperatureC = t;
            s.humidityPct = h;
            s.pressureHpa = p;
            s.gasPpm = gas;
            s.warmupComplete = warm;
            s.bme280Ok = bmeOk;
        });

        vTaskDelayUntil(&lastWake, period);
    }
}

void taskControl(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kControlIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        SystemSnapshot snap = gState.read();

        // Until the MQ-2 warm-up finishes, gas readings are unreliable
        // (thesis 2.4): keep relays driven by temperature/humidity only by
        // feeding a "safe" gas value into the controller during warm-up.
        const float gasForControl = snap.warmupComplete ? snap.gasPpm : 0.0f;

        control::ClimateOutputs out =
            gController.update(snap.temperatureC, snap.humidityPct, gasForControl, millis());

        if (!snap.manualOverrideActive) {
            gRelays.apply(out);
        }
        gAlarmIndicator.setAlarm(out.gasAlarm);

        gState.with([&](SystemSnapshot& s) { s.outputs = out; });

        if (out.gasAlarm && !gAlarmWasActive) {
            gLogger.log(EventLogger::Level::Error, "gas_alarm_triggered");
        } else if (!out.gasAlarm && gAlarmWasActive) {
            gLogger.log(EventLogger::Level::Info, "gas_alarm_cleared");
        }
        gAlarmWasActive = out.gasAlarm;

        vTaskDelayUntil(&lastWake, period);
    }
}

void taskAlarmWatch(void*) {
    // Faster-cadence safety net on top of TaskControl (thesis Table 2.9:
    // "TaskAlarm ... critical priority, checks emergency thresholds, forced
    // control"): reacts within 500 ms instead of waiting for the 1 s control
    // cycle if gas readings spike between control-task iterations.
    const TickType_t period = pdMS_TO_TICKS(cfg::kAlarmTaskIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        const SystemSnapshot snap = gState.read();
        if (snap.warmupComplete && snap.gasPpm > control::ClimateThresholds{}.gasAlarmPpm) {
            gAlarmIndicator.setAlarm(true);
        }
        vTaskDelayUntil(&lastWake, period);
    }
}

void taskLcd(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kLcdRefreshIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        const SystemSnapshot snap = gState.read();
        if (snap.outputs.gasAlarm) {
            gDisplay.showFireAlarm();
        } else {
            gDisplay.showNextMode(snap, gBluetooth.connected(), gMqtt.connected());
        }
        vTaskDelayUntil(&lastWake, period);
    }
}

void taskBluetooth(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kBluetoothTelemetryIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        gBluetooth.sendSnapshot(gState.read());
        vTaskDelayUntil(&lastWake, period);
    }
}

void taskMqtt(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kMqttTelemetryIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        gMqtt.loop();
        gMqtt.publishSnapshot(gState.read());
        vTaskDelayUntil(&lastWake, period);
    }
}

void taskLogger(void*) {
    const TickType_t period = pdMS_TO_TICKS(cfg::kLogIntervalMs);
    TickType_t lastWake = xTaskGetTickCount();
    for (;;) {
        const SystemSnapshot snap = gState.read();
        char msg[96];
        snprintf(msg, sizeof(msg), "t=%.1f h=%.1f gas=%.0f vent=%d heat=%d hum=%d",
                 snap.temperatureC, snap.humidityPct, snap.gasPpm,
                 snap.outputs.ventilation, snap.outputs.heating, snap.outputs.humidifier);
        gLogger.log(EventLogger::Level::Info, msg);
        vTaskDelayUntil(&lastWake, period);
    }
}

} // namespace

void setup() {
    Serial.begin(115200);

    gAlarmIndicator.begin();
    gRelays.begin();
    if (!gSensors.begin()) {
        Serial.println("WARN: BME280 not responding, continuing in degraded mode");
    }
    gDisplay.begin();
    if (!gLogger.begin()) {
        Serial.println("WARN: LittleFS mount failed, event logging disabled");
    }
    gBluetooth.begin();
    gMqtt.begin();

    // Core 1: sensing and control (thesis Table 2.9 -- highest/critical priority)
    xTaskCreatePinnedToCore(taskAlarmWatch, "TaskAlarm", 4096, nullptr, 6, nullptr, 1);
    xTaskCreatePinnedToCore(taskSensors, "TaskSensors", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(taskControl, "TaskControl", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(taskLcd, "TaskLCD", 4096, nullptr, 3, nullptr, 1);

    // Core 0: connectivity and logging
    xTaskCreatePinnedToCore(taskLogger, "TaskLogger", 4096, nullptr, 1, nullptr, 0);
    xTaskCreatePinnedToCore(taskBluetooth, "TaskBluetooth", 4096, nullptr, 3, nullptr, 0);
    xTaskCreatePinnedToCore(taskMqtt, "TaskMQTT", 8192, nullptr, 3, nullptr, 0);
}

void loop() {
    // All work happens in the FreeRTOS tasks created in setup().
    vTaskDelay(pdMS_TO_TICKS(1000));
}
