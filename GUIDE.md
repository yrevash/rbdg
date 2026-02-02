# ESP32 Robot Dog - Complete Beginner Guide

A step-by-step guide to build, flash, and control this quadruped robot dog from scratch.

---

## Part 1: What You Need

### Hardware

| Part | Quantity | Notes |
|------|----------|-------|
| ESP32 DevKit board | 1 | Any ESP32 dev board (e.g. "DOIT ESP32 DEVKIT V1") |
| TowerPro MG90D servo | 12 | Or similar metal-gear micro servos |
| 18650 Li-ion battery | 2 | Fully charged (3.7V each, 7.4V total) |
| Battery holder | 1 | 2-cell 18650 holder with wires |
| Jumper wires | ~40 | Male-to-male for breadboard connections |
| Breadboard | 1 | Full-size recommended |
| USB cable | 1 | Micro-USB or USB-C (depends on your ESP32 board) |
| 3D-printed frame | 1 set | See original project for STL files |

Optional but recommended:
- INA219 power sensor module (monitors battery voltage/current)
- MPU9250 IMU module (accelerometer/gyroscope - read but not actively used yet)

### Software (on your computer)

- **Python 3** - https://www.python.org/downloads/
- **PlatformIO CLI** - installed via Python (instructions below)
- **A text editor** - VS Code, Notepad++, or anything you like

---

## Part 2: Wiring

### Servo Connections

Each leg has 3 servos (alpha, beta, gamma joints). Connect each servo's signal wire to the corresponding ESP32 GPIO pin:

```
LEFT FRONT (LF)           RIGHT FRONT (RF)
  Alpha → GPIO 25           Alpha → GPIO 16
  Beta  → GPIO 26           Beta  → GPIO 18
  Gamma → GPIO 27           Gamma → GPIO 17

LEFT HIND (LH)            RIGHT HIND (RH)
  Alpha → GPIO 13           Alpha → GPIO 4
  Beta  → GPIO 12           Beta  → GPIO 2
  Gamma → GPIO 14           Gamma → GPIO 15
```

### Servo Power

Servos need more current than the ESP32 can provide through its pins. Connect servo power (red/brown wires) to the battery, NOT to the ESP32's 3.3V/5V pins.

- All servo **red wires** → battery positive (+7.4V or regulated 5V-6V)
- All servo **brown/black wires** → battery negative (GND)
- **Battery GND must also connect to ESP32 GND** (common ground)

### Optional Sensors (I2C)

If you have the INA219 or MPU9250, connect them to the ESP32's I2C bus:
- SDA → GPIO 21
- SCL → GPIO 22
- VCC → 3.3V
- GND → GND

---

## Part 3: Install Software

### Step 1: Install Python

Download and install Python 3 from https://www.python.org/downloads/

During installation on Windows, **check "Add Python to PATH"**.

Verify it works by opening a terminal (Command Prompt, PowerShell, or Terminal on Mac/Linux):
```bash
python --version
```

### Step 2: Install PlatformIO

Open a terminal and run:
```bash
pip install platformio
```

Verify it works:
```bash
pio --version
```

### Step 3: Download This Project

If you have `git`:
```bash
git clone https://github.com/SovGVD/esp32-robot-dog-code.git
cd esp32-robot-dog-code
```

Or download the ZIP from GitHub and extract it.

---

## Part 4: Configure WiFi

Open the file `software/robot_dog_esp32/config_wifi.h` in a text editor.

You'll see:
```c
const char* wifiSsid[] = {"RobotDog", "your_wifi_ssid"};
const char* wifiPass[] = {"robotdog123", "your_wifi_password"};
```

- **First entry** (`"RobotDog"` / `"robotdog123"`) = the WiFi network the robot creates in AP (Access Point) mode. Change the name and password to whatever you want.
- **Second entry** = your home WiFi network. Replace `"your_wifi_ssid"` and `"your_wifi_password"` with your actual WiFi name and password.

The robot starts in AP mode by default (it creates its own WiFi network).

---

## Part 5: Build and Flash

### Step 1: Connect the ESP32

Plug the ESP32 into your computer with a USB cable. No servos or batteries need to be connected yet for flashing.

### Step 2: Build and Upload

Open a terminal, navigate to the project folder, and run:
```bash
cd esp32-robot-dog-code
pio run -t upload
```

The first time you run this, PlatformIO will automatically:
1. Download the ESP32 toolchain
2. Download all required libraries (with correct versions)
3. Compile the code
4. Upload it to the ESP32

If it can't find the ESP32, you may need to install a USB driver:
- **CP2102 chip** (most common): https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
- **CH340 chip**: https://www.wch-ic.com/downloads/CH341SER_EXE.html

Check which chip your board uses (printed on the board near the USB connector).

### Step 3: Open Serial Monitor

After uploading, open the serial monitor to see output:
```bash
pio device monitor
```

You should see something like:
```
Settings
0 leg trim {0.0, 0.0, 0.0}
1 leg trim {0.0, 0.0, 0.0}
2 leg trim {0.0, 0.0, 0.0}
3 leg trim {0.0, 0.0, 0.0}
ESP32_ISR v1.5.0
IMU
WiFi_AP
WiFi AP IP: 192.168.4.1
WebServer

 CLI: type `get|set|run|sbs help` to see available methods.
```

If you see this, the firmware is running. Press `Ctrl+C` to exit the monitor.

---

## Part 6: Connect to the Robot

### Option A: Access Point Mode (default)

1. On your phone or laptop, look for a WiFi network named **"RobotDog"** (or whatever you set in `config_wifi.h`)
2. Connect using the password you set (default: `robotdog123`)
3. Open a browser and go to **http://192.168.4.1**
4. The web control interface loads

### Option B: Connect to Your Home WiFi

To switch the robot from AP mode to your home WiFi:
1. Open the serial monitor (`pio device monitor`)
2. Type and press Enter:
   ```
   set wifi 1
   ```
   (the `1` means "use the second entry in config_wifi.h", which is your home WiFi)
3. The robot restarts WiFi and prints its new IP address
4. Open that IP in your browser

To switch back to AP mode:
```
set wifi 0
```

---

## Part 7: Using the Web Interface

Once you open the robot's IP in a browser, you get a joystick-style control interface.

- **Left joystick**: Move the robot (forward/backward/left/right)
- **Right joystick**: Rotate the robot and adjust pitch/roll

The robot uses WebSocket to communicate in real-time. If the WebSocket connection drops for more than ~80ms, the failsafe kicks in and stops all movement (safety feature).

---

## Part 8: Using the Serial CLI

The serial terminal is your direct control and debugging tool. Connect with:
```bash
pio device monitor
```

### See Available Commands

```
get help      # list all GET commands
set help      # list all SET commands
run help      # list all RUN commands
```

### Useful Commands

| Command | What it does |
|---------|-------------|
| `get angles` | Show all servo angles for all 4 legs |
| `get power` | Show battery voltage and current (needs INA219) |
| `get imu` | Show accelerometer pitch/roll/yaw (needs MPU9250) |
| `get debug 1` | Show loop timing (main loop, service loop) |
| `get debug 2` | Show body position and leg foot positions |
| `run wifiinfo` | Show current WiFi IP, SSID, and password |
| `run i2cscan` | Scan for I2C devices (check sensor connections) |
| `run calibrateimu` | Calibrate IMU (place robot on flat surface first) |
| `set servo_calib` | Set all servos to 90 degrees (for assembly) |
| `set HAL 0` | Disable servo control (servos go limp) |
| `set HAL 1` | Enable servo control |

---

## Part 9: Assemble and Calibrate the Legs

This is the most important step for the robot to walk properly. Servos are never perfectly centered, so you need to calibrate ("trim") each one.

### Step 1: Set Servos to Calibration Position

With the ESP32 connected to your computer and servos powered:
```
set servo_calib
```
All 12 servos move to their 90-degree (middle) position.

### Step 2: Attach the Legs

Attach each leg part to the servo horn so that the joint is at the correct angle. Use the 3D-printed calibration tools from the `tools/` directory to check alignment.

The target angles during calibration are:
- Alpha (hip, side-to-side): 90 degrees
- Beta (upper leg): 45 degrees
- Gamma (lower leg): 90 degrees

### Step 3: Trim Each Servo

After assembly, if a joint isn't perfectly aligned, trim it. Trimming adds a small offset to correct misalignment.

Example - if the left front alpha joint is off by about 3 degrees:
```
set LF_HAL_trim_alpha -3
```

The naming pattern is:
```
set [LEG]_HAL_trim_[JOINT] [degrees]
```

Where:
- `[LEG]` = `LF` (left front), `RF` (right front), `LH` (left hind), `RH` (right hind)
- `[JOINT]` = `alpha`, `beta`, or `gamma`
- `[degrees]` = a number like `-3` or `2.5` (range: about -14 to +14 degrees)

To check current trim values:
```
get LF_HAL_trim_alpha
```

Trim values are saved to EEPROM automatically and survive power cycles.

---

## Part 10: Troubleshooting

### "No module named platformio"
Python isn't finding PlatformIO. Try:
```bash
python -m pip install platformio
```

### Upload fails / ESP32 not found
- Try a different USB cable (some are charge-only, no data)
- Install the correct USB driver (CP2102 or CH340 - see Part 5)
- On Linux, you may need to add your user to the `dialout` group:
  ```bash
  sudo usermod -a -G dialout $USER
  ```
  Then log out and back in.

### "WARNING! Increase LOOP_TIME" on serial
The main loop is running slower than expected. This is usually OK during startup. If it happens constantly during walking, there may be too much CPU load.

### Servos jitter or don't move
- Check power: servos need 5V-7.4V directly from batteries, not from the ESP32's pins
- Check GND: the battery ground MUST be connected to the ESP32's GND
- Check that you're using the correct GPIO pins (see wiring table above)

### WiFi doesn't appear
- Check the serial output for errors
- Make sure `config_wifi.h` exists and has valid values
- The ESP32 takes a few seconds to start the WiFi after boot

### Web page doesn't load
- Make sure you're connected to the robot's WiFi (or same network)
- Try `http://` not `https://`
- The correct IP is shown on serial output at startup

### Robot doesn't walk straight
This means the legs need calibration. See Part 9 for the trim procedure. Good calibration is the difference between a robot that walks and one that falls over.

---

## Quick Reference Card

```
BUILD:           pio run -t upload
MONITOR:         pio device monitor
ALL ANGLES:      get angles
POWER:           get power
WIFI INFO:       run wifiinfo
CALIBRATE MODE:  set servo_calib
TRIM SERVO:      set LF_HAL_trim_alpha -3
DISABLE SERVOS:  set HAL 0
ENABLE SERVOS:   set HAL 1
```
