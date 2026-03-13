
/**
 * Specific for robot config
 */
// default legs state (TODO measure it!!!)
#define LEG_BODY_X      39.5
#define LEG_BODY_Y_F    70.5  // not sure
#define LEG_BODY_Y_H    70.5
#define LEG_BODY_Z       0.0
#define LEG_POINT_X     56.0
#define LEG_POINT_Y_F   70.5 // not sure
#define LEG_POINT_Y_H   70.5
#define LEG_POINT_Z     80.0

// Robot config
#define LEG_NUM 4


// Size of leg parts in mm
#define LEG_SIZE_L1   0.0
#define LEG_SIZE_L2  51.9
#define LEG_SIZE_L3  50.0

point balanceOffset = {0, 0, 0};

leg legs[LEG_NUM] = {
  // LEFT FRONT (channels 0,1,2)
  // Alpha: 0-90° (0=Outward, 90=Inward, mid~65°)
  // Beta:  0-115° (0=Front, 115=Back)
  // Gamma: 0-135° (0=Back, 135=Front)
  {
    {LEGLF, "LF"},
    { -LEG_BODY_X,  LEG_BODY_Y_F,   LEG_BODY_Z},
    {-LEG_POINT_X, LEG_POINT_Y_F, -LEG_POINT_Z},
    { LEG_SIZE_L1,   LEG_SIZE_L2,  LEG_SIZE_L3},
    {LEG_ANGLE_ALPHA_MIN, LEG_ANGLE_BETA_MIN, LEG_ANGLE_GAMMA_MIN},
    {LEG_ANGLE_ALPHA_MAX, LEG_ANGLE_BETA_MAX, LEG_ANGLE_GAMMA_MAX},
    {
      {M_PI_2, M_PI_2, M_PI_2},  // hardware middle
      {  0,  1,  2},             // PCA9685 channels
      {  0,  0,  0},             // servo middle trim
      {  2,  1,  1}              // gear ratio
    },
    {    -LEG_BODY_X,   LEG_BODY_Y_F,      LEG_BODY_Z},
    {   -LEG_POINT_X,  LEG_POINT_Y_F,    -LEG_POINT_Z},
    {LEG_ANGLE_ALPHA, LEG_ANGLE_BETA, LEG_ANGLE_GAMMA},
    {true, true, false, false, true, true},
    {true, 0, 0}
  },
  // RIGHT FRONT (channels 6,7,8)
  // Alpha: 10-135° (135=Outward, 10=Inward, mid~50°) — inverted vs LF
  // Beta:  0-135° (135=Front, 0=Back) — inverted vs LF
  // Gamma: 0-135° (0=Front, 135=Back)
  {
    {LEGRF, "RF"},
    { LEG_BODY_X,  LEG_BODY_Y_F,   LEG_BODY_Z},
    {LEG_POINT_X, LEG_POINT_Y_F, -LEG_POINT_Z},
    {LEG_SIZE_L1,   LEG_SIZE_L2,  LEG_SIZE_L3},
    {LEG_ANGLE_ALPHA_MIN, LEG_ANGLE_BETA_MIN, LEG_ANGLE_GAMMA_MIN},
    {LEG_ANGLE_ALPHA_MAX, LEG_ANGLE_BETA_MAX, LEG_ANGLE_GAMMA_MAX},
    {
      {M_PI_2, M_PI_2, M_PI_2},
      {  6,  7,  8},             // PCA9685 channels
      {  0,  0,  0},
      {  2,  1,  1}
    },
    {     LEG_BODY_X,   LEG_BODY_Y_F,      LEG_BODY_Z},
    {    LEG_POINT_X,  LEG_POINT_Y_F,    -LEG_POINT_Z},
    {LEG_ANGLE_ALPHA, LEG_ANGLE_BETA, LEG_ANGLE_GAMMA},
    {false, true, false, true, false, false},
    {true, 0, 0}
  },
  // LEFT HIND / LEFT BACK (channels 3,4,5)
  // Alpha: 0-130° (0=Inward, 130=Outward, mid~65°) — inverted vs LF
  // Beta:  0-135° (0=Front, 135=Back)
  // Gamma: 0-135° (0=Back, 135=Front) — same as LF
  {
    {LEGLH, "LH"},
    { -LEG_BODY_X,  -LEG_BODY_Y_H,   LEG_BODY_Z},
    {-LEG_POINT_X, -LEG_POINT_Y_H, -LEG_POINT_Z},
    { LEG_SIZE_L1,    LEG_SIZE_L2,  LEG_SIZE_L3},
    {LEG_ANGLE_ALPHA_MIN, LEG_ANGLE_BETA_MIN, LEG_ANGLE_GAMMA_MIN},
    {LEG_ANGLE_ALPHA_MAX, LEG_ANGLE_BETA_MAX, LEG_ANGLE_GAMMA_MAX},
    {
      {M_PI_2, M_PI_2, M_PI_2},
      {  3,  4,  5},             // PCA9685 channels
      {  0,  0,  0},
      {  2,  1,  1}
    },
    {    -LEG_BODY_X,  -LEG_BODY_Y_H,      LEG_BODY_Z},
    {   -LEG_POINT_X, -LEG_POINT_Y_H,    -LEG_POINT_Z},
    {LEG_ANGLE_ALPHA, LEG_ANGLE_BETA, LEG_ANGLE_GAMMA},
    {true, true, false, true, true, true},
    {true, 0, 0}
  },
  // RIGHT HIND / RIGHT BACK (channels 9,10,11)
  // Alpha: 0-90° (0=Outward, 90=Inward, mid~50°)
  // Beta:  0-135° (135=Front, 0=Back) — inverted vs LF
  // Gamma: 0-135° (0=Front, 135=Back)
  {
    {LEGRH, "RH"},
    { LEG_BODY_X,  -LEG_BODY_Y_H,   LEG_BODY_Z},
    {LEG_POINT_X, -LEG_POINT_Y_H, -LEG_POINT_Z},
    {LEG_SIZE_L1,    LEG_SIZE_L2,  LEG_SIZE_L3},
    {LEG_ANGLE_ALPHA_MIN, LEG_ANGLE_BETA_MIN, LEG_ANGLE_GAMMA_MIN},
    {LEG_ANGLE_ALPHA_MAX, LEG_ANGLE_BETA_MAX, LEG_ANGLE_GAMMA_MAX},
    {
      {M_PI_2, M_PI_2, M_PI_2},
      {  9, 10, 11},             // PCA9685 channels
      {  0,  0,  0},
      {  2,  1,  1}
    },
    {     LEG_BODY_X,  -LEG_BODY_Y_H,      LEG_BODY_Z},
    {    LEG_POINT_X, -LEG_POINT_Y_H,    -LEG_POINT_Z},
    {LEG_ANGLE_ALPHA, LEG_ANGLE_BETA, LEG_ANGLE_GAMMA},
    {false, true, false, false, false, false},
    {true, 0, 0}
  }
};

/**
 * Not all servos are perfect
 * 
 * This is for TowerPro MG90D (or maybe I have clone, who knows)
 */
servoProfile servoMainProfile = {
    20,   // minAngle
    160,  // maxAngle
    780,  // degMin
    890,  // deg30
    1090, // deg50
    1300, // deg70
    1500, // deg90 - middle
    1700, // deg110
    1900, // deg130
    2110, // deg150
    2200  // degMax
};

/**
 * My Trim settings
 * 0 leg trim {-11.000790, -3.093972, 2.979381}
 * 1 leg trim {-0.572958, 4.927437, -3.093972}
 * 2 leg trim {9.969466, 0.000000, 2.979381}
 * 3 leg trim {4.927437, 0.000000, -7.104677}
 */
