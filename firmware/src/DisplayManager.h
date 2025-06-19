#pragma once

#include <LiquidCrystal_I2C.h>
#include "Config.h"
#include "SharedState.h"

// Cycles through the three display modes from thesis Table 1.1 / section 1.4:
// 1. sensor readings, 2. relay states, 3. wireless-connection status.
class DisplayManager {
public:
    void begin() {
        lcd_.init();
        lcd_.backlight();
    }

    void showNextMode(const SystemSnapshot& s, bool bluetoothConnected, bool mqttConnected) {
        lcd_.clear();
        switch (mode_) {
            case Mode::Readings: renderReadings(s); break;
            case Mode::RelayStates: renderRelayStates(s); break;
            case Mode::ConnectionStatus: renderConnectionStatus(bluetoothConnected, mqttConnected); break;
        }
        mode_ = static_cast<Mode>((static_cast<int>(mode_) + 1) % 3);
    }

    void showFireAlarm() {
        lcd_.clear();
        lcd_.setCursor(0, 0);
        lcd_.print("** FIRE/GAS  **");
        lcd_.setCursor(0, 1);
        lcd_.print("Ventilating...");
    }

private:
    enum class Mode { Readings, RelayStates, ConnectionStatus };

    void renderReadings(const SystemSnapshot& s) {
        lcd_.setCursor(0, 0);
        if (!s.bme280Ok) {
            lcd_.print("BME280 fault!");
        } else {
            lcd_.print("T:");
            lcd_.print(s.temperatureC, 1);
            lcd_.print("C H:");
            lcd_.print(s.humidityPct, 0);
            lcd_.print("%");
        }
        lcd_.setCursor(0, 1);
        if (!s.warmupComplete) {
            lcd_.print("Gas: warming up");
        } else {
            lcd_.print("Gas:");
            lcd_.print(s.gasPpm, 0);
            lcd_.print("ppm");
        }
    }

    void renderRelayStates(const SystemSnapshot& s) {
        lcd_.setCursor(0, 0);
        lcd_.print("Vent:");
        lcd_.print(s.outputs.ventilation ? "ON " : "OFF");
        lcd_.print(" Heat:");
        lcd_.print(s.outputs.heating ? "ON" : "OFF");
        lcd_.setCursor(0, 1);
        lcd_.print("Humid:");
        lcd_.print(s.outputs.humidifier ? "ON" : "OFF");
    }

    void renderConnectionStatus(bool bluetoothConnected, bool mqttConnected) {
        lcd_.setCursor(0, 0);
        lcd_.print("BT: ");
        lcd_.print(bluetoothConnected ? "connected" : "idle");
        lcd_.setCursor(0, 1);
        lcd_.print("MQTT: ");
        lcd_.print(mqttConnected ? "connected" : "offline");
    }

    LiquidCrystal_I2C lcd_{cfg::kLcdAddress, cfg::kLcdCols, cfg::kLcdRows};
    Mode mode_ = Mode::Readings;
};
