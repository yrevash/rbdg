// =============================================================
// ALL LEGS Combined Test — PCA9685 servo driver, ESP32
// LF: 0,1,2  LB: 3,4,5  RF: 6,7,8  RB: 9,10,11
// =============================================================

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40);

#define PULSE_MIN 150
#define PULSE_MAX 600

// ---- Leg definition ----
struct LegConfig {
  const char* name;
  uint8_t chAlpha, chBeta, chGamma;
  float alphaMin, alphaMax, alphaMid;
  float betaMin,  betaMax,  betaMid;
  float gammaMin, gammaMax, gammaMid;
  const char* alphaMinLabel; const char* alphaMaxLabel;
  const char* betaMinLabel;  const char* betaMaxLabel;
  const char* gammaMinLabel; const char* gammaMaxLabel;
};

struct LegState {
  float alpha, beta, gamma;
};

// Leg configs
const LegConfig legs[4] = {
  // LEFT FRONT
  { "LF", 0, 1, 2,
     0,  90, 65,   0, 115, 57,   0, 135, 67,
    "Outward","Inward", "Front","Back", "Back","Front" },
  // LEFT BACK
  { "LB", 3, 4, 5,
     0, 130, 65,   0, 135, 67,   0, 135, 67,
    "Inward","Outward", "Front","Back", "Back","Front" },
  // RIGHT FRONT
  { "RF", 6, 7, 8,
    10, 135, 50,   0, 135, 67,   0, 135, 67,
    "Inward","Outward", "Back","Front", "Front","Back" },
  // RIGHT BACK
  { "RB", 9, 10, 11,
     0,  90, 50,   0, 135, 67,   0, 135, 67,
    "Outward","Inward", "Back","Front", "Front","Back" }
};

LegState state[4];
int activeLeg = 0;  // 0=LF, 1=LB, 2=RF, 3=RB

uint16_t angleToPulse(float angle) {
  return map((long)(angle * 10), 0, 1800, PULSE_MIN, PULSE_MAX);
}

void moveServo(uint8_t channel, float angle, const char* jointName,
               const char* legName, float minA, float maxA,
               const char* minLabel, const char* maxLabel) {
  if (angle < minA) angle = minA;
  if (angle > maxA) angle = maxA;
  uint16_t pulse = angleToPulse(angle);
  pca.setPWM(channel, 0, pulse);
  Serial.printf("[%s] %s -> %.1f deg  (pulse %u)  [%.0f=%s .. %.0f=%s]\n",
                legName, jointName, angle, pulse, minA, minLabel, maxA, maxLabel);
}

void moveAlpha(int leg, float angle) {
  const LegConfig& c = legs[leg];
  state[leg].alpha = constrain(angle, c.alphaMin, c.alphaMax);
  moveServo(c.chAlpha, state[leg].alpha, "Alpha", c.name,
            c.alphaMin, c.alphaMax, c.alphaMinLabel, c.alphaMaxLabel);
}

void moveBeta(int leg, float angle) {
  const LegConfig& c = legs[leg];
  state[leg].beta = constrain(angle, c.betaMin, c.betaMax);
  moveServo(c.chBeta, state[leg].beta, "Beta", c.name,
            c.betaMin, c.betaMax, c.betaMinLabel, c.betaMaxLabel);
}

void moveGamma(int leg, float angle) {
  const LegConfig& c = legs[leg];
  state[leg].gamma = constrain(angle, c.gammaMin, c.gammaMax);
  moveServo(c.chGamma, state[leg].gamma, "Gamma", c.name,
            c.gammaMin, c.gammaMax, c.gammaMinLabel, c.gammaMaxLabel);
}

void printLegInfo(int leg) {
  const LegConfig& c = legs[leg];
  const LegState& s = state[leg];
  Serial.printf("--- %s ---\n", c.name);
  Serial.printf("  Alpha: %.1f deg  pulse %u  [%.0f=%s .. %.0f=%s]\n",
                s.alpha, angleToPulse(s.alpha), c.alphaMin, c.alphaMinLabel, c.alphaMax, c.alphaMaxLabel);
  Serial.printf("  Beta:  %.1f deg  pulse %u  [%.0f=%s .. %.0f=%s]\n",
                s.beta, angleToPulse(s.beta), c.betaMin, c.betaMinLabel, c.betaMax, c.betaMaxLabel);
  Serial.printf("  Gamma: %.1f deg  pulse %u  [%.0f=%s .. %.0f=%s]\n",
                s.gamma, angleToPulse(s.gamma), c.gammaMin, c.gammaMinLabel, c.gammaMax, c.gammaMaxLabel);
}

void printInfo() {
  Serial.printf("=== All Legs Info (active: %s) ===\n", legs[activeLeg].name);
  for (int i = 0; i < 4; i++) printLegInfo(i);
}

void sweepJoint(int leg, const char* jointName, uint8_t channel,
                float minA, float maxA, float* cur,
                const char* minLabel, const char* maxLabel) {
  Serial.printf("[%s] Sweeping %s: %.0f -> %.0f -> %.0f\n",
                legs[leg].name, jointName, minA, maxA, minA);
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
  Serial.printf("  %s returned to mid %.1f\n", jointName, mid);
}

void doSweep(int leg) {
  const LegConfig& c = legs[leg];
  LegState& s = state[leg];
  Serial.printf("=== %s sweep ===\n", c.name);
  sweepJoint(leg, "Alpha", c.chAlpha, c.alphaMin, c.alphaMax, &s.alpha,
             c.alphaMinLabel, c.alphaMaxLabel);
  delay(300);
  sweepJoint(leg, "Beta", c.chBeta, c.betaMin, c.betaMax, &s.beta,
             c.betaMinLabel, c.betaMaxLabel);
  delay(300);
  sweepJoint(leg, "Gamma", c.chGamma, c.gammaMin, c.gammaMax, &s.gamma,
             c.gammaMinLabel, c.gammaMaxLabel);
  Serial.println("=== sweep complete ===");
}

void goMid(int leg) {
  const LegConfig& c = legs[leg];
  Serial.printf("[%s] Moving to mid positions...\n", c.name);
  moveAlpha(leg, c.alphaMid);
  moveBeta(leg, c.betaMid);
  moveGamma(leg, c.gammaMid);
}

void goMidAll() {
  Serial.println("Moving ALL legs to mid positions...");
  for (int i = 0; i < 4; i++) goMid(i);
}

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  lf / lb / rf / rb  — select active leg");
  Serial.println("  a <angle>          — move active leg alpha");
  Serial.println("  b <angle>          — move active leg beta");
  Serial.println("  g <angle>          — move active leg gamma");
  Serial.println("  mid                — active leg to mid positions");
  Serial.println("  midall             — ALL legs to mid positions");
  Serial.println("  sweep              — sweep active leg joints");
  Serial.println("  sweepall           — sweep ALL legs sequentially");
  Serial.println("  info               — print all leg states");
  Serial.println("  help               — show this help");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== ALL LEGS Combined Test ===");
  Serial.println("LF:0,1,2  LB:3,4,5  RF:6,7,8  RB:9,10,11");

  Wire.begin();
  pca.begin();
  pca.setOscillatorFrequency(27000000);
  pca.setPWMFreq(50);
  delay(10);

  goMidAll();
  Serial.println();
  printInfo();
  Serial.println();
  Serial.printf("Active leg: %s\n", legs[activeLeg].name);
  printHelp();
}

void loop() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  // Leg selection
  if (line == "lf") {
    activeLeg = 0;
    Serial.printf("Active leg: %s\n", legs[activeLeg].name);
  } else if (line == "lb") {
    activeLeg = 1;
    Serial.printf("Active leg: %s\n", legs[activeLeg].name);
  } else if (line == "rf") {
    activeLeg = 2;
    Serial.printf("Active leg: %s\n", legs[activeLeg].name);
  } else if (line == "rb") {
    activeLeg = 3;
    Serial.printf("Active leg: %s\n", legs[activeLeg].name);
  }
  // Joint commands (active leg)
  else if (line.startsWith("a ")) {
    moveAlpha(activeLeg, line.substring(2).toFloat());
  } else if (line.startsWith("b ")) {
    moveBeta(activeLeg, line.substring(2).toFloat());
  } else if (line.startsWith("g ")) {
    moveGamma(activeLeg, line.substring(2).toFloat());
  }
  // Mid / sweep
  else if (line == "midall") {
    goMidAll();
  } else if (line == "mid") {
    goMid(activeLeg);
  } else if (line == "sweepall") {
    for (int i = 0; i < 4; i++) {
      doSweep(i);
      delay(500);
    }
  } else if (line == "sweep") {
    doSweep(activeLeg);
  } else if (line == "info") {
    printInfo();
  } else if (line == "help") {
    printHelp();
  } else {
    Serial.println("Unknown command. Type 'help' for usage.");
  }
}
