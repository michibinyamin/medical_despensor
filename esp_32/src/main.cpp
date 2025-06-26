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

int pressureValue = 300; // Variable to store the pressure value

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

void resetPositions()
{
    // move to start Platform 1, Platform 2, and Elivator simultaneously
    send_command("motor(toStart)", Board_A_address);
    send_command("motor(toStart)", Board_B_address);
    send_command("motor(toStart)", Board_Elivator);
}

void slavesApprove()
{
    bool approveA = false;
    bool approveB = false;
    bool approveE = false;
    Serial.println("Waiting for slaves to approve...");
    while (!(approveA && approveB && approveE))
    {
        // Wait for slaves to approve
        Wire.requestFrom(Board_A_address, 1);
        if (Wire.available())
        {
            approveA = Wire.read(); // 1 = approved
            // Serial.print("Board A approval: ");
            // Serial.println(approveA);
        }

        Wire.requestFrom(Board_B_address, 1);
        if (Wire.available())
        {
            approveB = Wire.read(); // 1 = approved
            // Serial.print("Board B approval: ");
            // Serial.println(approveB);
        }

        Wire.requestFrom(Board_Elivator, 1);
        if (Wire.available())
        {
            approveE = Wire.read(); // 1 = approved
            // Serial.print("Elevator approval: ");
            // Serial.println(approveE);
        }
        if (stateCatchingPill && analogRead(pressure_PIN) < pressureValue) // Check if pressure is high
        {
            send_command("pillCaught", Board_Elivator); // Send command to elivator to stop becuase pill is caught(only if pill is caught)
            Serial.println("Pill Caught!");
            pillCaught = true;
            stateCatchingPill = false;
            break; // If pill is caught, break the loop
        }
    }
    Serial.println("All slaves approved the command");
}

void smart_search(char platform, int container)
{
    // This function is incharge of moving the platform and elivator to find the pill
    Serial.println("Searching for pill");
    pillCaught = false;
    stateCatchingPill = true;
    int i = 1;
    int m = 1;
    while (!pillCaught)
    {
        digitalWrite(vacume_PIN, 1);
        send_command("motor(getPill)", Board_Elivator);
        slavesApprove(); // Stops becuase of limits or because caught pill
        if (pillCaught)
        {
            send_command("motor(stepUp)", Board_Elivator); // Move elivator up
            slavesApprove();
            send_command("motor(toStart)", Board_B_address); // CHANGE TO CORRECT PLATFORM
            send_command("motor(toStart)", Board_A_address);
            slavesApprove();
            send_command("motor(toEnd)", Board_Elivator); // Move elivator to the platform
            slavesApprove();
            digitalWrite(vacume_PIN, 0);
            delay(500);
            send_command("motor(toStart)", Board_Elivator);
        }
        else
        {
            digitalWrite(vacume_PIN, 0);
            Serial.println("Pill not caught, retrying...");
            send_command("motor(stepUp)", Board_Elivator); // Move elivator to the platform
            slavesApprove();
            if (i < 5)
            {
                if (platform == 'a')
                {
                    send_command("motor(moveSteps-" + String(i * 150 * m) + ")", Board_A_address); // Move platform to start position
                }
                else
                {
                    send_command("motor(moveSteps-" + String(i * 150 * m) + ")", Board_B_address); // Move platform to start position
                }
                slavesApprove();
                i++;
                m = m * -1;
            }
            else
            {
                send_command("motor(toStart)", Board_Elivator);
                slavesApprove();
                send_command("motor(toStart)", Board_B_address);
                send_command("motor(toStart)", Board_A_address);
                digitalWrite(vacume_PIN, 0);
                slavesApprove();
                return;
            }
            digitalWrite(vacume_PIN, 0);
        }
    }
    slavesApprove();
    pillCaught = false;
}

void get_pill(char platform, int container)
{
    int approveSmartSearch_A = 1;
    int approveSmartSearch_B = 1;
    int approveSmartSearch_E = 1;

    if (platform == 'a')
    {
        send_command("motor(toStart)", Board_B_address);
        slavesApprove();
        send_command("motor(moveTo-" + String(container) + ")", Board_A_address);
        send_command("motor(moveTo-" + String(platform) + ")", Board_Elivator);
    }
    else if (platform == 'b')
    {
        send_command("motor(toStart)", Board_A_address);
        send_command("motor(moveTo-" + String(container) + ")", Board_B_address);
        send_command("motor(moveTo-" + String(platform) + ")", Board_Elivator);
    }
    slavesApprove(); // Wait for slaves to approve
    Serial.println("Starting smart search...");
    smart_search(platform, container);
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    delay(1000);
    Serial.println("Esp32 Started");
    pinMode(vacume_PIN, OUTPUT);
    digitalWrite(vacume_PIN, 0);
    pinMode(pressure_PIN, INPUT);
    resetPositions();
}

void loop()
{
    if (Serial.available() > 0) // דפיקה בדלת
    {
        String command = Serial.readStringUntil('\n'); // or use Serial.readString() פתיחת
        if (command == "reset")
        {
            resetPositions();
        }
        else if (command.startsWith("get"))
        {
            resetPositions();
            slavesApprove();
            int startIdx = command.indexOf('(');
            int endIdx = command.indexOf(')');
            if (startIdx != -1 && endIdx != -1 && endIdx > startIdx)
            {
                String params = command.substring(startIdx + 1, endIdx);
                int lastIndex = 0;
                while (lastIndex < params.length())
                {
                    int commaIdx = params.indexOf(',', lastIndex);
                    String token;
                    if (commaIdx == -1)
                    {
                        token = params.substring(lastIndex);
                        lastIndex = params.length();
                    }
                    else
                    {
                        token = params.substring(lastIndex, commaIdx);
                        lastIndex = commaIdx + 1;
                    }

                    token.trim();
                    if (token.length() >= 2 && isAlpha(token.charAt(0)))
                    {
                        char letter = token.charAt(0);
                        int number = token.substring(1).toInt();
                        Serial.print("Letter: ");
                        Serial.println(letter);
                        Serial.print("Number: ");
                        Serial.println(number);
                        get_pill(letter, number);
                    }
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
// get(a1,b3,b3) - Get pill from platform A, container 1, and platform B, container 3 twice
// reset - Reset the positions of the platforms and elivator