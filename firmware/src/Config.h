#pragma once

// Pin map and tunables for the air-quality monitor (thesis sections 2.1-2.6).
// Adjust to match your actual wiring before flashing.

namespace cfg {

// I2C (shared bus: BME280 + LCD backpack)
constexpr int kI2cSda = 21;
constexpr int kI2cScl = 22;

// MQ-2 analog input
constexpr int kMq2Pin = 34; // ADC1_CH6

// Relay outputs (active HIGH through opto-isolation, see hardware/diagrams.drawio)
constexpr int kRelayVentilationPin = 25;
constexpr int kRelayHeatingPin = 26;
constexpr int kRelayHumidifierPin = 27;

// Alarm indicators
constexpr int kBuzzerPin = 4;
constexpr int kLedGreenPin = 32; // power
constexpr int kLedRedPin = 33;   // alarm

// LCD (PCF8574 I2C backpack)
constexpr uint8_t kLcdAddress = 0x27;
constexpr uint8_t kLcdCols = 16;
constexpr uint8_t kLcdRows = 2;

// BME280 I2C address (0x76 or 0x77 depending on the module)
constexpr uint8_t kBme280Address = 0x76;

// Timing (thesis section 2.2/2.4)
constexpr uint32_t kSensorPollIntervalMs = 1000;
constexpr uint32_t kControlIntervalMs = 1000;
constexpr uint32_t kLcdRefreshIntervalMs = 2000;
constexpr uint32_t kBluetoothTelemetryIntervalMs = 5000;
constexpr uint32_t kMqttTelemetryIntervalMs = 5000;
constexpr uint32_t kLogIntervalMs = 30000;
constexpr uint32_t kMq2WarmupMs = 120000;
constexpr uint32_t kAlarmTaskIntervalMs = 500;

// Wi-Fi / MQTT (fill in for your deployment; consider moving to NVS/secrets.h
// instead of committing real credentials)
constexpr const char* kWifiSsid = "CHANGE_ME";
constexpr const char* kWifiPassword = "CHANGE_ME";
constexpr const char* kMqttBroker = "homeassistant.local";
constexpr uint16_t kMqttPort = 1883;
constexpr const char* kMqttClientId = "esp32-airmonitor";
constexpr const char* kBluetoothDeviceName = "AirMonitor-ESP32";

} // namespace cfg
