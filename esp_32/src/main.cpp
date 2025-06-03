#include <Arduino.h>
#include <Wire.h>

#define Board_A_address 0x08 // Board 1
#define Board_B_address 0x09 // Board 2
// #define Board_C_address 0x10  // Board 3
#define Board_Elivator 0x11 // Elivator

#define vacume_PIN 13   // Pin for the vacuum motor
#define pressure_PIN 33 // Pressure sensor pin

bool pillCaught = false;        // Flag to indicate if the pill is trying to be caught
bool stateCatchingPill = false; // Flag to indicate if the elivator is trying to catch the pill

void send_command(String command, int address)
{
    Wire.beginTransmission(address);
    Wire.write(command.c_str());
    byte result = Wire.endTransmission(); // The actual I2C transmission begins here

    // Check transmission result
    if (result == 0)
    {
        Serial.println("Command sent successfully");
    }
    else
    {
        Serial.print("Error sending command. Code: ");
        Serial.println(result);
    }
}

void calibrate()
{
    // Calibrate Platform 1 & Platform 2 simontaniously
    // Calibrate Elivator
    // send_command("motor(toStart)",Board_A_address);
    send_command("motor(toStart)", Board_B_address);
    send_command("motor(toStart)", Board_Elivator);
}

void slavesApprove()
{
    bool approveA = true; // Change to false if you want to test the approval
    bool approveB = false;
    bool approveE = false;
    while (!(approveA && approveB && approveE))
    {
        // Wait for slaves to approve
        Serial.println("Waiting for slaves to approve...");
        delay(1000); // Wait for 1 second before checking again

        // Wire.requestFrom(Board_A_address, 1);
        // if (Wire.available())
        // {
        //     approveA = Wire.read(); // 1 = approved
        //     Serial.print("Board A approval: ");
        //     Serial.println(approveA);
        // }

        Wire.requestFrom(Board_B_address, 1);
        if (Wire.available())
        {
            approveB = Wire.read(); // 1 = approved
            Serial.print("Board B approval: ");
            Serial.println(approveB);
        }

        Wire.requestFrom(Board_Elivator, 1);
        if (Wire.available())
        {
            approveE = Wire.read(); // 1 = approved
            Serial.print("Elevator approval: ");
            Serial.println(approveE);
        }
        Serial.println(digitalRead(pressure_PIN));
        // if (stateCatchingPill && digitalRead(pressure_PIN) == 1) // Check if pressure is high
        // {
        //     send_command("pillCaught", Board_Elivator); // Send command to elivator to stop becuase pill is caught(only if pill is caught)
        //     pillCaught = true;
        //     stateCatchingPill = false;
        //     return; // If pill is caught, break the loop
        // }
    }
}

void smart_search(char platform, int container)
{
    // This function is incharge of moving the platform and elivator to find the pill
    Serial.println("Searching for pill");
    pillCaught = false;
    stateCatchingPill = true; // Set the state to catching pill
    digitalWrite(vacume_PIN, 1);
    send_command("motor(getPill)", Board_Elivator);
    slavesApprove();
    if (pillCaught)
    {
        send_command("motor(up)", Board_Elivator); // Move elivator to the platform
        slavesApprove();
        digitalWrite(vacume_PIN, 0); // Turn off the vacume for testing purposes
        pillCaught = false;
    }
    else // Change in the future(put in loop)
    {
        Serial.println("Pill not caught, retrying...");
        send_command("motor(toStart)", Board_Elivator); // Move elivator to the start position
        slavesApprove();
        digitalWrite(vacume_PIN, 0); // Turn off the vacume for testing purposes
        return;                      // Exit the function if pill is not caught
    }
}

void get_pill(char platform, int container)
{
    int approveSmartSearch_A = 1;
    int approveSmartSearch_B = 1;
    int approveSmartSearch_E = 1;

    if (platform == 'a')
    {
        send_command("motor(toStart)", Board_B_address);
        send_command("motor(moveTo-" + String(platform) + ")", Board_Elivator);
        send_command("motor(moveTo-" + String(container) + ")", Board_A_address);

        // Add request like platform b
    }
    else if (platform == 'b')
    {
        send_command("motor(moveTo-" + String(container) + ")", Board_B_address);
        send_command("motor(moveTo-" + String(platform) + ")", Board_Elivator);
    }
    slavesApprove(); // Wait for slaves to approve
    Serial.println("Slaves approved, starting smart search...");
    smart_search(platform, container);
}

void setup()
{
    Wire.begin(); // Join I2C bus as master
    Serial.begin(115200);
    delay(1000);
    Serial.println("Esp32 Started");
    pinMode(vacume_PIN, OUTPUT);
    digitalWrite(vacume_PIN, 0);
    pinMode(pressure_PIN, INPUT);
}

void loop()
{
    Serial.println(analogRead(pressure_PIN));
    // Send data to slave
    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n'); // or use Serial.readString()
        if (command == "calibrate")
        {
            calibrate();
        }
        else if (command.startsWith("get"))
        {
            int startIdx = command.indexOf('(');
            int endIdx = command.indexOf(')');
            if (startIdx != -1 && endIdx != -1 && endIdx > startIdx)
            {
                // Extract the argument inside parentheses, e.g., "A2" from "get(A2)"
                String param = command.substring(startIdx + 1, endIdx);
                if (param.length() == 2 && isAlpha(param.charAt(0)) && isDigit(param.charAt(1)))
                {
                    char letter = param.charAt(0);
                    int number = param.substring(1).toInt(); // Handles multi-digit numbers too

                    Serial.print("Letter: ");
                    Serial.println(letter);
                    Serial.print("Number: ");
                    Serial.println(number);
                    get_pill(letter, number);
                }
            }
        }
        else
        {
            Serial.println("Unknown Command");
        }
    }
}

// Commands:
// get(b2) - Get pill from platform B, container 2
// calibrate - Calibrate the platforms and elivator

// Bugs:
// 1. when buttom button or clw button is pressed, when moving to start for the first time, the motor does not move, only in the second it does