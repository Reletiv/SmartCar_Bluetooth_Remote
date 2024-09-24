#include <SoftwareSerial.h>

// Define motor control pins
const int IN1 = 7;   // Left motor forward
const int IN2 = 4;   // Left motor backward
const int IN3 = 3;   // Right motor forward
const int IN4 = 2;   // Right motor backward
const int ENA = 5;   // PWM speed control for left motor
const int ENB = 6;   // PWM speed control for right motor

// Define pins for Bluetooth module
#define BTSERIAL_RX 10   // Bluetooth serial RX
#define BTSERIAL_TX 11   // Bluetooth serial TX

// SoftwareSerial object for HM-10 Bluetooth communication
SoftwareSerial BTSerial(BTSERIAL_RX, BTSERIAL_TX);

// Variables for speed and steering
int speedValue = 0;
int steeringValue = 0;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

// Threshold for treating small speed and steering values as zero
const int speedThreshold = 4;
const int steeringThreshold = 4;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize Bluetooth serial communication
  BTSerial.begin(9600);

  // Set motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Set initial motor speed to 0 (stopped)
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  // Configure the Slave HM-10 module (optional)
  delay(1000); // Give some time for the HM-10 to initialize
  sendATCommand("AT+ROLE0"); // Set as Slave
  sendATCommand("AT+IMME1"); // Enable immediate mode (auto-connect)
  sendATCommand("AT+RESET"); // Reset to apply changes
  delay(1000);
  Serial.println("Car ready to receive commands via Bluetooth.");
}

void loop() {
  // Check for Bluetooth data
  if (BTSerial.available()) {
    String receivedData = BTSerial.readStringUntil('\n');
    receivedData.trim();
    Serial.println("Received from Bluetooth: " + receivedData);

    // Process the received command
    processCommand(receivedData);
  }

  // Check for user input from Serial Monitor (for debugging)
  if (Serial.available()) {
    String userCommand = Serial.readStringUntil('\n');
    userCommand.trim();
    BTSerial.println(userCommand);
    Serial.println("Sent to Bluetooth: " + userCommand);
  }

  // Update motor speeds based on the latest values
  updateMotorSpeeds();
}

// Function to process the received Bluetooth command
void processCommand(String command) {
  if (command.startsWith("SPEED:")) {  // Handle speed command
    int speed = command.substring(6).toInt();
    
    // Treat small speed values as zero
    if (abs(speed) <= speedThreshold) {
      speed = 0;
    }

    speedValue = speed;
    Serial.println("Set speed: " + String(speedValue));
  } 
  else if (command.startsWith("STEERING:")) {  // Handle steering command
    int steering = command.substring(9).toInt();
    
    // Treat small steering values as zero
    if (abs(steering) <= steeringThreshold) {
      steering = 0;
    }

    steeringValue = steering;
    Serial.println("Set steering: " + String(steeringValue));
  } 
}

// Function to calculate and set the motor speeds based on speed and steering
void updateMotorSpeeds() {
  // Calculate the speed for each motor
  leftMotorSpeed = -speedValue + steeringValue;  // Invert left motor speed
  rightMotorSpeed = -speedValue - steeringValue; // Invert right motor speed

  // Keep speeds within bounds (-255 to 255)
  leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);

  // Apply motor speeds with appropriate directions
  digitalWrite(IN1, leftMotorSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, leftMotorSpeed < 0 ? HIGH : LOW);
  digitalWrite(IN3, rightMotorSpeed >= 0 ? HIGH : LOW);
  digitalWrite(IN4, rightMotorSpeed < 0 ? HIGH : LOW);

  // Set the actual speed values to PWM outputs
  analogWrite(ENA, abs(leftMotorSpeed));
  analogWrite(ENB, abs(rightMotorSpeed));
}

// Function to send AT command to HM-10
void sendATCommand(String command) {
  BTSerial.println(command);
  Serial.println("Sent AT Command: " + command);
  delay(500); // Give time for response
}
