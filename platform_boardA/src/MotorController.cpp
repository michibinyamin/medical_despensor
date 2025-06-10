#include "MotorController.h"

MotorController::MotorController(uint8_t dirPin, uint8_t stepPin,
                                 uint8_t sleepPin, uint8_t cwPin,
                                 uint8_t ccwPin)
    : dirPin(dirPin), stepPin(stepPin), sleepPin(sleepPin),
      motor(MOTOR_STEPS, dirPin, stepPin, sleepPin), ccwBtn(ccwPin),
      cwBtn(cwPin), isMoving(false), currentPosition(0),
      totalSteps(TOTAL_STEPS) {}

void MotorController::begin()
{

  // motor init
  motor.begin(RPM, MICROSTEPS);
  motor.setSpeedProfile(motor.LINEAR_SPEED, 350, 350);
  // motor.setSpeedProfile(motor.CONSTANT_SPEED);
  stopMotor();
  cwBtn.setBtnLevel(1);
  ccwBtn.setBtnLevel(1);
}

void MotorController::processCommand(String command)
{
  if (command.startsWith("motor"))
  {
    String s = command.substring(5);
    s.replace("(", "");
    s.replace(" ", "");
    s.replace(")", "");

    if (s == "calibrate")
    {
      calibrate();
    }
    else if (s.startsWith("moveTo"))
    {
      int container = s.substring(s.indexOf('-') + 1).toInt();
      Serial.println(container);
      moveToContainer(container);
    }
    else if (s.startsWith("moveSteps-"))
    {
      int steps = s.substring(s.indexOf('-') + 1).toInt();
      Serial.println(steps);
      moveSteps(steps);
    }
    else if (s == "stop")
    {
      stopMotor();
    }
    else if (s == "toStart")
    {
      moveToStart();
    }
  }
}

void MotorController::calibrate()
{
  Serial.println("calibration");
}

void MotorController::stopMotor()
{
  motor.stop();
  motor.disable();
  isMoving = false;
}

void MotorController::startMotor()
{
  motor.enable();
  Serial.println("Motor started");
}

bool MotorController::checkButtons()
{
  cwBtn.tick();
  ccwBtn.tick();

  if (cwBtn.press())
  {
    Serial.println("cwBtn pressed");
    stopMotor();
    return true;
  }
  else if (ccwBtn.press())
  {
    Serial.println("ccwBtn pressed");
    stopMotor();
    return true;
  }
  else
  {
    return false;
  }
}

void MotorController::moveSteps(int steps)
{
  startMotor();
  motor.startMove(steps);
  isMoving = true;
  while (motor.nextAction())
  {
    if (checkButtons()) // Stops motor if needed
    {
      break;
    }
  }
  stopMotor();
}

void MotorController::moveToContainer(int container_number)
{
  startMotor();
  if (container_number > 0 && container_number < 9)
  {
    motor.startMove(container_number * container_space);
    isMoving = true;
    while (motor.nextAction())
    {
      if (checkButtons()) // Stops motor if needed
      {
        break;
      }
    }
    stopMotor();
  }
}

void MotorController::moveToStart()
{
  startMotor();
  motor.startMove(80000); // Non-blocking: prepares the motion
  ccwBtn.tick();
  while (motor.nextAction())
  {
    if (checkButtons() || ccwBtn.pressing()) // Stops motor if needed
    {
      break;
    }
  }
  stopMotor();
}