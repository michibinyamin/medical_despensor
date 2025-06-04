#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include <BasicStepperDriver.h>
#include <EncButton.h>

class MotorController
{
public:
  MotorController(uint8_t dirPin, uint8_t stepPin, uint8_t sleepPin,
                  uint8_t ccwPin, uint8_t cwPin, uint8_t pogoPin, uint8_t opticalPin, bool *pillCaught);

  void begin(); // initialize motor and buttons
  void processCommand(String command);
  void calibrate(); // Still not set properly
  void moveToPlatform(char platform_letter);
  void moveSteps(int steps);
  void moveToStart();
  void moveToEnd();

private:
  static const uint16_t MOTOR_STEPS = 200;
  static const uint8_t MICROSTEPS = 32;
  static const uint32_t TOTAL_STEPS = 96000;
  static const uint8_t RPM = 50;
  static const long platform_space = -9000;

  uint8_t dirPin;
  uint8_t stepPin;
  uint8_t sleepPin;

  BasicStepperDriver motor;
  Button buttomBtn;
  Button topBtn;
  Button pogoBtn;
  Button opticalBtn;

  bool isMoving;
  int32_t currentPosition;
  int32_t targetPosition;
  int32_t stepDifference;
  int8_t direction;
  uint32_t totalSteps;
  int countOptical = 0;
  bool *pillCaught;

  void startMotor();
  void stopMotor();
  bool checkButtons();
  int smartStop();
  bool opticalStop();
  void moveUp();
  void calibrateReverse();
};

#endif // MOTOR_CONTROLLER_H