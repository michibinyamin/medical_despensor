#include "MotorController.h"

MotorController::MotorController(uint8_t dirPin, uint8_t stepPin,
                                 uint8_t sleepPin, uint8_t buttomBtn,
                                 uint8_t topBtn, uint8_t pogoBtn, uint8_t opticalBtn, bool *pillCaught)
    : dirPin(dirPin), stepPin(stepPin), sleepPin(sleepPin),
      motor(MOTOR_STEPS, dirPin, stepPin, sleepPin), topBtn(topBtn),
      buttomBtn(buttomBtn), pogoBtn(pogoBtn), opticalBtn(opticalBtn), isMoving(false), currentPosition(0),
      totalSteps(TOTAL_STEPS), pillCaught(pillCaught) {}

void MotorController::begin()
{

  // motor init
  motor.begin(RPM, MICROSTEPS);
  motor.setSpeedProfile(motor.LINEAR_SPEED, 700, 350);
  stopMotor();
  buttomBtn.setBtnLevel(1);
  topBtn.setBtnLevel(1);
  pogoBtn.setBtnLevel(1);
  opticalBtn.setBtnLevel(0);
}

void MotorController::processCommand(String command)
{
  if (command.startsWith("motor"))
  {
    String s = command.substring(5);
    s.replace("(", "");
    s.replace(" ", "");
    s.replace(")", "");

    if (s == "calibrate") // Not active right now
    {
      calibrate();
    }
    else if (s.startsWith("moveTo"))
    {
      char container = s.charAt(s.indexOf('-') + 1);
      Serial.println("moving to platform " + String(container));
      moveToPlatform(container);
    }
    else if (s == "stepUp")
    {
      Serial.println("moving up"); // Moving untill optical sensor is pressed
      moveUp();
    }
    else if (s == "stop")
    {
      stopMotor();
    }
    else if (s == "toStart")
    {
      Serial.println("moving to start");
      moveToStart(); // Also resets values
    }
    else if (s == "toEnd")
    {
      Serial.println("moving to end");
      moveToEnd();
    }
    else if (s == "getPill")
    {
      motor.setRPM(SRPM);
      Serial.println("getPill command received");
      moveSteps(-15000);
    }
  }
}

void MotorController::calibrate() // not working right now
{
  Serial.println("calibration");
}

void MotorController::stopMotor()
{
  motor.stop();
  motor.disable();
  isMoving = false;
  Serial.println("motor stopped");
}

void MotorController::startMotor()
{
  motor.enable();
  Serial.println("Motor started");
}

bool MotorController::checkButtons()
{
  buttomBtn.tick();
  topBtn.tick();
  if (buttomBtn.press())
  {
    Serial.println("buttomBtn pressed");
    stopMotor();
    return true;
  }
  else if (topBtn.press())
  {
    Serial.println("topBtn pressed");
    stopMotor();
    return true;
  }
  else
  {
    return false;
  }
}

int MotorController::smartStop()
{
  pogoBtn.tick();
  opticalBtn.tick();
  if (!pogoBtn.busy())
  {
    Serial.println("pogoBtn pressed");
    stopMotor();
    return 1;
  }
  else if (opticalBtn.press()) // .press
  {
    countOptical++;
    Serial.println("opticalBtn pressed");
    stopMotor();
    return 2;
  }
  else if (*pillCaught) // this will be the vacume condition sensor it needs to receive the command from the outside
  {
    Serial.println("vacume pressure, pill caught");
    stopMotor();
    *pillCaught = false; // Reset the pill caught state
    return 3;            // Stopped
  }
  else
  {
    return 0;
  }
}

bool MotorController::opticalStop()
{
  opticalBtn.tick();
  if (opticalBtn.press())
  {
    Serial.println("opticalBtn pressed");
    countOptical--;
    stopMotor();
    return true;
  }
  return false;
}

void MotorController::moveSteps(int steps)
{
  startMotor();
  motor.startMove(steps);
  isMoving = true;
  while (motor.nextAction())
  {
    if (smartStop() != 0) // Stops motor if needed
    {
      break;
    }
  }
  stopMotor();
  motor.setRPM(RPM); // Reset RPM after moving
}

void MotorController::moveUp()
{
  // First moves elivator slightly up just in case pogopin was pressed while on optical sensor already
  constexpr int FULL_STEPS_TO_IGNORE = 50;
  constexpr int MICROSTEPS = 8; // Set this to match your driver config
  const long ignoreUntil = FULL_STEPS_TO_IGNORE * MICROSTEPS;

  startMotor();
  motor.startMove(15000);
  isMoving = true;
  long stepsTaken = 0;
  while (motor.nextAction())
  {
    stepsTaken++; // count each microstep

    if (stepsTaken >= ignoreUntil)
    {
      if (opticalStop() != 0) // end-stop triggered
      {
        break;
      }
    }
  }
  stopMotor();
}

void MotorController::moveToPlatform(char platform_letter)
{
  countOptical = 0; // Reset optical sensor count
  startMotor();
  if (platform_letter == 'a')
  {
    motor.startMove(2.2 * platform_space * 2); // dependent now on optical sensor and so the space is not really relevant
    while (motor.nextAction())
    {
      opticalBtn.tick();
      if (opticalBtn.press() && countOptical == 0)
      {
        countOptical++;
        // No break here, continue to check for the second activation
      }
      else if (opticalBtn.press() && countOptical >= 1)
      {
        Serial.println("Optical sensor activated");
        countOptical++;
        Serial.println(countOptical);
        break;
      }
      if (checkButtons()) // Stops motor if needed
      {
        break;
      }
    }
  }
  else if (platform_letter == 'b')
  {
    motor.startMove(platform_space * 2);
    while (motor.nextAction())
    {
      opticalBtn.tick();
      if (checkButtons() || opticalBtn.press()) // Stops motor if needed
      {
        Serial.println("Optical sensor activated");
        countOptical++;
        break;
      }
    }
  }
  stopMotor();
}

void MotorController::moveToStart()
{
  startMotor();
  motor.startMove(80000); // Non-blocking: prepares the motion
  while (motor.nextAction())
  {
    topBtn.tick();
    if (checkButtons() || topBtn.pressing()) // Stops motor if needed
    {
      break;
    }
  }
  stopMotor();
  countOptical = 0;
}

void MotorController::moveToEnd()
{
  startMotor();
  motor.startMove(-80000); // Non-blocking: prepares the motion
  while (motor.nextAction())
  {
    topBtn.tick();
    if (checkButtons() || topBtn.pressing()) // Stops motor if needed
    {
      break;
    }
  }
  stopMotor();
  countOptical = 0;
}