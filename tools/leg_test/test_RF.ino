// =============================================================
// RIGHT FRONT Leg Test — PCA9685 servo driver, ESP32
// Channels: Alpha=6, Beta=7, Gamma=8
// =============================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

// PCA9685 pulse limits at 50 Hz
#define PULSE_MIN 150
#define PULSE_MAX 600

// Channel assignments
#define CH_ALPHA 6
#define CH_BETA  7
#define CH_GAMMA 8

// Calibrated angle limits
#define ALPHA_MIN   10
#define ALPHA_MAX  135   // 135=Outward, 10=Inward
#define ALPHA_MID   50

#define BETA_MIN     0
#define BETA_MAX   135   // 135=Front, 0=Back (inverted from left)
#define BETA_MID    67

#define GAMMA_MIN    0
#define GAMMA_MAX  135   // 0=Front, 135=Back
#define GAMMA_MID   67

// Current positions
float curAlpha = ALPHA_MID;
float curBeta  = BETA_MID;
float curGamma = GAMMA_MID;

uint16_t angleToPulse(float angle) {
  return map((long)(angle * 10), 0, 1800, PULSE_MIN, PULSE_MAX);
}

void moveServo(uint8_t channel, float angle, const char* name,
               float minA, float maxA, const char* minLabel, const char* maxLabel) {
  if (angle < minA) angle = minA;
  if (angle > maxA) angle = maxA;
  uint16_t pulse = angleToPulse(angle);
  pca.setPWM(channel, 0, pulse);
  Serial.printf("%s -> %.1f deg  (pulse %u)  [%.0f=%s .. %.0f=%s]\n",
                name, angle, pulse, minA, minLabel, maxA, maxLabel);
}

void moveAlpha(float angle) {
  curAlpha = constrain(angle, (float)ALPHA_MIN, (float)ALPHA_MAX);
  moveServo(CH_ALPHA, curAlpha, "Alpha", ALPHA_MIN, ALPHA_MAX, "Inward", "Outward");
}

void moveBeta(float angle) {
  curBeta = constrain(angle, (float)BETA_MIN, (float)BETA_MAX);
  moveServo(CH_BETA, curBeta, "Beta", BETA_MIN, BETA_MAX, "Back", "Front");
}

void moveGamma(float angle) {
  curGamma = constrain(angle, (float)GAMMA_MIN, (float)GAMMA_MAX);
  moveServo(CH_GAMMA, curGamma, "Gamma", GAMMA_MIN, GAMMA_MAX, "Front", "Back");
}

void printInfo() {
  Serial.println("--- RIGHT FRONT current state ---");
  Serial.printf("  Alpha: %.1f deg  pulse %u  [%d=%s .. %d=%s]\n",
                curAlpha, angleToPulse(curAlpha), ALPHA_MIN, "Inward", ALPHA_MAX, "Outward");
  Serial.printf("  Beta:  %.1f deg  pulse %u  [%d=%s .. %d=%s]\n",
                curBeta, angleToPulse(curBeta), BETA_MIN, "Back", BETA_MAX, "Front");
  Serial.printf("  Gamma: %.1f deg  pulse %u  [%d=%s .. %d=%s]\n",
                curGamma, angleToPulse(curGamma), GAMMA_MIN, "Front", GAMMA_MAX, "Back");
}

void sweepJoint(const char* name, uint8_t channel,
                float minA, float maxA, float* cur,
                const char* minLabel, const char* maxLabel) {
  Serial.printf("Sweeping %s: %.0f -> %.0f -> %.0f\n", name, minA, maxA, minA);
  for (float a = minA; a <= maxA; a += 1.0) {
    *cur = a;
    pca.setPWM(channel, 0, angleToPulse(a));
    delay(20);
  }
  for (float a = maxA; a >= minA; a -= 1.0) {
    *cur = a;
    pca.setPWM(channel, 0, angleToPulse(a));
    delay(20);
  }
  float mid = (minA + maxA) / 2.0;
  *cur = mid;
  pca.setPWM(channel, 0, angleToPulse(mid));
  Serial.printf("  %s returned to mid %.1f\n", name, mid);
}

void doSweep() {
  Serial.println("=== RIGHT FRONT sweep ===");
  sweepJoint("Alpha", CH_ALPHA, ALPHA_MIN, ALPHA_MAX, &curAlpha, "Inward", "Outward");
  delay(300);
  sweepJoint("Beta",  CH_BETA,  BETA_MIN,  BETA_MAX,  &curBeta,  "Back",  "Front");
  delay(300);
  sweepJoint("Gamma", CH_GAMMA, GAMMA_MIN, GAMMA_MAX, &curGamma, "Front", "Back");
  Serial.println("=== sweep complete ===");
}

void goMid() {
  Serial.println("Moving to mid positions...");
  moveAlpha(ALPHA_MID);
  moveBeta(BETA_MID);
  moveGamma(GAMMA_MID);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== RIGHT FRONT Leg Test ===");
  Serial.println("Channels: Alpha=6, Beta=7, Gamma=8");
  Serial.println("Commands: a <angle>, b <angle>, g <angle>, mid, sweep, info");

  Wire.begin();
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(50);
  delay(10);

  goMid();
  printInfo();
}

void loop() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("a ")) {
    moveAlpha(line.substring(2).toFloat());
  } else if (line.startsWith("b ")) {
    moveBeta(line.substring(2).toFloat());
  } else if (line.startsWith("g ")) {
    moveGamma(line.substring(2).toFloat());
  } else if (line == "mid") {
    goMid();
  } else if (line == "sweep") {
    doSweep();
  } else if (line == "info") {
    printInfo();
  } else {
    Serial.println("Unknown command. Use: a/b/g <angle>, mid, sweep, info");
  }
}
