# Development and Implementation of an Automated Microprocessor-Based Air Quality Control System for Residential and Industrial Premises

Qualification work (Fchoval Molodshyi Bakalavr / EQF Level 5), specialty 123 Computer Engineering, study programme "Maintenance of Computer Systems and Networks".

- **Author:** Damir
- **Group:** [redacted]
- **Academic advisor:** [name redacted]
- **Institution:** Y. O. Paton Vocational College of Welding and Electronics
- **Year:** 2026

> This document is an English translation of the original Ukrainian explanatory note ("Пояснювальна записка"). Names of college staff (advisor, reviewers, committee chair) have been redacted; numeric results, formulas, and technical content are preserved unchanged. The three accompanying engineering drawings are translated separately in `hardware/`.

## Abstract

This qualification work is devoted to the development of an automated microprocessor-based system for monitoring and controlling air quality in residential and industrial premises, aimed at ensuring safe occupancy conditions and preventing the accumulation of harmful substances and combustible gases.

The work analyzes modern approaches to monitoring air environment parameters and methods of organizing automatic microclimate control using microcontrollers and sensor networks. A functional structure and a principal electrical schematic were developed, integrating temperature, humidity, and gas-concentration measurement with automatic control of actuators through relay channels.

A system architecture based on the dual-core ESP32 microcontroller with FreeRTOS task distribution is proposed, using BME280 sensors (temperature, humidity, pressure) and MQ-2 sensors (gas analysis), together with the integration of Bluetooth SPP and MQTT wireless protocols for remote monitoring and integration with home-automation systems. A detailed selection and justification of electronic components was carried out, and the circuit design of a relay block with galvanic isolation and RC snubber networks was developed.

Reliability calculations (mean time between failures of 38,168 hours), power-consumption calculations (monitoring-mode draw of ~2.05 W), and a techno-economic efficiency assessment were performed, confirming the feasibility of deploying the system with a payback period of 3 months. An adaptive hysteresis algorithm for actuator control was implemented, along with FreeRTOS-based firmware that logs critical events to LittleFS.

Particular attention is paid to fire safety and emergency response upon detection of combustible-gas concentrations above permissible thresholds, ensuring automatic activation of ventilation within 100 ms.

The occupational-safety section covers requirements for organizing safe conditions during the system's development and installation, including electrical safety, ESD protection, sanitary norms for soldering work, and fire-prevention requirements.

The results of this work can be used in the development of microclimate control systems for Smart Home, office, and industrial automation segments, as well as for emergency alarm and incident-prevention purposes in premises of various functional uses.

**Keywords:** ESP32, microcontroller, air quality monitoring, MQ-2, BME280, automation, FreeRTOS, MQTT, Bluetooth, air quality, reliability.

## Table of Contents

1. [General Part](#1-general-part)
   1. Purpose and application of the air-quality control system
   2. Technical specifications of the system
   3. Comparative analysis with similar systems
   4. Operating principle of the system
2. [Special Part](#2-special-part)
   1. Purpose and functional structure of the ESP32-based system
   2. Technical specifications and system requirements
   3. Justification of the component base
   4. Operating principle and firmware
   5. Reliability and power-consumption calculations
   6. Calculation of power switching elements
3. [Operational Part](#3-operational-part)
   1. Hardware-based health-monitoring methods
   2. Technical means of fault finding and repair
   3. Software diagnostic tools
4. [Economic Part](#4-economic-part)
   1. Determining the system's cost
   2. Efficiency assessment
5. [Occupational Safety](#5-occupational-safety)
   1. Analysis of hazardous and harmful production factors
   2. Safety measures
   3. Fire safety
6. [Conclusions](#conclusions)
7. [References](#references)

## Introduction

Monitoring air-environment parameters in residential, office, and industrial premises is a mandatory condition for their safe operation under DBN B.2.5-67:2013 (Ukrainian building code for heating, ventilation, and air conditioning) and the international standard ISO 16000. These documents regulate permissible concentrations of harmful substances, temperature-humidity regimes, and ambient pressure. However, most existing installations are limited to stand-alone sensors without centralized data processing, automatic response, or remote monitoring. In practice, most installed systems consist of disconnected sensors: temperature is measured in one place, ventilation is switched on manually elsewhere. This qualification work solves a specific task — to combine measurement, decision-making, and control in a single ESP32-based device.

The **goal** of the qualification work is to develop an automated microprocessor-based system for monitoring and controlling the microclimate and safety of the air environment, based on the ESP32 microcontroller, with wireless-communication support and adaptive actuator control.

The **object of study** is the processes of automatic control of air-environment parameters and microclimate-system control in premises of various functional purposes.

The **subject of study** is the hardware and software means of implementing the system based on the ESP32 microcontroller using FreeRTOS, BME280 and MQ-2 sensors, and the Bluetooth and MQTT protocols.

To achieve the goal, the following tasks were addressed:

- analysis of the regulatory framework (DBN B.2.5-67:2013, ISO 16000) and a review of existing solutions;
- selection and justification of the component base: the ESP32 microcontroller, the BME280 sensor, the MQ-2 gas analyzer;
- development of the hardware part: a relay block with opto-isolation, RC snubber networks, a 16×2 LCD display;
- development of firmware on FreeRTOS with task distribution between the two ESP32 cores: Bluetooth/MQTT processing on Core 0, sensor polling and relay control on Core 1;
- implementation of an algorithm for stabilizing MQ-2 readings (120 s warm-up mode) and software hysteresis;
- organizing logging of critical events to SPIFFS/LittleFS and telemetry transmission via Bluetooth;
- reliability and power-consumption calculations and a techno-economic efficiency assessment.

The assembled and debugged prototype passed functional testing: the ventilation relay was confirmed to trigger when the temperature threshold was exceeded, and gas contamination was detected by the MQ-2 sensor after a 120-second warm-up. The response time to an emergency event is no more than 5 seconds. The implemented adaptive energy-saving strategy reduces actuator-maintenance costs. The developed system can be applied in the Smart Home, office, and industrial automation segments to prevent emergencies related to the accumulation of combustible gases and smoke.

## 1. General Part

### 1.1 Purpose and Application of the Air Quality Control System

The automated microprocessor-based air-quality control system is designed for continuous monitoring of key air-environment parameters and automatic regulation of their values by controlling actuators. The main goal is to maintain comfortable and safe occupancy conditions, prevent the accumulation of harmful substances, reduce fire-safety risk, and optimize energy consumption.

The system controls the following parameters:

- concentration of combustible gases, smoke, and carbon monoxide (MQ-2 sensor, range 200–10,000 ppm, sensitive to LPG, propane, methane, hydrogen, alcohol, smoke);
- air temperature (range 0…+85 °C, accuracy ±2 °C);
- relative air humidity (range 20…90 %, accuracy ±5 %).

According to DBN B.2.5-67:2013, the optimal indoor temperature is 18–28 °C, relative humidity 30–60 %. The system is adaptable to different object types:

- residential premises (apartments, offices): control of compact extractor fans up to 500 W, humidifiers, heaters, air conditioners, and ELTRAL KS 30/40 or IBFM 325/326 chain window actuators;
- industrial premises (warehouses, workshops): control of industrial extractor fans up to 10 kW, large heaters, and air conditioners.

*Figure 1.1 — Keyestudio MQ-2 sensor.*
*Figure 1.2 — First power-class relay.*
*Figure 1.3 — Increased-power relay.*

The difference between residential and industrial configurations lies solely in the relay power rating and the connected actuators — the system issues a signal to the corresponding relay regardless of load type. For larger premises, scaling is provided by adding independent zones with separate sensors and relays.

Main problems solved by the system:

- preventing the accumulation of combustible gases and smoke — upon a critical rise in concentration, the buzzer activates and a "Fire" message is shown on the display;
- maintaining comfortable temperature (18–26 °C) and humidity (40–60 % RH) by automatically switching the corresponding devices on or off;
- reducing energy costs through cascaded activation of ventilation means;
- reliability: wired connection of sensors and relays, power supply for logging and data transmission during a network outage.

### 1.2 Technical Specifications of the System

*Figure 1.4 — ESP32 module.*

**Table 1.1 — Full technical specifications of the microclimate monitoring system**

| Module / Category | Parameter | Technical specification |
|---|---|---|
| 1. Central processing module | Microcontroller | ESP32 (dual-core Xtensa LX6, up to 240 MHz) |
| | Resource (core) allocation | Core 1 — sensor data processing and relay control; Core 0 — Bluetooth communication |
| | Interfaces and memory | Bluetooth Classic/BLE, Wi-Fi, SPIFFS for the logging file system |
| | Power consumption | Active mode with Bluetooth: 70–140 mA (peak up to 240 mA); standby (no active transmission): ~40–70 mA |
| 2. Sensors and monitored parameters | Gas/smoke/CO sensor | MQ-2 (Keyestudio or equivalent). Detection range: 200–10,000 ppm (LPG, propane, methane, hydrogen, CO, smoke, alcohol). Accuracy: ±20–30 % (after calibration). Supply voltage: 5 V. Sensing layer: SnO2 |
| | Temperature/humidity sensor | BME280. Temperature: 0…+85 °C (accuracy ±2 °C, resolution 1 °C). Humidity: 20…90 % RH (accuracy ±5 % RH, resolution 1 % RH). Response time: temperature up to 10 s, humidity up to 6 s. Sampling interval: recommended 1–5 s (30 s in this system) |
| 3. Actuators and control | Relay groups | 3 independent groups (ventilation, humidification, heating) |
| | Relay load power | Residential use: up to 500 W (average power). Industrial use: up to 10 kW (with power margin) |
| | Chain window actuator | ELTRAL KS 30/40 or IBFM 325/326. Rated voltage: 110/230 V AC. Pull/push force: 300 N. Current draw: 0.12 A. Travel speed: 12 mm/s. Adjustable stroke: 100–400 mm. Control principle: pulsed (voltage applied → state change). Protection class: typically IP44 or higher |
| 4. Indication and interface | Display | 16×2 LCD (I²C or parallel connection) |
| | Display modes | 3 modes: 1. Sensor readings 2. Relay states 3. Wireless-connection status |
| | Alarm signaling | Piezo buzzer (activates on critical gas contamination) + "Fire" message on display |
| 5. Power and reliability | Primary power | Power supply unit (5 V / 3.3 V outputs for ESP32, relays, and sensors) |
| | Backup power | 18650 Li-Ion battery + TP4056 charge module (sustains logging and Bluetooth transmission during a mains outage) |
| | Operating conditions | System operating temperature: −5…+65 °C |
| | Component reliability | Sensor service life: 5–10 years (with periodic MQ-2 recalibration) |
| 6. Operating modes and logging | Polling interval | 30 seconds (sensor polling and SPIFFS logging) |
| | Data logging | Automatic recording of sensor readings and relay states to internal SPIFFS memory |
| | Wireless transmission | Broadcast via Bluetooth as a text string: `Gas contamination: xxx / Temperature: xx °C / Humidity: xx % / Relay1: ON/OFF` etc. |
| | Upgradeability | Scalable: new zones (sensors + relays) can be added without changing the base control algorithm |

These specifications provide a balance between cost, ease of implementation, and functionality, making the system suitable for both residential and limited industrial use with the reliability of a wired connection and local emergency signaling.

### 1.3 Comparative Analysis with Similar Systems

The developed system is compared with commercial solutions and DIY projects based on ESP32. Main competitors: AirGradient ONE (Europe/Thailand, open-source), Atmotube PRO 2 (USA/Latvia), Airthings View Plus (Norway/USA), and typical DIY ESP32 projects with CCS811/BME280 sensors.

The key distinction of the developed system is direct control of actuators through three relay groups (ventilation, humidification, heating), whereas most commercial competitors are limited to passive monitoring and notifications. Commercial systems offer more precise sensors (NDIR for CO2, Plantower for PM2.5/PM10), but lack built-in active control of extractor fans or windows.

The component cost of the developed system is approximately UAH 500–1500 per base zone, substantially lower than commercial solutions (AirGradient ONE — around $200; Atmotube PRO 2 and Airthings View Plus — $150–300). Advantages: ability to directly control real devices up to 10 kW, full independence of critical functions from wireless networks, backup power, and open-source code. Disadvantages compared to commercial alternatives: limited parameter set (no precise CO2, PM2.5, VOC measurement) and lower MQ-2 sensor accuracy (±20–30 % after calibration).

### 1.4 Operating Principle of the System

The system's operating principle is based on cyclically reading sensor data, processing it in real time on the central microcontroller, comparing it with threshold values, and automatically or manually controlling actuators. The system runs in a continuous loop with a 30-second sensor-polling period.

The structural diagram of the system contains the following main blocks:

- **sensor block:** MQ-2 (analog output) — detects combustible gases, smoke, and CO; BME280 (I2C) — measures temperature and relative humidity;
- **central processing block:** the ESP32 microcontroller (Xtensa LX6, two cores). Core 1 — reads sensors, compares with thresholds, controls relays, updates the display, writes logs to SPIFFS. Core 0 — maintains Bluetooth, receives commands from the mobile app, transmits telemetry;
- **actuator block:** three relay groups (ventilation, humidification, heating). Control algorithm — when a parameter deviates from the norm, the relay switches on; if the parameter has not normalized after 5 minutes, an additional notification is sent;
- **indication and alarm block:** 16×2 LCD (3 display modes); piezo buzzer (activates when MQ-2 readings rise critically);
- **power block:** primary — from a PC ATX power supply (5 V / 3.3 V); backup — an 18650 Li-Ion battery via a TP4056 module;
- **transmission and logging block:** logs to SPIFFS every 30 seconds; transmits via Bluetooth to a mobile app; outputs to a USB serial port.

General operating logic: sensors → ESP32 (read → compare with thresholds → decide) → relay control → actuators. In parallel: data → display / Bluetooth / SPIFFS / buzzer (on alarm).

*Figure 1.5 — Structural diagram of the system.*

## 2. Special Part

### 2.1 Purpose and Functional Structure of the ESP32-Based System

The system is designed for continuous automatic control of air-environment parameters in premises ranging from residential apartments to small industrial facilities. The main task is full microclimate management: the system independently decides whether to switch actuators (ventilation, heating, humidification) on or off, depending on the state of the environment. At the same time, it detects emergency concentrations of combustible gases and smoke. Compliance with DBN B.2.5-67:2013 and ISO 16000 is a mandatory condition for the safe operation of any facility.

The system architecture is built on a hierarchical, four-level principle:

- **sensor level.** BME280 (I2C): temperature −40…+85 °C (±0.5 °C), humidity 0–100 % (±3 %), pressure 300–1100 hPa. MQ-2 gas analyzer (analog input): range 200–10,000 ppm for LPG, propane, methane, smoke;
- **processing and control level.** The ESP32 microcontroller with FreeRTOS: Core 0 — Bluetooth SPP and MQTT for Home Assistant; Core 1 — sensor polling, relay control algorithms, processing of alarm interrupts;
- **actuator level.** A relay block with opto-isolation (galvanic isolation between the control and power sections). Each channel is equipped with an RC snubber network to suppress switching interference when working with inductive loads;
- **display and data-transmission level.** A 16×2 LCD (I2C) for local indication. Telemetry, alarm messages, and logs are transmitted via Bluetooth to a smartphone. Critical events are stored in LittleFS.

*Figure 2.1 — Functional structure of the ESP32-based microclimate monitoring and control system.*

**Table 2.1 — Functional blocks of the system and their purpose**

| Block name | Component base | Main function | Communication interface |
|---|---|---|---|
| Physical-parameter measurement block | BME280 | Measures temperature, humidity, pressure | I2C |
| Gas analysis block | MQ-2 | Detects combustible gases and smoke | ADC (analog input) |
| Central processing module | ESP32 (FreeRTOS) | Computation, control, logging | — |
| Power switching block | Opto-isolated relays | Controls ventilation, heating, humidification | GPIO (digital output) |
| Local indication block | 16×2 LCD (I2C) | Displays current parameters | I2C |
| Wireless communication block | ESP32 BT / Wi-Fi | Transmits telemetry, MQTT, alarm notifications | Bluetooth SPP, MQTT |

The firmware logic implements specific operating modes. On every power-up, the system enters a 120-second Warm-up mode, during which the MQ-2 sensor heats up and stabilizes. Until this interval ends, gas-analyzer readings are not used for relay control. Software hysteresis prevents frequent relay switching: the system turns an actuator on when the upper threshold is exceeded and turns it off only after the reading drops below the lower threshold.

### 2.2 Technical Specifications and System Requirements

The main regulatory document governing microclimate in Ukraine is DBN B.2.5-67:2013: the temperature in the warm season must not exceed 28 °C; in the cold season, it must stay within 18–22 °C; relative humidity must be 30–60 %. ISO 16000 sets limit concentrations for chemical pollutants and combustible gases. The lower explosive limit (LEL): methane — 5 % vol., propane — 2.1 %, LPG — 1.5 %. The system triggers at 10–20 % of the LEL (a few hundred ppm), which is why the lower bound of the MQ-2 range starts at 200 ppm.

**Table 2.2 — Technical specifications of the microclimate and air-environment safety monitoring system**

| Parameter | Value | Note |
|---|---|---|
| Microcontroller | ESP32, dual-core Xtensa LX6, 240 MHz | With FreeRTOS support |
| Temperature measurement range | −40…+85 °C | BME280 sensor |
| Temperature measurement error | ±0.5 °C | At 25 °C |
| Humidity measurement range | 0…100 % | BME280 sensor |
| Humidity measurement error | ±3 % | — |
| Pressure measurement range | 300 to 1100 hPa | BME280 sensor |
| Pressure measurement error | ±1 hPa | — |
| Gas-concentration measurement range | 200 to 10,000 ppm | MQ-2 sensor |
| Monitored gases | LPG, propane, methane, smoke | — |
| Gas-sensor warm-up time | 120 seconds | Warm-up mode |
| Number of relay channels | 3 | ventilation, heating, humidification |
| Maximum switched relay voltage | 250 V AC | With opto-isolation |
| Local-indication interface | 16×2 LCD, I2C | — |
| Wireless interface | Bluetooth SPP, MQTT | Home Assistant, smartphone |
| Logging file system | LittleFS (SPIFFS) | Internal ESP32 memory |
| System supply voltage | 5 V (USB) / 3.3 V (internal LDO) | — |
| Projected energy-consumption reduction | 25–40 % | Adaptive energy-saving mode |

**Table 2.3 — Conformance of technical specifications with regulatory requirements**

| Parameter | Regulatory requirement (DBN / ISO 16000) | System range | Conformance |
|---|---|---|---|
| Air temperature | 18…28 °C (DBN B.2.5-67:2013) | −40…+85 °C | Meets with a wide margin |
| Relative humidity | 30…60 % (DBN B.2.5-67:2013) | 0…100 % | Meets |
| Combustible-gas concentration | ≤ 10 % of LEL (ISO 16000) | 200 to 10,000 ppm | Meets |
| Atmospheric pressure | Informational parameter | 300…1100 hPa | Meets |

The actual accuracy of the BME280 may deviate somewhat from its rated values due to microcontroller self-heating and electromagnetic interference. It is recommended to mount the sensor on a remote lead or at a distance from the main board. MQ-2 sensitivity is temperature-dependent: below 10 °C, readings are understated by 15–20 %. The firmware implements a software compensation algorithm based on BME280 data. The system is not a certified metrological instrument but an automation and alarm device with sufficient accuracy to detect deviations beyond permissible limits.

Timing characteristics: BME280 polling — 1 s; MQ-2 polling — 1 s (actual gas-detection resolution — 5–10 s); display refresh — every 2 s; Bluetooth telemetry packet transmission — every 5 s.

*Figure 2.2 — Diagram of the system's measurement ranges compared with regulatory environmental limits.*

### 2.3 Justification of the Component Base

**Microcontroller selection.** Three candidates were considered at the design stage: ESP8266, ESP32, and an Arduino Mega 2560 with an HC-05 module.

**Table 2.4 — Comparative analysis of microcontrollers by key parameters**

| Parameter | ESP8266 | ESP32 (Selected) | Arduino Mega 2560 + HC-05 |
|---|---|---|---|
| Core count | 1 | 2 | 1 |
| Clock speed | 80 MHz | 240 MHz | 16 MHz |
| RAM | 80 KB | 520 KB | 8 KB |
| Wi-Fi | Yes (802.11 b/g/n) | Yes (802.11 b/g/n) | No (external module) |
| Bluetooth | No | Yes (Classic + BLE) | HC-05 only (Classic) |
| ADC | 1 channel (10-bit) | 18 channels (12-bit) | 16 channels (10-bit) |
| I2C | Yes | Yes | Yes |
| RTOS | Limited support | FreeRTOS (built-in) | No |
| Internal file system | SPIFFS | SPIFFS / LittleFS | No |
| Power consumption | ~80 mA | ~160–240 mA | ~93 mA + ~40 mA (HC-05) |
| Approximate cost | ~UAH 80–120 | ~UAH 180–240 | ~UAH 600–800 (combined) |
| Suitability for the project | Limited | Fully suitable | Not suitable |

The ESP8266 has a critical drawback — a single analog input with 10-bit resolution and no hardware Bluetooth. The Arduino Mega 2560 loses out on cost (UAH 600–800 vs. UAH 180–240 for the ESP32), lacks built-in Wi-Fi, and its ATmega2560 processor (16 MHz) is insufficiently powerful for parallel task execution. The ESP32 is the only candidate that fully meets the requirements: dual-core architecture up to 240 MHz, built-in FreeRTOS, 18 ADC channels (12-bit), hardware Bluetooth and Wi-Fi, and the LittleFS file system.

*Figure 2.3 — Appearance of the ESP32 DevKit v1 module.*

**Temperature, humidity, and pressure sensor selection.** DHT22, SHT31, and BME280 were considered.

**Table 2.5 — Comparative analysis of temperature/humidity sensors**

| Parameter | DHT22 | SHT31 | BME280 (Selected) |
|---|---|---|---|
| Parameters measured | Temperature, humidity | Temperature, humidity | Temperature, humidity, pressure |
| Temperature range | −40…+80 °C | −40…+125 °C | −40…+85 °C |
| Temperature accuracy | ±0.5 °C | ±0.3 °C | ±0.5 °C |
| Humidity accuracy | ±2 % | ±2 % | ±3 % |
| Interface | 1-Wire (digital) | I2C / SPI | I2C / SPI |
| Response time | 2 seconds | 0.5 second | 1 second |
| Pressure measurement | No | No | Yes (300–1100 hPa) |
| Approximate cost | ~UAH 40–60 | ~UAH 160–240 | ~UAH 80–160 |
| Suitability for the project | Satisfactory | Good | Optimal |

The BME280 was selected for its I2C support (shared bus with the LCD display) and the presence of an atmospheric-pressure channel. The SHT31 outperforms the BME280 in accuracy but does not measure pressure and costs substantially more.

*Figure 2.4 — Appearance of the BME280 module on its board.*

**Gas-analyzer selection.** MQ-2, MQ-5, and MQ-135 were considered.

**Table 2.6 — Comparative analysis of MQ-series gas sensors**

| Parameter | MQ-2 (Selected) | MQ-5 | MQ-135 |
|---|---|---|---|
| Monitored gases | LPG, propane, methane, smoke, hydrogen | LPG, methane, town gas | NH3, NOx, CO2, smoke, benzene |
| Sensitivity to combustible gases | High | High | Low |
| Sensitivity to smoke | Yes | No | Partial |
| Concentration range | 200–10,000 ppm | 200–10,000 ppm | 10–300 ppm (CO2 up to 10,000) |
| Warm-up time | 20–30 s (stabilization 120 s) | 20 seconds | 20 seconds |
| Heater voltage | 5 V | 5 V | 5 V |
| Approximate cost | ~UAH 35–60 | ~UAH 45–60 | ~UAH 45–60 |
| Suitability for the project | Optimal | Partial | Not suitable |

The MQ-2 was selected for its widest range of monitored hazardous gases and its response to smoke. The MQ-5 does not respond to smoke; the MQ-135 is oriented toward environmental monitoring and is unsuitable for fire-safety tasks.

*Figure 2.5 — Appearance of the MQ-2 module and the operating principle of a resistive gas sensor.*

**Wireless-protocol selection.** The system uses two channels: Bluetooth SPP — a direct link to a smartphone for displaying telemetry and alarm notifications without requiring network infrastructure; MQTT over Wi-Fi — integration with Home Assistant through a publish/subscribe model. MQTT was chosen as the de-facto IoT standard: a minimum packet header of 2 bytes, and a QoS mechanism that guarantees delivery of critical alarm messages.

**Table 2.7 — Comparative analysis of data-transmission protocols for IoT**

| Parameter | MQTT (Selected) | HTTP REST | WebSocket | CoAP |
|---|---|---|---|---|
| Interaction model | Publish/subscribe | Request/response | Bidirectional | Request/response |
| Header overhead | Minimal (2 bytes) | Large (hundreds of bytes) | Medium | Minimal |
| Broker support | Yes (Mosquitto) | No | No | No |
| Delivery quality (QoS) | 3 levels (0, 1, 2) | No | No | 2 levels |
| Home Assistant support | Built-in | Limited | Limited | No |
| Implementation complexity | Low | Medium | Medium | Medium |
| Suitability for the project | Optimal | Satisfactory | Satisfactory | Not suitable |

*Figure 2.6 — Wireless-communication architecture of the system: Bluetooth for local monitoring and MQTT/Wi-Fi for remote integration.*

### 2.4 Operating Principle and Firmware

The system operates as a closed-loop controller: every 30 s, sensor readings are taken, compared with threshold values, and relay commands are generated. When a parameter deviates from the norm, the system switches on the corresponding actuator; after normalization, it switches it off, accounting for hysteresis.

**Initial start-up and warm-up mode.** After power-up, the microcontroller initializes the I2C bus and checks for the presence of the BME280 and the display. If there is no response, the system enters a protective mode with relays disabled. LittleFS is then initialized, FreeRTOS tasks are started, and the system enters a 120-second Warm-up mode. During this time, MQ-2 readings are flagged in the telemetry, relays driven by MQ-2 signals are blocked, but they remain active for BME280-driven signals.

The flowchart of the main monitoring-system algorithm is shown on drawing FKZE.123OKSM0.KRE6.

*Figure 2.7 — Flowchart of the main operating algorithm of the monitoring system.*

**Automatic control mode.** The system controls four independent relay channels (ventilation, humidification, heating, cooling). Hysteresis prevents frequent switching: the system turns a relay on when the upper threshold is exceeded and turns it off only after the reading drops below the lower threshold. If a critical gas threshold is exceeded, regardless of the current control mode, ventilation switches on, heating elements are blocked, an alarm message is displayed, and a notification is sent to the smartphone. Exiting the alarm state requires the gas concentration to remain at a safe level for at least 30 seconds.

**Table 2.8 — Automatic actuator-control logic**

| Channel | Controlled parameter | Switch-on condition | Switch-off condition | Additional condition |
|---|---|---|---|---|
| Ventilation | Temperature / gas concentration | Temperature > Tmax threshold OR gas > alarm threshold | Temperature < Tmin threshold AND gas normal | Forced activation on alarm gas concentration regardless of temperature |
| Heating | Temperature | Temperature < Tmin threshold | Temperature > Tmax threshold | Blocked while ventilation is in alarm mode |
| Humidifier | Relative humidity | Humidity < Hmin threshold | Humidity > Hmax threshold | Blocked on critical gas concentration |

*Figure 2.8 — Timing diagram of the hysteresis algorithm for the temperature-based ventilation control channel.*

**FreeRTOS task distribution.** Core 1 runs the sensor-polling and control task at the highest priority. Core 0 services Bluetooth and MQTT. Exchange between tasks on different cores occurs through FreeRTOS message queues protected by mutexes.

**Table 2.9 — FreeRTOS task distribution between the ESP32 cores**

| Core | FreeRTOS task | Priority | Execution interval | Function |
|---|---|---|---|---|
| Core 1 | TaskAlarm | Critical (6) | 500 ms | Checks alarm thresholds, forced control |
| Core 1 | TaskSensors | Highest (5) | 1000 ms | Polls BME280 and MQ-2, filters, queues data |
| Core 1 | TaskControl | Highest (5) | 1000 ms | Analyzes data, hysteresis, relay control |
| Core 1 | TaskLCD | Medium (3) | 2000 ms | Updates the display |
| Core 0 | TaskLogger | Low (1) | On event | Writes critical events to LittleFS |
| Core 0 | TaskBluetooth | Medium (3) | 5000 ms | Builds and sends a telemetry packet over BT |
| Core 0 | TaskMQTT | Medium (3) | 5000 ms | Publishes data to Home Assistant topics |

**Monitoring via Bluetooth and MQTT.** Bluetooth SPP transmits telemetry as text every 5 seconds. MQTT publishes each parameter to a separate topic: `home/airmonitor/temperature`, `home/airmonitor/humidity`, `home/airmonitor/relay/ventilation`, etc. Home Assistant subscribes to these topics through its built-in MQTT component and displays all parameters on a dashboard. The alarm state is published to the `home/airmonitor/alarm` topic. Manual mode allows any relay channel to be forced on or off via Home Assistant or Bluetooth.

*Figure 2.9 — Information-flow diagram between system components in automatic and manual control modes.*

### 2.5 Reliability and Power-Consumption Calculations

**System reliability.** The calculation was performed using the failure-rate summation method. Total failure rate:

```
λc = Σ λi × ni                                    (2.1)
```

where λc — total failure rate, 1/h; λi — base failure rate of type-i elements, 1/h; ni — number of type-i elements.

Mean time between failures:

```
Tavg = 1 / λc                                      (2.2)
```

Probability of failure-free operation:

```
P(t) = e^(−λc × t)                                  (2.3)
```

where t — the specified failure-free operating time, h.

Temperature coefficient Kt = 1 (conditions 20–25 °C). Load coefficients Kload are set individually for each group.

**Table 2.10 — Input data for the system reliability calculation**

| Component | Qty (ni) | Kt | Kload | Failure rate (λi·10⁻⁶), 1/h | Group failure rate (ni·λi·10⁻⁶), 1/h |
|---|---|---|---|---|---|
| ESP32 microcontroller | 1 | 1 | 0.7 | 0.50 | 0.50 |
| BME280 sensor | 1 | 1 | 0.7 | 0.35 | 0.35 |
| MQ-2 gas analyzer | 1 | 1 | 0.8 | 1.20 | 1.20 |
| 16×2 LCD display | 1 | 1 | 0.7 | 0.80 | 0.80 |
| Optocoupler (relay block) | 3 | 1 | 0.8 | 0.40 | 1.20 |
| Electromagnetic relay | 3 | 1 | 0.8 | 2.00 | 6.00 |
| Resistors | 12 | 1 | 0.9 | 0.67 | 8.04 |
| Electrolytic capacitors | 4 | 1 | 0.8 | 0.80 | 3.20 |
| Ceramic capacitors | 6 | 1 | 0.8 | 0.50 | 3.00 |
| Protection diodes | 3 | 1 | 0.9 | 0.40 | 1.20 |
| AMS1117 voltage regulator | 1 | 1 | 0.7 | 0.45 | 0.45 |
| Connectors | 5 | 1 | 0.8 | 0.01 | 0.05 |
| Soldered joints | 68 | 1 | 0.4 | 0.003 | 0.204 |
| **TOTAL** | — | — | — | — | **26.194** |

Calculation results:

- λc = 26.20 × 10⁻⁶ 1/h
- Tavg = 38,168 h
- P(1000) = 0.974; P(5000) = 0.877; P(8760) = 0.795

**Table 2.11 — Probability of failure-free operation for various time intervals**

| Time interval, h | Real-world equivalent | P(t) | Rating |
|---|---|---|---|
| 1000 | ~42 days | 0.974 | Excellent |
| 2000 | ~83 days | 0.949 | Excellent |
| 5000 | ~7 months | 0.877 | Good |
| 8760 | 1 year | 0.795 | Satisfactory |
| 17520 | 2 years | 0.632 | Acceptable |
| 38168 | Tavg (mean time to failure) | 0.368 | Boundary value |

*Figure 2.10 — Graph of the probability of failure-free operation P(t) versus operating time.*

The mean time between failures of 38,168 h exceeds the standard for consumer electronics (10,000–20,000 h). The largest contributors to the total failure rate are resistors (~31 %) and electromagnetic relays (~23 %). The SRD-05VDC relay contact life is 100,000 to 500,000 operations.

**Safety analysis.** Three main risks were identified in the circuit:

- mains voltage of 220 V — neutralized by galvanic isolation via a PC817 optocoupler (isolation dielectric strength 5000 V);
- elevated temperature of the MQ-2 heating element (up to 60 °C) — neutralized by the sensor's structural placement with sufficient clearance;
- electromagnetic interference during switching — suppressed by RC snubber networks.

**Table 2.12 — Safety-risk analysis of the electronic module and mitigation measures**

| Risk | Source | Consequences | Protective measure | Effectiveness |
|---|---|---|---|---|
| Electric shock | 220 V relay power circuits | Hazard to maintenance personnel | PC817 optocoupler (5000 V isolation) | High |
| MQ-2 sensor overheating | 0.75 W heating element | Ignition of the housing or nearby components | Structural clearance, heat-resistant housing material | High |
| Switching interference | Relay switching of inductive loads | Microcontroller glitches, false sensor readings | RC snubber on each relay channel | High |
| Power-supply overvoltage | Back-EMF from the relay coil | Breakdown of transistors and optocouplers | Protective diodes in parallel with coils | High |
| Electromagnetic interference | External sources (Wi-Fi, household appliances) | Unstable sensor readings | Filter capacitors, proper PCB routing | Medium |
| Condensation from rapid temperature change | Transition between operating conditions | Short circuits on the board | Protective conformal coating | Medium |

**Power-consumption calculation.** Total power:

```
P = U × I                                          (2.4)
Ptotal = P1 + P2 + ... + Pn                        (2.5)
```

**Table 2.13 — Input data for the system power-consumption calculation**

| Component | Qty | Supply voltage, V | Current per unit, mA | Total current, mA | Power, mW |
|---|---|---|---|---|---|
| ESP32 microcontroller (Wi-Fi + BT) | 1 | 3.3 | 240 | 240 | 792 |
| BME280 sensor (measuring) | 1 | 3.3 | 3.6 | 3.6 | 11.9 |
| MQ-2 gas analyzer (heater) | 1 | 5.0 | 150 | 150 | 750 |
| MQ-2 gas analyzer (measuring) | 1 | 5.0 | 10 | 10 | 50 |
| 16×2 LCD with backlight | 1 | 5.0 | 60 | 60 | 300 |
| I2C bridge module (PCF8574) | 1 | 3.3 | 2.5 | 2.5 | 8.3 |
| PC817 optocoupler (control circuit) | 3 | 3.3 | 5 | 15 | 49.5 |
| AMS1117-3.3 regulator | 1 | 5.0 | 5 | 5 | 25 |
| Status indicator LEDs | 2 | 3.3 | 10 | 20 | 66 |
| SRD-05VDC relay coil (active) | 1–3 | 5.0 | 72 | 72–216 | 360–1080 |
| **TOTAL (monitoring mode)** | — | — | — | **506.1** | **2012.7** |
| **TOTAL (full active mode)** | — | — | — | **722.1** | **3610.5** |

Baseline mode (all relays off):

```
Pbase = 792 + 11.9 + 750 + 50 + 300 + 8.3 + 49.5 + 25 + 66 = 2052.7 mW ≈ 2.05 W
```

Active control mode (all three relays on):

```
Pmax = 2052.7 + 3 × 360 = 3132.7 mW ≈ 3.13 W
```

Power-supply rating with a 1.3 safety margin:

```
Psupply = Pmax × Kmargin = 3.13 × 1.3 = 4.07 W                (2.6)
```

A standard 5 V / 1 A (5 W) USB adapter was selected.

The adaptive energy-saving strategy reduces average consumption by 25–40 %: the LCD backlight switches off after 60 s of inactivity (−300 mW, −14.6 % of baseline consumption); the nighttime sensor-polling rate drops from 1 to 5 s; Wi-Fi power is reduced between MQTT sessions.

Average consumption with the adaptive mode (8 active hours + 16 night hours):

```
Pavg = (Pbase × 8 + Peco × 16) / 24                          (2.7)
Peco = 2.05 × 0.7 = 1.44 W
Pavg = (2.05 × 8 + 1.44 × 16) / 24 = 1.64 W
```

Annual consumption:

```
Wyear = Pavg × 8760 = 1.64 × 8760 = 14,366 W·h = 14.37 kW·h    (2.8)
```

*Figure 2.11 — Diagram of power-consumption distribution among system nodes in baseline monitoring mode.*

### 2.6 Calculation of Power Switching Elements

**Relay-block composition and structure.** The block contains three identical, independent channels. Each channel consists of: a GPIO current-limiting resistor; a PC817 optocoupler (galvanic isolation); an SRD-05VDC-SL-C relay (direct load switching); a protective network — a back-EMF suppression diode + an RC snubber.

*Figure 2.12 — Schematic of one relay-block channel with opto-isolation and a snubber network.*

**R1 calculation (optocoupler current limiting).** PC817 rated characteristics: Uled = 1.2 V, IF = 10 mA (nominal), IFmax = 50 mA. Take IF = 10 mA.

Voltage across the resistor:

```
UR1 = UGPIO − Uled = 3.3 − 1.2 = 2.1 V                       (2.9)
```

R1 resistance:

```
R1 = UR1 / IF = 2.1 / 0.010 = 210 Ω                          (2.10)
```

From the E24 series, R1 = 220 Ω was selected. Actual current:

```
IFactual = UR1 / R1 = 2.1 / 220 = 9.55 mA                    (2.11)
```

R1 power dissipation:

```
PR1 = UR1 × IFactual = 2.1 × 0.00955 = 0.020 W = 20 mW       (2.12)
PR1actual = 20 mW < PR1rated = 125 mW — condition satisfied   (2.13)
```

A MLT-0.125 / 220 Ω / 5 % resistor was selected.

**PC817 operating-mode calculation.** Current through the SRD-05VDC relay coil:

```
Icoil = Ucoil / Rcoil = 5.0 / 69.4 = 72 mA                   (2.14)
```

The PC817's ICmax = 50 mA < 72 mA — a BC547 amplifying transistor stage is required.

BC547 rated data: ICmax = 100 mA; UCEmax = 45 V; h21Emin = 110.

Base current for saturation:

```
IBmin = IC / h21Emin = 72 / 110 = 0.655 mA                   (2.15)
```

Base resistor:

```
URbase = Uoptout − UBE = 5.0 − 0.7 = 4.3 V                   (2.16)
```

Taking IB = 2 mA (two-fold margin):

```
Rbase = URbase / IB = 4.3 / 0.002 = 2150 Ω                   (2.17)
```

From the E24 series, Rbase = 2.2 kΩ (MLT-0.125) was selected.

```
PRbase = 4.3 × 0.002 = 8.6 mW < 125 mW — condition satisfied  (2.18)
```

**SRD-05VDC-SL-C relay selection.** Rated load current for a 150 W fan:

```
Iload = Pload / Umains = 150 / 220 = 0.682 A                 (2.19)
```

Maximum inrush current (5× rated):

```
Iinrush = 5 × Iload = 5 × 0.682 = 3.41 A                     (2.20)
```

The SRD-05VDC's maximum allowable current of 10 A leaves an adequate current margin.

**RC snubber network calculation.** For a 100–150 W fan motor: L ≈ 0.5 H, Rload = 200 Ω. Taking Csn = 100 nF (class X2):

```
Rsn = 2 × √(L / Csn) = 2 × √(0.5 / 100×10⁻⁹) = 4472 Ω         (2.21)
```

From the E24 series, Rsn = 4.7 kΩ (MLT-0.5) was selected. Capacitor: K73-17, 100 nF / 275 V AC.

Energy dissipated during switching:

```
Wsn = Csn × U² / 2 = 100×10⁻⁹ × 220² / 2 = 2.42 mJ            (2.22)
```

**Table 2.14 — Summary of relay-block component calculations and selections**

| Element | Calculated value | Selected component | Reliability margin |
|---|---|---|---|
| Optocoupler current-limiting resistor R1 | 210 Ω, 20 mW | MLT-0.125, 220 Ω, 5 % | 6.3× on power |
| Optocoupler | IC up to 50 mA | PC817 (ICmax = 50 mA) | Used with an amplifier |
| Current-amplifier transistor | IC = 72 mA, IB = 2 mA | BC547, ICmax = 100 mA | 1.4× on current |
| Transistor base resistor | 2150 Ω, 8.6 mW | MLT-0.125, 2.2 kΩ, 5 % | 14.5× on power |
| Electromagnetic relay | Iload = 0.682 A (3.41 A inrush) | SRD-05VDC-SL-C (10 A) | 2.9× on current (inrush) |
| Relay-coil protection diode | I = 72 mA, Urev up to 5 V | 1N4007 (1 A, 1000 V) | 13.9× on current |
| Snubber resistor | 4472 Ω, pulsed | MLT-0.5, 4.7 kΩ, 5 % | — |
| Snubber capacitor | 100 nF, 220 V | K73-17, 100 nF, 275 V (X2) | 1.25× on voltage |

## 3. Operational Part

### 3.1 Hardware-Based Health-Monitoring Methods

Three main hardware-monitoring mechanisms are used in the developed system. The hardware watchdog timer (HWDT) is a built-in ESP32 peripheral block that operates independently of the processor cores. A two-tier architecture is implemented: the Task Watchdog Timer (TWDT) monitors FreeRTOS tasks, and the Interrupt Watchdog monitors the state of interrupt handlers. The HWDT timeout is set to 8 seconds — sufficient for sensor polling and Bluetooth transmission without false triggers.

The Brownout Detector (BOD) continuously compares VDD with a threshold value. The threshold is set to 2.6 V — if the voltage sags below this value, a controlled reset is generated to prevent LittleFS data corruption.

**Sensor-line integrity monitoring.** For the BME280 (I2C): 4.7 kΩ pull-up resistors keep the bus at a logic-high level; a broken line returns an ACK error, which the hardware I2C controller detects. For the MQ-2 (ADC): a zero reading indicates a broken signal line, while a maximum reading under normal conditions indicates a short to the supply rail. Both states are flagged as hardware faults, indicated on the LCD, and reported as an alarm message over Bluetooth.

*Figure 3.1 — Structural diagram of the hardware-monitoring tools interacting with the microcontroller.*

### 3.2 Technical Means of Fault Finding and Repair

Troubleshooting is performed sequentially — from a visual inspection to instrumental testing.

**Visual inspection (the first mandatory step):** checking IC packages and passive components for cracks, overheating marks, or charring; checking the condition of the ESP32 antenna, the crystal oscillator, and the USB connector; checking transistor switches, resistive dividers, and optocouplers.

**Instrumental testing** is performed with a digital multimeter (continuity testing with the circuit de-energized; voltage measurement with power on: 3.3 V on the ESP32 VCC, 5 V on the relay power line; a deviation of more than 5 % from nominal indicates a fault).

**BME280 check:** a logic analyzer or oscilloscope on the I2C bus's SDA/SCL lines (400 kHz, 4.7 kΩ pull-ups). **MQ-2 check:** a multimeter in DC-voltage mode on the analog output (after a 120 s warm-up — a level of 0.3–1.0 V under normal gas conditions).

Repairs are carried out using soldering equipment at 300–320 °C. Faulty modules (ESP32, BME280, MQ-2, relays) are replaced entirely, following a modular-replacement principle. After repair — repeat continuity testing and a functional check.

*Figure 3.2 — Algorithm for sequential fault finding and repair in the system.*

**Table 3.2 — Diagnostic and repair matrix for typical hardware faults**

| System node | Fault symptom | Test instrument | Expected (normal) value | Repair method |
|---|---|---|---|---|
| ESP32 power rail | Voltage below 3.1 V or absent | Digital multimeter (DC V mode) | 3.3 V ± 0.16 V | Replace the regulator, check for a short circuit |
| Relay module | No click on the control signal | Multimeter (continuity mode) | Coil winding resistance, or 5 V on the supply line | Replace the relay module or transistor switch |
| I2C interface (BME280) | Polling hangs, ACK error | Logic analyzer / oscilloscope | Pulses present on SDA and SCL lines | Re-solder the 4.7 kΩ pull-ups, replace the cable |
| Analog path (MQ-2) | Stuck at 0 V or 3.3 V at the ADC input | Digital multimeter (DC V mode) | Dynamic voltage 0.3–1.0 V after 120 s warm-up | Replace the MQ-2 sensor (heater degradation or open circuit) |

### 3.3 Software Diagnostic Tools

The primary tool for initial diagnostics is the Serial Monitor (Arduino IDE or VS Code/PlatformIO) over UART0 at 115200 baud. Structured debug-log output uses INFO, WARN, and ERROR prefixes for normal events, borderline values, and critical failures, respectively.

**Return-code analysis on I2C initialization:** the Adafruit BME280 library's `begin()` function returns `false` if there is no response at address 0x76 or 0x77 — the system enters a protective mode. **MQ-2 analog-channel monitoring:** out-of-range values signal a sensor fault. **Bus check:** via `Wire.beginTransmission()` with completion-code analysis.

**Logging critical events to LittleFS** (ESP32 flash memory): timestamp, event code, text message. The log records alarm triggers, reboots, initialization errors, and HWDT resets. A circular buffer prevents overflow. The log is read via the Serial Monitor or Bluetooth.

**Remote monitoring** — via MQTT (status topics, error codes) and OTA firmware updates over Wi-Fi without physical access to the device.

## 4. Economic Part

### 4.1 Determining the System's Cost

Production cost is calculated in accordance with NP(S)BU 16 "Expenses":

```
PC = DMC + LC + ODC + OPE                          (4.1)
```

where PC — production cost, UAH; DMC — direct material costs; LC — direct labor costs; ODC — other direct costs; OPE — overhead production expenses.

**Direct material costs.** List and cost of components (prices as of 2026, rozetka.ua, aliexpress.com):

**Table 4.1 — Material-cost specification for manufacturing the air-quality control system**

| No. | Component | Qty | Price, UAH | Amount, UAH |
|---|---|---|---|---|
| 1 | Mounting materials and enclosure | | | |
| 1.1 | Universal prototyping board | 2 pcs | 22.00 | 44.00 |
| 1.2 | ABS plastic enclosure 150×100 | 1 pc | 85.00 | 85.00 |
| 1.3 | Hook-up wire (set) | 1 set | 55.00 | 55.00 |
| 1.4 | POS-61 solder (15 g) | 1 pc | 32.00 | 32.00 |
| 1.5 | Rosin flux (10 ml) | 1 pc | 18.00 | 18.00 |
| | **Section 1 subtotal** | | | **234.00** |
| 2 | Electronic components | | | |
| 2.1 | ESP32 DevKit V1 microcontroller | 1 pc | 185.00 | 185.00 |
| 2.2 | BME280 sensor (I2C) | 1 pc | 125.00 | 125.00 |
| 2.3 | MQ-2 gas analyzer | 1 pc | 90.00 | 90.00 |
| 2.4 | 16×2 LCD with I2C | 1 pc | 98.00 | 98.00 |
| 2.5 | 3-channel relay module | 1 pc | 115.00 | 115.00 |
| 2.6 | Resistors (set of 200) | 1 set | 35.00 | 35.00 |
| 2.7 | Capacitors (set of 100) | 1 set | 28.00 | 28.00 |
| 2.8 | AMS1117 voltage regulator | 2 pcs | 8.00 | 16.00 |
| | **Section 2 subtotal** | | | **692.00** |
| 3 | Other hardware | | | |
| 3.1 | 5 V/2 A USB power supply | 1 pc | 135.00 | 135.00 |
| 3.2 | JST connectors (set) | 1 set | 42.00 | 42.00 |
| 3.3 | Screws, standoffs, mounts | 1 set | 24.00 | 24.00 |
| | **Section 3 subtotal** | | | **201.00** |
| | **Total DMC** | | | **1127.00** |

```
DMC = 234.00 + 692.00 + 201.00 = 1127.00 UAH
```

**Direct labor costs.** Hourly wage system:

```
Whourly = Rhourly × T                              (4.2)
Wsupp = Wbase × Ksupp                              (4.3)
```

where Ksupp = 0.15 (15 % of base wages).

```
Wsupp = 3044.00 × 0.15 = 456.60 UAH
SSC = (Wbase + Wsupp) × 0.22                       (4.4)
SSC = (3044.00 + 456.60) × 0.22 = 770.11 UAH
LC = 3044.00 + 456.60 + 770.11 = 4270.71 UAH
```

**Other direct costs.** Electricity costs (400 W × 36 h = 14.4 kW·h × 12.50 UAH/kW·h = 180.00 UAH). Depreciation of test equipment (10 %/month × 2 weeks) ≈ 95.00 UAH.

```
ODC = 180 + 95 = 275.00 UAH
```

Overhead production expenses (30 % of base wages):

```
OPE = 3044 × 0.30 = 913.20 UAH
```

**Table 4.3 — Summary cost calculation**

| No. | Cost item | Amount, UAH | Share, % |
|---|---|---|---|
| 1 | Direct material costs (DMC) | 1127 | 17.42 |
| 2 | Direct labor costs (LC) | 4270.71 | 66.01 |
| 3 | Other direct costs (ODC) | 275 | 2.43 |
| 4 | Overhead production expenses (OPE) | 913.2 | 14.12 |
| | **Production cost (PC)** | **6585.91** | **100** |

```
PC = 1127 + 4270.71 + 275 + 913.20 = 6585.91 UAH
```

**Contract price.** With a 30 % profitability margin:

```
CP = PC × (1 + PM/100)                             (4.5)
CP = 6585.91 × 1.30 = 8561.68 UAH
```

Including 20 % VAT:

```
CP with VAT = 8561.68 × 1.20 = 10274.02 UAH
```

The market price of UAH 10,274.02 is competitive compared with commercial alternatives (UAH 8000–25000), given the system's broader functionality in gas analysis and actuator control.

**Table 4.4 — Project price formation**

| Indicator | Value, UAH |
|---|---|
| Production cost (PC) | 6585.91 |
| Profit (30 % of PC) | 1975.77 |
| Contract price excl. VAT (CP) | 8561.68 |
| VAT (20 % of CP) | 1681.71 |
| Contract price incl. VAT | 10090.27 |

### 4.2 Efficiency Assessment

Labor efficiency is assessed using the following formulas:

```
E = AEE / CI                                       (4.6)
PBP = CI / AEE                                     (4.7)
```

where E — efficiency coefficient; AEE — annual economic effect, UAH; CI — capital investment (excl. VAT), UAH.

Sources of economic benefit:

- labor-cost savings: without the system — 1 h/day × 72.00 UAH/h × 365 = 26,280.00 UAH/year;
- electricity savings: adaptive control reduces consumption by 25–40 %. At a ventilation/heating cost of UAH 1800.00/month, the saving is UAH 585.00/month = UAH 7020.00/year;
- prevention of emergencies (fire-related losses in the residential sector run to tens of thousands of UAH, but this is not factored into the calculation as an unverified line item).

**Table 4.5 — Calculation of the economic effect of system deployment**

| Cost/income item | Before deployment, UAH/yr | After deployment, UAH/yr | Annual effect, UAH |
|---|---|---|---|
| Labor cost of manual monitoring | 26280.00 | 0.00 | +26280.00 |
| Electricity costs | 21600.00 | 14580.00 | +7020.00 |
| System maintenance costs | 2000.00 | 1200.00 | +800.00 |
| Cost of emergency risk (notional) | 15000.00 | 1500.00 | +13500.00 |
| **Total** | **64880.00** | **17280.00** | **+47600.00** |

```
AEE = 26280.00 + 7020.00 = 33300.00 UAH/year
E = AEE / CI = 33300.00 / 8561.68 = 3.89
```

E > 1 — the project is economically justified.

```
PBP = CI / AEE = 8561.68 / 33300.00 = 0.26 year ≈ 3 months
```

**Table 4.6 — Summary techno-economic indicators of the project**

| Indicator | Value |
|---|---|
| Production cost | UAH 6585.91 |
| Contract price excl. VAT | UAH 8561.68 |
| Contract price incl. VAT | UAH 10090.27 |
| Profitability margin | 30 % |
| Annual economic effect | UAH 33,300.00 |
| Payback period | 0.25 year (3 months) |
| Efficiency coefficient | 3.89 |
| Total development effort | 36 person-hours |

*Figure 4.4 — Project payback graph by month.*

The system's production cost is UAH 6585.91, and its market value including VAT is UAH 10,274.02. The payback period of 0.26 year (3 months) is far below the typical range for industrial automation (1–3 years), confirming the project's efficiency. An efficiency coefficient of 3.89 means that every UAH invested returns nearly four UAH of economic benefit within the first year.

## 5. Occupational Safety

### 5.1 Analysis of Hazardous and Harmful Production Factors

In accordance with GOST 12.0.003-2015, the following factors were identified during the system's development and deployment.

**Physical factors:**

- elevated electrical voltage. The system is powered from a 220 V mains supply and contains secondary 5 V and 3.3 V circuits. The relay block switches 220 V loads. There is a risk of electric shock during installation and commissioning;
- elevated surface temperature. A soldering iron heats up to 300–380 °C — a source of thermal burns. The microcontroller and power elements heat up during operation;
- electromagnetic emissions. The ESP32 generates EM emissions in the 2.4 GHz band (Wi-Fi, Bluetooth). Power does not exceed regulatory limits but requires compliance with sanitary norms;
- elevated noise level. Connected ventilation units may exceed the permissible limit of 50–65 dBA.

**Chemical factors:** rosin and lead-containing solder fumes during soldering — toxic with prolonged inhalation.

**Psychophysiological factors:** programming the ESP32 and debugging the Bluetooth connection require prolonged concentration on an oscilloscope screen and the Serial Monitor — accumulating visual and nervous-psychological fatigue, especially during late-night debugging sessions.

**Table 5.1 — Hazardous and harmful production factors during system development**

| No. | Factor | Factor class | Source | Regulatory document |
|---|---|---|---|---|
| 1 | Elevated voltage | Physical | Mains and circuit | GOST 12.1.019 |
| 2 | Elevated temperature | Physical | Soldering iron, heated elements | GOST 12.1.005 |
| 3 | EM emissions, 2.4 GHz | Physical | ESP32 Bluetooth module | NRBU-97 |
| 4 | Elevated noise | Physical | Load ventilation fan | DSN 3.3.6.037 |
| 5 | Rosin fumes | Chemical | Soldering process | MAC 6 mg/m³ |
| 6 | Visual fatigue | Psychophysiological | Prolonged PC work | DSanPiN 3.3.2.007 |
| 7 | Static loading | Psychophysiological | Posture during assembly | GOST 12.0.003 |
| 8 | Combustible gases | Chemical | MQ-2 sensor exposure zone | PB 08-624 |

**Artificial-lighting calculation.** Input data: room S = 24 m² (6 m × 4 m), h = 3.2 m; visual-work category IV (assembly of small parts), E = 400 lx; K = 1.4; Z = 1.1; v = 0.8; mounting height h = 3.2 − 0.8 = 2.4 m.

Room index:

```
i = (A × B) / (h × (A + B)) = (6 × 4) / (2.4 × (6 + 4)) = 24 / 24 = 1.0   (5.1)
```

Luminous-flux utilization factor η = 0.6.

Total required luminous flux:

```
Ftotal = (E × K × S × Z) / (η × v) = (400 × 1.4 × 24 × 1.1) / (0.6 × 0.8) = 30,800 lm   (5.2)
```

A 4×18 LED luminaire was selected (Flum = 1700 lm, n = 4 lamps).

```
Flum,total = 4 × 1700 = 6800 lm                                       (5.3)
```

Number of luminaires: n = 30,800 / 6800 = 4.53 ≈ 5 units.

Verification:

```
Eactual = (5 × 6800 × 0.6 × 0.8) / (1.4 × 24 × 1.1) = 441.7 lx > 400 lx   (5.4)
```

The standard is met.

*Figure 5.1 — Layout of luminaires in the work room.*

### 5.2 Safety Measures

**General requirements** (NPAOP 40.1-1.32-01, GOST 12.1.019-79). Before starting work: check tool condition, the integrity of power-cable insulation, switch on ventilation, and put on an anti-static wristband.

**During soldering:** a soldering iron with a tip temperature up to 350–380 °C; local ventilation or an extraction hood; switch off the iron after use.

**When testing the relay block (220 V):** all connections only with power disconnected; tools with insulated handles.

**Electrical safety during device operation.** The device is built in a dielectric plastic enclosure, which prevents accidental user contact with live parts. By its type of shock protection, the hardware belongs to Protection Class II under DSTU EN 61140. Protective grounding is neither provided nor required for such devices due to the presence of double or reinforced insulation.

To ensure reliable and safe system operation, a set of structural and circuit-design measures has been implemented. **Structural isolation and separation:** double insulation of power circuits is used.

The design is physically separated into isolated low-voltage and high-voltage zones. The air gap and creepage distance between these zones is at least 6 mm, fully meeting the requirements of DSTU EN 60950-1.

**Galvanic isolation:** the ESP32's control outputs are fully isolated from the 220 V relay power circuits using PC817 optocouplers. The built-in isolation dielectric strength of the optocoupler is 5000 V, preventing high voltage from breaking through to the digital part of the device.

**Overvoltage and arc-suppression protection:** to limit switching overvoltages arising when inductive loads are connected or disconnected, RC snubber networks (R = 4.7 kΩ, C = 100 nF/630 V) are integrated in parallel with the relay contacts. They effectively suppress back-EMF voltage spikes and reduce contact wear.

**Emergency overcurrent protection (fuse system):** to protect against short circuits and prolonged current overloads, fuses are integrated into the power section of the circuit. A separate fuse is installed on each power-switching channel, allowing the faulty section to be isolated without disconnecting the entire device. The rating of each fuse is calculated and selected according to the permissible continuous current load and cross-section of the connected cable, as well as the maximum operating current of the switched equipment. This ensures the protection trips before thermal damage occurs to wire or PCB-trace insulation.

**Enclosure protection:** the protection rating of the housing and remote sensors is no lower than IP30 per DSTU EN 60529. This provides reliable protection against the ingress of solid objects and particles larger than 2.5 mm, restricting access of tools or fingers to hazardous live components.

**Table 5.3 — Electric-shock protection measures in the system**

| No. | Protective measure | Technical implementation | Regulatory document |
|---|---|---|---|
| 1 | Galvanic isolation | PC817 optoisolator between ESP32 and relays | GOST 12.1.019 |
| 2 | Protective grounding | Grounding of metal housing elements | PUE §1.7 |
| 3 | Overvoltage protection | RC snubber, 4.7 kΩ + 100 nF/630 V | DSTU EN 60950 |
| 4 | Short-circuit protection | PP17-6A fuses | GOST 17242 |
| 5 | Enclosure protection | IP30 housing for external sensors | DSTU EN 60529 |

**ESD protection.** The ESP32, BME280, and optoisolators are ESD-sensitive (Class 2 per JEDEC JESD22-A114, threshold 2000 V). Required: an anti-static wristband, an anti-static mat, storage of ICs in anti-static packaging.

**Safety during MQ-2 calibration.** Eliminate open flame sources; ventilation with an air-exchange rate of at least 5/h; apply test gas in small doses (up to 1000 ppm); ventilate the room for 15 minutes after testing.

### 5.3 Fire Safety

The development-room category is B (fire-hazardous, NAPB B.07.005-86). The system's installation zone may carry an elevated risk in the event of a combustible-gas leak.

**Main causes of fire:** short circuits due to damaged insulation; overheating of circuit elements; ignition of insulation by a soldering iron; combustible-gas leakage and ignition by an electrical spark during relay switching.

**Preventive measures:** installation per the schematic; RC snubber networks limit arcing during switching; the hysteresis algorithm prevents contact wear; the MQ-2 sensor detects gas concentrations from 200 ppm and automatically activates ventilation.

**Table 5.4 — Fire-prevention measures during system development and operation**

| Fire cause | Protective measure | Regulatory document |
|---|---|---|
| Short circuit | 6 A fuses + protective grounding | NAPB A.01.001 |
| Overheating of elements | Thermal-regime calculation, unit ventilation | GOST 12.1.004 |
| Ignition by soldering iron | Non-flammable mat, switch-off after use | NAPB B.07.005 |
| Combustible-gas leak | MQ-2 sensor + automatic ventilation | DBN B.2.5-20 |
| Insulation ignition | VVG-ng(A) flame-retardant-jacketed cable | DSTU 5076 |

Primary fire-extinguishing means: VVK-1.4 or VVK-2 CO2 extinguishers (do not damage electronic equipment). It is forbidden to extinguish live equipment with water or foam-based agents.

**The system's role in improving fire safety.** The MQ-2 sensor responds to gas concentrations from as low as 200 ppm — well before reaching the LEL (methane — 50,000 ppm, propane — 21,000 ppm). On detecting an alarm threshold: ventilation switches on (response time no more than 100 ms); an alarm message is shown on the display; a notification is sent via Bluetooth; the event is logged to LittleFS with a timestamp. The system meets the requirements of DBN B.2.5-56:2014 and ISO 16000.

*Figure 5.4 — Algorithm for the system's emergency response upon detecting combustible gases.*

## Conclusions

In the course of this qualification work, a complete automated microprocessor-based system for monitoring and controlling the microclimate and safety of the air environment was developed, based on the ESP32 microcontroller.

The general part defines the main functions and application areas of the air-quality control system — from Smart Home installations to industrial premises. A comparative analysis of the system's technical specifications was carried out, in particular the measurement ranges for temperature, humidity, atmospheric pressure, and combustible-gas concentration. A comparative analysis against existing similar solutions was performed, justifying the chosen implementation approach. The system's operating principle was also examined at the structural-diagram level, providing a coherent understanding of its architecture.

The special part examines the system's component base in detail. The choice of the ESP32 microcontroller with a FreeRTOS architecture for parallel task processing on two cores is justified. The functional purpose and technical specifications of the sensor node — the precision BME280 sensor and the MQ-2 gas analyzer — are defined. The circuit-design solutions for the opto-isolated relay block with RC snubber networks are described. The system's key parameters — reliability, power consumption, and individual electrical-schematic elements, including the LED indication node — were calculated.

The third part describes the means of ensuring the system's operability under operating conditions. Hardware monitoring methods are examined, including multimeters, oscilloscopes, and I2C bus-testing tools. Software diagnostic tools for monitoring system status via the Serial Monitor and over Bluetooth are presented. A fault-finding algorithm was developed, covering failures of the sensor node, actuators, and communication interfaces. Service equipment for ongoing sensor adjustment and calibration is also characterized.

The economic section calculates the cost of developing and manufacturing the system, accounting for the cost of electronic components, labor costs, and overhead production expenses. The contract price of the project was determined, and a techno-economic efficiency assessment was carried out, confirming the feasibility of deploying the system due to a 25–40 % reduction in microclimate-control costs. The occupational-safety section describes requirements for safe work with electronic equipment, measures to protect against electric shock, and fire-safety requirements.

In summary, the developed system fully meets the requirements of the technical assignment, ensures continuous monitoring of air-environment parameters in accordance with DBN B.2.5-67:2013 and ISO 16000, implements intelligent actuator control, and provides real-time telemetry transmission via Bluetooth. The assembled and debugged prototype confirmed the operability of all three control channels: ventilation triggers when the temperature exceeds its threshold and when the MQ-2 detects gas levels above 200 ppm, with hysteresis eliminating relay "chatter". The time from detecting an alarm gas concentration to activating ventilation is less than 100 ms. The system is ready for installation in a real-world room.

## References

1. DBN B.2.5-67:2013. *Heating, Ventilation and Air Conditioning.* — Kyiv: Ministry for Regional Development of Ukraine, 2013. — 120 p.
2. *Datasheet MQ-2 Gas Sensor Module.* — Winsen Electronics Technology Co., Ltd., 2023. — Available at: https://www.winsen-sensor.com/d/files/PDF/MQ-2%20ver1.4%20-%20Manual.pdf
3. *BME280 Data Sheet.* — Available at: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
4. *ESP32 Technical Reference Manual.* — Espressif Systems, 2024. — Available at: https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
5. *ELTRAL KS 30/40 Chain Actuator Installation and Operating Instructions.* — ELTRAL GmbH, 2023. — Available at: https://www.eltral.de/en/products/chain-actuators/ks-30-40
6. *Arduino IDE Documentation.* — Arduino.cc, 2025. — Available at: https://docs.arduino.cc/
7. *AirGradient ONE Open-Source Air Quality Monitor Documentation.* — AirGradient, 2025. — Available at: https://www.airgradient.com/documentation/
8. *Atmotube PRO 2 User Manual.* — Atmotube, 2024. — Available at: https://atmotube.com/atmotube-pro-2
9. *Airthings View Plus Product Specifications.* — Airthings AS, 2025. — Available at: https://www.airthings.com/view-plus
10. Kozak I. V., Petrenko O. O. *Microprocessor Control Systems: a textbook.* — Kyiv: Igor Sikorsky Kyiv Polytechnic Institute Publishing House, 2022. — 368 p.
11. *ESP32 Arduino Core Documentation.* — Espressif Systems, 2025. — Available at: https://docs.espressif.com/projects/arduino-esp32/
12. *WHO Guidelines for Indoor Air Quality: Selected Pollutants.* — World Health Organization, 2010 (updated 2021). — Available at: https://www.who.int/publications/i/item/9789289002134
13. *Soldering Station.* — Available at: https://www.antistat.co.uk/wp-content/uploads/2025/10/069-0012-ESD-Workstation-Kit-Blue-01.webp
