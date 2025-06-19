# Firmware

PlatformIO project implementing the ESP32 air-quality monitoring/control system described in [`../docs/thesis.md`](../docs/thesis.md) (sections 2.1–2.4).

## Layout

- `lib/control_logic/` — hardware-independent control logic (hysteresis, MQ-2 warm-up timer, gas alarm state machine, the combined climate controller). No Arduino/ESP32 dependency, so it builds and runs identically on the host and on the target.
- `src/` — ESP32 application: sensor drivers, relay/LCD/buzzer I/O, Bluetooth SPP and MQTT telemetry, LittleFS event logging, and the FreeRTOS task setup (`main.cpp`) implementing the Core 0 / Core 1 split from thesis Table 2.9.
- `test/test_control_logic/` — Unity unit tests for everything in `lib/control_logic/`.

## Building

```sh
pip install platformio

# Build the real firmware for the ESP32 target
pio run -e esp32dev

# Run the hardware-independent unit tests on this machine (no board needed)
pio test -e native

# Flash + monitor (requires a connected ESP32 board)
pio run -e esp32dev -t upload
pio device monitor
```

Both commands above were verified to pass on this machine: `pio run -e esp32dev` links successfully (Flash 81.8%, RAM 18.1% of an esp32dev target with the `min_spiffs` partition table), and `pio test -e native` passes all 11 unit tests. **No physical ESP32 board or sensors were available during development** — the hardware-facing code (sensor drivers, display, relays, radios) is compile-checked but not flash-tested. Treat wiring, I2C addresses, and the MQ-2 ppm conversion in `SensorManager.h` as starting points to verify against your actual hardware.

## Before flashing

- Set real Wi-Fi/MQTT credentials in `src/Config.h` (`kWifiSsid`, `kWifiPassword`, `kMqttBroker`) — don't commit real secrets to a public repo; consider moving them to a gitignored `secrets.h`.
- Confirm the pin assignments in `src/Config.h` against your actual wiring (`hardware/diagrams.drawio`, sheet 3).
- Confirm your BME280 module's I2C address (`0x76` or `0x77`).
- The MQ-2 ppm conversion in `SensorManager::readGasPpm()` is a linear placeholder; the thesis (section 2.2) notes the sensor is accurate to only ±20–30% even after proper calibration — replace it with a calibration curve before relying on absolute ppm values.
