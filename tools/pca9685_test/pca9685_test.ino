/*
 * PCA9685 Servo Test & Calibration Tool
 *
 * Commands (type in Serial Monitor, set to "Both NL & CR"):
 *   <number>     - Set angle in degrees (0-180)
 *   p<number>    - Set raw pulse value (100-700)
 *   c<number>    - Change channel (0-15)
 *   sweep        - Sweep 0 to 180 and back
 *   stop         - Stop at current position
 *   mid          - Go to 90 degrees
 *   min          - Find minimum angle (go to 0)
 *   max          - Find maximum angle (go to 180)
 *   info         - Show current state
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ  50

// Pulse lengths for 50Hz (adjust these if your servo differs)
#define PULSE_MIN  150   // ~0 degrees
#define PULSE_MAX  600   // ~180 degrees

uint8_t currentChannel = 2;
uint16_t currentPulse = 0;
float currentAngle = 0;
bool sweeping = false;

uint16_t angleToPulse(float angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  return map(angle * 10, 0, 1800, PULSE_MIN, PULSE_MAX);
}

float pulseToAngle(uint16_t pulse) {
  return (float)(pulse - PULSE_MIN) / (PULSE_MAX - PULSE_MIN) * 180.0;
}

void setAngle(float angle) {
  currentAngle = angle;
  currentPulse = angleToPulse(angle);
  pwm.setPWM(currentChannel, 0, currentPulse);
  printState();
}

void setPulse(uint16_t pulse) {
  currentPulse = pulse;
  currentAngle = pulseToAngle(pulse);
  pwm.setPWM(currentChannel, 0, currentPulse);
  printState();
}

void printState() {
  Serial.print("  CH:");
  Serial.print(currentChannel);
  Serial.print("  Angle:");
  Serial.print(currentAngle, 1);
  Serial.print("°  Pulse:");
  Serial.println(currentPulse);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== PCA9685 Servo Calibration Tool ===");

  Wire.begin();
  Wire.setClock(400000);

  Wire.beginTransmission(0x40);
  uint8_t error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("PCA9685 found at 0x40");
  } else {
    Serial.println("ERROR: PCA9685 NOT found!");
    while(1);
  }

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(100);

  Serial.println();
  Serial.println("Commands:");
  Serial.println("  <number>   = set angle (0-180)");
  Serial.println("  p<number>  = set raw pulse (100-700)");
  Serial.println("  c<number>  = change channel (0-15)");
  Serial.println("  sweep      = sweep 0-180-0");
  Serial.println("  stop       = stop sweeping");
  Serial.println("  mid        = go to 90°");
  Serial.println("  min        = go to 0°");
  Serial.println("  max        = go to 180°");
  Serial.println("  info       = show current state");
  Serial.println();

  Serial.print("Starting on channel ");
  Serial.println(currentChannel);
  setAngle(90);
}

void loop() {
  if (sweeping) {
    for (float a = 0; a <= 180; a += 1) {
      if (Serial.available()) { sweeping = false; break; }
      setAngle(a);
      delay(20);
    }
    delay(300);
    for (float a = 180; a >= 0; a -= 1) {
      if (Serial.available()) { sweeping = false; break; }
      setAngle(a);
      delay(20);
    }
    delay(300);
    return;
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) return;

    if (input == "sweep") {
      Serial.println("Sweeping... type anything to stop");
      sweeping = true;
    }
    else if (input == "stop") {
      sweeping = false;
      Serial.println("Stopped.");
    }
    else if (input == "mid") {
      setAngle(90);
    }
    else if (input == "min") {
      setAngle(0);
    }
    else if (input == "max") {
      setAngle(180);
    }
    else if (input == "info") {
      printState();
    }
    else if (input.startsWith("c")) {
      int ch = input.substring(1).toInt();
      if (ch >= 0 && ch <= 15) {
        currentChannel = ch;
        Serial.print("Switched to channel ");
        Serial.println(currentChannel);
        setAngle(currentAngle);
      } else {
        Serial.println("Invalid channel (0-15)");
      }
    }
    else if (input.startsWith("p")) {
      int p = input.substring(1).toInt();
      if (p >= 100 && p <= 700) {
        setPulse(p);
      } else {
        Serial.println("Invalid pulse (100-700)");
      }
    }
    else {
      float angle = input.toFloat();
      if (angle >= 0 && angle <= 180) {
        setAngle(angle);
      } else {
        Serial.println("Invalid angle (0-180)");
      }
    }
  }
}
