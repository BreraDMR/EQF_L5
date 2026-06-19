#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "SharedState.h"

// MQTT/Wi-Fi telemetry for Home Assistant integration (thesis section 2.4):
// publishes each parameter to its own topic under home/airmonitor/*.
class MqttTelemetry {
public:
    void begin() {
        WiFi.begin(cfg::kWifiSsid, cfg::kWifiPassword);
        mqtt_.setClient(wifiClient_);
        mqtt_.setServer(cfg::kMqttBroker, cfg::kMqttPort);
    }

    bool connected() { return mqtt_.connected(); }

    void loop() {
        if (WiFi.status() != WL_CONNECTED) return;
        if (!mqtt_.connected()) {
            mqtt_.connect(cfg::kMqttClientId);
        }
        mqtt_.loop();
    }

    void publishSnapshot(const SystemSnapshot& s) {
        if (!mqtt_.connected()) return;
        publishFloat("home/airmonitor/temperature", s.temperatureC);
        publishFloat("home/airmonitor/humidity", s.humidityPct);
        publishFloat("home/airmonitor/pressure", s.pressureHpa);
        publishFloat("home/airmonitor/gas_ppm", s.gasPpm);
        mqtt_.publish("home/airmonitor/relay/ventilation", s.outputs.ventilation ? "ON" : "OFF");
        mqtt_.publish("home/airmonitor/relay/heating", s.outputs.heating ? "ON" : "OFF");
        mqtt_.publish("home/airmonitor/relay/humidifier", s.outputs.humidifier ? "ON" : "OFF");
        mqtt_.publish("home/airmonitor/alarm", s.outputs.gasAlarm ? "ON" : "OFF");
    }

private:
    void publishFloat(const char* topic, float value) {
        if (isnan(value)) return;
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f", value);
        mqtt_.publish(topic, buf);
    }

    WiFiClient wifiClient_;
    PubSubClient mqtt_;
};
