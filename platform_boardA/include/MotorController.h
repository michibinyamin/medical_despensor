#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include <BasicStepperDriver.h>
#include <EncButton.h>

class MotorController
{
public:
  MotorController(uint8_t dirPin, uint8_t stepPin, uint8_t sleepPin,
                  uint8_t ccwPin, uint8_t cwPin);

  void begin(); // initialize motor and buttons
  void processCommand(String command);
  void calibrate(); // Still not set properly
  void moveToContainer(int container_number);
  void moveSteps(int steps);
  void moveToStart();

private:
  static const uint16_t MOTOR_STEPS = 200;
  static const uint8_t MICROSTEPS = 32;
  static const uint32_t TOTAL_STEPS = 96000;
  static const uint8_t RPM = 50;
  static const long container_space = -3070;

  uint8_t dirPin;
  uint8_t stepPin;
  uint8_t sleepPin;

  BasicStepperDriver motor;
  Button cwBtn;
  Button ccwBtn;

  bool isMoving;
  int32_t currentPosition;
  int32_t targetPosition;
  int32_t stepDifference;
  int8_t direction;
  uint32_t totalSteps;

  void startMotor();
  void stopMotor();
  bool checkButtons();
  void calibrateReverse();
};

#endif // MOTOR_CONTROLLER_H