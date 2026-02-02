# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-based quadruped robot dog firmware. 4-leg robot with 12 servo motors (3 per leg: alpha, beta, gamma joints), IMU (MPU9250), power sensor (INA219), controlled via WebSocket/serial CLI. **Project is closed and unmaintained** — libraries have drifted and it may not compile cleanly.

## Build & Flash

### PlatformIO (recommended)

`platformio.ini` at the repo root pins all platform and library versions. Install PlatformIO CLI (`pip install platformio`) or use the VS Code extension.

```bash
# Edit WiFi credentials first:
#   software/robot_dog_esp32/config_wifi.h

pio run -t upload        # build and flash
pio device monitor       # serial monitor (115200 baud)
```

### Arduino IDE (alternative)

Main sketch: `software/robot_dog_esp32/robot_dog_esp32.ino`.

- **Board**: ESP32 by Espressif — install version **2.0.17** (NOT 3.x — breaking API changes)
- **Libraries** (install via Library Manager): `ESP32_ISR_Servo` 1.5.0, `ESPAsyncWebServer`, `AsyncTCP`, `MPU9250_WE` 1.2.6, `Adafruit PWM Servo Driver Library`, `INA219_WE`
- WiFi credentials: edit `config_wifi.h` (created from `config_wifi.example.h`)
- Serial baud: 115200

### Important build note

The `libs/` subdirectory `.cpp` files are `#include`d directly into the main sketch (single compilation unit). The PlatformIO config excludes `libs/` from auto-compilation to avoid "multiple definition" errors. `libs/IK/IK.cpp` is the full IK solver (unused) — only `IK_simple.cpp` is active (L1=0 config).

### Web UI Build

```bash
cd software/web
npm install
npm run build    # Gulp pipeline: inline, minify, gzip → outputs index.html.gz.h
```

The build produces `software/robot_dog_esp32/web/index.html.gz.h` (precompiled gzipped HTML served by the ESP32).

## Architecture

### Dual-Core Task Split

- **Core 1 (real-time loop, 250 Hz / 4ms):** failsafe check → gait update → inverse kinematics → servo PWM output
- **Core 0 (service loop, ~10 Hz):** IMU read, WiFi management, power monitoring, CLI processing, subscription updates

Entry point: `robot_dog_esp32.ino` — sets up FreeRTOS tasks on both cores.

### Key Subsystems

| Subsystem | Files | Role |
|-----------|-------|------|
| Inverse Kinematics | `libs/IK/` | Per-leg 3-link IK solver, geometry types |
| Gait Engine | `libs/gait/`, `gait.ino` | Trotting gait sequencing (swing/stance phases) |
| Movement Planner | `libs/planner/` | Predictive body positioning from velocity vectors |
| Balance | `libs/balance/` | Center-of-mass calculation (incomplete) |
| Body State | `libs/HAL_body/` | Aggregates body position/rotation state |
| Transition | `libs/transition/` | Smooth leg movement interpolation |
| Hardware Abstraction | `HAL.ino`, `HAL_ESP32PWM.ino` | IK integration + servo PWM via ISR |
| CLI | `cli.ino`, `cliGet.ino`, `cliSet.ino` | Serial command interface (`get`/`set`/`run`/`sbs`) |
| WebSocket/HTTP | `webServer.ino`, `packagesProcess.ino` | AsyncWebServer, binary protocol over WS |
| Subscriptions | `subscription.ino` | Real-time telemetry streaming (FAST=5ms, SLOW=100ms) |
| Settings | `settings.ino` | EEPROM persistence for trims and WiFi config |
| Failsafe | `failsafe.ino` | Stops robot if no WS message for ~100ms |

### Config Files

- `config.h` — loop timing, PWM controller selection (ESP32PWM vs PCA9685), sensor addresses, servo limits
- `config_small.h` — robot-specific: leg dimensions (L2=51.9mm, L3=50mm), servo GPIO pin assignments, servo PWM calibration table, gear ratios
- `def.h` — struct/type definitions

### Servo Pin Mapping (GPIO)

| Leg | Alpha | Beta | Gamma |
|-----|-------|------|-------|
| LF (Left Front) | 25 | 26 | 27 |
| RF (Right Front) | 16 | 18 | 17 |
| LH (Left Hind) | 13 | 12 | 14 |
| RH (Right Hind) | 4 | 2 | 15 |

### Communication Protocols

**WebSocket** (`ws://<ip>/ws`): Binary frames — `P_MOVE` (0x4D, 14 bytes: velocity + rotation vectors as int16 /10000) and `P_TELEMETRY` (0x54, 14 bytes: voltage, current, loop time).

**Serial CLI**: `get angles`, `set LF_HAL_trim_alpha 2.5`, `run i2cscan`, `sbs imu 1 0` (subscribe to IMU at slow rate).

### EEPROM Layout

Addresses 0-1: version/revision. Addresses 2-13: per-leg trim values (alpha/beta/gamma for LF, RF, LH, RH). Address 14: WiFi mode (0=AP, 1-255=network index).

## Calibration

Servo calibration requires physical measurement — see `tools/` directory and README.md for the procedure. Trim values are adjusted via CLI (`set <leg>_HAL_trim_<joint> <degrees>`) and persisted to EEPROM.
