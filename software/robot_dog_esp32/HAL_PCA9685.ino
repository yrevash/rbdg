#if PWM_CONTROLLER_TYPE == PCA9685

void initServoHAL() {
  Serial.println("PCA9685");
  pwm = Adafruit_PWMServoDriver();
  pwm.begin();

  // Check if PCA9685 is responding on I2C
  Wire.beginTransmission(PWM_CONTROLLER_ADDRESS);
  uint8_t error = Wire.endTransmission();
  if (error == 0) {
    Serial.print("  PCA9685 found at 0x");
    Serial.println(PWM_CONTROLLER_ADDRESS, HEX);
  } else {
    Serial.print("  ERROR: PCA9685 NOT found at 0x");
    Serial.print(PWM_CONTROLLER_ADDRESS, HEX);
    Serial.print(" (error=");
    Serial.print(error);
    Serial.println(")");
  }

  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz
  pwm.setPWMFreq(SERVO_FREQ);  // This is the maximum PWM frequency of servo
  Serial.print("  PWM freq: ");
  Serial.print(SERVO_FREQ);
  Serial.println(" Hz");
}

uint16_t angleToPulse(double angleRad) {
  // Convert radians to degrees, then map to PCA9685 pulse at 50Hz
  // Tested values: 150 = 0°, 600 = 180°
  double angleDeg = angleRad * (180.0 / M_PI);
  if (angleDeg < 0) angleDeg = 0;
  if (angleDeg > 180) angleDeg = 180;
  return (uint16_t)mapf(angleDeg, 0, 180, 150, 600);
}

void setLegPWM(leg &_leg)
{
  pwm.setPWM(_leg.hal.pin.alpha,  0,  angleToPulse(limitServoAngle(getHALAngle(_leg.angle.alpha, _leg.hal.mid.alpha, _leg.hal.trim.alpha, _leg.hal.ratio.alpha, _leg.inverse.alpha))));
  pwm.setPWM(_leg.hal.pin.beta,   0,  angleToPulse(limitServoAngle(getHALAngle(_leg.angle.beta,  _leg.hal.mid.beta,  _leg.hal.trim.beta,  _leg.hal.ratio.beta,  _leg.inverse.beta ))));
  pwm.setPWM(_leg.hal.pin.gamma,  0,  angleToPulse(limitServoAngle(getHALAngle(_leg.angle.gamma, _leg.hal.mid.gamma, _leg.hal.trim.gamma, _leg.hal.ratio.gamma, _leg.inverse.gamma))));
}

void runServoCalibrate(leg &_leg)
{
  double midAngle = M_PI_2;  // 90 degrees
  pwm.setPWM(_leg.hal.pin.alpha, 0, angleToPulse(midAngle));
  pwm.setPWM(_leg.hal.pin.beta,  0, angleToPulse(midAngle));
  pwm.setPWM(_leg.hal.pin.gamma, 0, angleToPulse(midAngle));
}


#endif
