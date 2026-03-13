/*
 * I2C Scanner - Upload this to test your wiring
 *
 * Scans SDA=GPIO21, SCL=GPIO22 and prints all found devices.
 * Expected results:
 *   0x40 = PCA9685 servo driver
 *   0x68 = MPU9250 IMU
 *   0x40 = INA219 (conflicts with PCA9685 if both at default)
 */

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);  // wait for serial monitor to connect

  Serial.println();
  Serial.println("=== I2C Scanner ===");
  Serial.println("SDA = GPIO 21");
  Serial.println("SCL = GPIO 22");
  Serial.println();

  Wire.begin(21, 22);
  Wire.setClock(400000);
}

void loop() {
  Serial.println("Scanning...");

  int found = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      found++;
      Serial.print("  FOUND device at 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      Serial.print(" (");
      Serial.print(addr, DEC);
      Serial.print(")");

      // identify known devices
      if (addr == 0x40) Serial.print(" ← PCA9685 or INA219");
      if (addr == 0x41) Serial.print(" ← INA219 (A0 bridged)");
      if (addr == 0x68) Serial.print(" ← MPU9250 IMU");
      if (addr == 0x69) Serial.print(" ← MPU9250 (AD0 high)");

      Serial.println();
    }
  }

  if (found == 0) {
    Serial.println("  NO devices found! Check wiring:");
    Serial.println("    - SDA wire to GPIO 21?");
    Serial.println("    - SCL wire to GPIO 22?");
    Serial.println("    - VCC to PCA9685?");
    Serial.println("    - GND connected?");
  } else {
    Serial.print("  Total: ");
    Serial.print(found);
    Serial.println(" device(s)");
  }

  Serial.println();
  delay(3000);  // scan every 3 seconds
}
