# Bill of Materials — Electrical Schematic

Translation of "Перелік елементів схеми електричної принципової" (component list for the principal electrical schematic, see `hardware/diagrams.drawio`, sheet 3 / `03-electrical-schematic.png`). Reference designators match the schematic.

## Capacitors

| Ref. | Description | Qty | Notes |
|---|---|---|---|
| C1, C2 | Electrolytic capacitor, 100 µF, 16 V | 2 | |
| C3–C5 | Film capacitor K73-17, 100 nF, 275 V | 3 | Snubbers |

## Modules

| Ref. | Description | Qty | Notes |
|---|---|---|---|
| DA1 | BME280 sensor | 1 | |
| DA2 | MQ-2 gas analyzer, 200–10,000 ppm, 5 V | 1 | |
| DD1 | ESP32 microcontroller, Xtensa LX6, 240 MHz | 1 | |
| HA1 | Piezo buzzer, 5 V | 1 | |
| HG1 | 16×2 LCD display with PCF8574 I2C adapter | 1 | |
| K1–K3 | SRD-05VDC-SL-C electromagnetic relay | 3 | |

## Resistors

| Ref. | Description | Qty | Notes |
|---|---|---|---|
| R1–R3 | MLT-0.125, 220 Ω, ±5% | 3 | Current limiting |
| R4–R6 | MLT-0.125, 2.2 kΩ, ±5% | 3 | Transistor base |
| R7, R8 | MLT-0.125, 4.7 kΩ, ±5% | 2 | I2C pull-up |
| R9, R11, R12 | MLT-0.5, 4.7 kΩ, ±5% | 3 | Snubbers |
| R10, R13–R16 | MLT-0.125, 10 kΩ, ±5% | 4 | |

## Semiconductors and connectors

| Ref. | Description | Qty | Notes |
|---|---|---|---|
| VD1–VD3 | 1N4007 diode, 1 A, 1000 V | 3 | Relay protection |
| VT1–VT3 | BC547 transistor, NPN, 100 mA, 45 V | 3 | |
| VU1–VU3 | PC817 optocoupler, Uiso = 5000 V | 3 | |
| XS1 | USB-B power connector | 1 | |
| XT1–XT3 | 3-pin screw terminal block | 3 | |
