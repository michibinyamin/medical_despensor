#include <Arduino.h>
#include <Wire.h>

// Slave Arduino address
#define SLAVE_ADDRESS 0x08

// Command buffer
#define BUFFER_SIZE 32
char commandBuffer[BUFFER_SIZE];

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  Serial.println("Master Arduino started");
  Serial.println("Available commands:");
  Serial.println("neck(calibrate) - motor calibration");
  Serial.println("neck(down) - move down to 75%");
  Serial.println("neck(up) - move up to 0%");
  Serial.println("neck(mX) - move to X percent (0-100)");
  Serial.println("neck(stop) - stop motor");
  Serial.println("neck(portrait) - set portrait mode");
  Serial.println("neck(landscape) - set landscape mode");

  // Initialize I2C as Master
  Wire.begin();
}

void loop() {
  if (Serial.available() > 0) {
    // Read command from Serial
    String command = Serial.readStringUntil('\n');
    command.trim();

    // Check if command starts with "neck"
    if (command.startsWith("neck")) {
      Serial.print("Sending command: ");
      Serial.println(command);

      // Send command via I2C
      Wire.beginTransmission(SLAVE_ADDRESS);
      Wire.write(command.c_str());
      byte result = Wire.endTransmission();

      // Check transmission result
      if (result == 0) {
        Serial.println("Command sent successfully");
      } else {
        Serial.print("Error sending command. Code: ");
        Serial.println(result);
      }
    } else {
      Serial.println("Invalid command. Command must start with 'neck'");
    }
  }
}