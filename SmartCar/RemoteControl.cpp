// RemoteControl.cpp
// Wietse Houwers
// 27-09-2024

#include "RemoteControl.h"
#include "Ultrasonic.h"

// Functie prototypes
void updateMotorSpeeds();
void updateBlinkers();
void sendATCommand(String command);
void processCommand(String command);

// SoftwareSerial
SoftwareSerial BTSerial(BTTX, BTRX);

// Variablen voor rijden en sturen
int speedValue = 0;
int steeringValue = 0;
int leftMotorSpeed = 0;
int rightMotorSpeed = 0;

// Threshold for treating small speed and steering values as zero
const int speedThreshold = 4;
const int steeringThreshold = 4;

// Distance threshold in cm for stopping the car
const int BACKUP_SPEED = -100;  // Speed to use when backing up
const int stopDistance = 10;
int stopped = 0;


// Variabelen voor afstandstabalisatie
#define NUM_READINGS 3                // Number of readings to store
long distanceReadings[NUM_READINGS];  // Array to store distance readings
int readingIndex = 0;                 // Current index in the readings array
bool isBufferFull = false;            // Flag to check if the buffer is full

// knoppen variabelen
int ledState = LOW;  // ledState used to set the LED
const long blinkInterval = 250;
unsigned long previousMillisBlinkers = 0;  // will store last time LED was updated
int blinkers = 0;
int alarmBlinkers = 0;
int objectdetection = 1;
int highspeed = 0;


// Setup joystick functie
void setupRemoteControl() {
  // Initialize Bluetooth serial communication
  BTSerial.begin(9600);


  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  //pinMode(BUZZ, OUTPUT);

  // Set initial motor speed to 0
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  // Configure the Slave HM-10 module
  delay(1000);                // Give some time for the HM-10 to initialize
  sendATCommand("AT+ROLE0");  // Set as Slave
  delay(100);
  sendATCommand("AT+IMME1");  // Enable immediate mode (auto-connect)
  delay(100);
  sendATCommand("AT+RESET");  // Reset to apply changes
  delay(1000);
  Serial.println("Car ready to receive commands via Bluetooth.");
}



// Functie om afstand op te halen en stabiel antwoord te geven
long updateAndGetMaxDistance(int trigPin, int echoPin) {
  // Get the raw distance from the sensor
  long currentDistance = getDistance(trigPin, echoPin);

  // Ignore faulty reading of exactly 9999
  if (currentDistance != 9999) {
    // Store the current distance in the array if it's valid
    distanceReadings[readingIndex] = currentDistance;
    readingIndex = (readingIndex + 1) % NUM_READINGS;

    // Once the buffer is full, it stays full for subsequent cycles
    if (readingIndex == 0) {
      isBufferFull = true;
    }
  }

  // Determine how many readings to consider
  int count = isBufferFull ? NUM_READINGS : readingIndex;

  // Find the maximum distance from the stored readings
  long maxDistance = distanceReadings[0];
  for (int i = 1; i < count; i++) {
    // Ignore the specific value 9999 as it represents a faulty reading
    if (distanceReadings[i] != 9999 && distanceReadings[i] > maxDistance) {
      maxDistance = distanceReadings[i];
    }
  }
  return maxDistance;
}

// Main functie voor de joystick
void doRemoteControl() {
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

  // Update motor speeds and blinkers based on the latest values
  updateMotorSpeeds();
  updateBlinkers();
}


//  processCommand function to handle button and toggle commands
void processCommand(String command) {
  if (command.startsWith("SPEED:")) {  // Handle combined speed and steering command

    // Find the comma that separates the speed and steering values
    int commaIndex = command.indexOf(',');

    // Extract and process the speed value
    String speedPart = command.substring(6, commaIndex);
    int speed = speedPart.toInt();

    if (abs(speed) <= speedThreshold) {
      speed = 0;
    }
    speedValue = speed;

    // Extract and process the steering value
    String steeringPart = command.substring(commaIndex + 10);  // "+ 10" to skip over ",STEERING:"
    int steering = steeringPart.toInt();

    if (abs(steering) <= steeringThreshold) {
      steering = 0;
    }
    steeringValue = steering;
  } else if (command.startsWith("TOGGLE_LEFT:")) {
    int state = command.substring(12).toInt();
    Serial.println("TOGGLE_LEFT state: " + String(state));
    highspeed = state;
  } else if (command.startsWith("TOGGLE_RIGHT:")) {
    int state = command.substring(13).toInt();
    Serial.println("TOGGLE_RIGHT state: " + String(state));
    objectdetection = state;
  } else if (command.startsWith("TOGGLE_TOP:")) {
    int state = command.substring(11).toInt();
    Serial.println("TOGGLE_TOP state: " + String(state));
    blinkers = state;

  } else if (command.startsWith("TOGGLE_O:")) {
    int state = command.substring(9).toInt();
    Serial.println("TOGGLE_ORANGE state: " + String(state));
    alarmBlinkers = state;

  } else if (command.startsWith("BUTTON_1:")) {
    int state = command.substring(9).toInt();
    Serial.println("BUTTON_1 state: " + String(state));

  } else if (command.startsWith("BUTTON_2:")) {
    int state = command.substring(9).toInt();
    Serial.println("BUTTON_2 state: " + String(state));

  } else if (command.startsWith("BUTTON_3:")) {
    int state = command.substring(9).toInt();
    Serial.println("BUTTON_3 state: " + String(state));
    writeExpanderPin(BUZZ, state);

  } else if (command.startsWith("BUTTON_O:")) {
    int state = command.substring(9).toInt();
    Serial.println("BUTTON_ORANGE state: " + String(state));

  } else if (command.startsWith("BUTTON_R:")) {
    int state = command.substring(9).toInt();
    Serial.println("BUTTON_RED state: " + String(state));
  }
}




// Functie om motorsnelheden te berekenen op basis van de positie van de joysticks
void updateMotorSpeeds() {

  // Constants for dynamic stop distance adjustment
  const int BASE_STOP_DISTANCE = 5;    // Minimum stop distance in cm (for low speeds)
  const float DISTANCE_FACTOR = 0.08;  // Factor to adjust stop distance per unit of speed

  // Calculate the dynamic stop distance based on speed
  int dynamicStopDistance = BASE_STOP_DISTANCE + (speedValue * DISTANCE_FACTOR);

  // Cap the stop distance if needed (optional)
  dynamicStopDistance = constrain(dynamicStopDistance, BASE_STOP_DISTANCE, 40);  // Example: Max 100 cm

  // Set distance
  long filteredDistanceCenter = updateAndGetMaxDistance(TRG1, ECHO1);

  // Stop auto als object te dichtbij komt
  if (filteredDistanceCenter < dynamicStopDistance && speedValue != 0 && objectdetection == 1) {
    // Move backward to avoid the obstacle
    leftMotorSpeed = BACKUP_SPEED;
    rightMotorSpeed = BACKUP_SPEED;

    Serial.print("Backing up! ");
    Serial.println(filteredDistanceCenter);
  } else {
    // Calculate the speed for each motor
    leftMotorSpeed = speedValue - steeringValue;   // Adjusted left motor logic for correct steering
    rightMotorSpeed = speedValue + steeringValue;  // Adjusted right motor logic for correct steering

    // Keep speeds within bounds (-255 to 255)
    if (highspeed) {
      leftMotorSpeed = constrain(leftMotorSpeed, -255, 255);
      rightMotorSpeed = constrain(rightMotorSpeed, -255, 255);
    } else {
      leftMotorSpeed = constrain(leftMotorSpeed, -100, 100);
      rightMotorSpeed = constrain(rightMotorSpeed, -100, 100);
    }
  }

  // Apply motor speeds with appropriate directions
  digitalWrite(IN1, rightMotorSpeed >= 0 ? HIGH : LOW);  // Right motor forward
  digitalWrite(IN2, rightMotorSpeed < 0 ? HIGH : LOW);   // Right motor backward
  digitalWrite(IN3, leftMotorSpeed >= 0 ? HIGH : LOW);   // Left motor forward
  digitalWrite(IN4, leftMotorSpeed < 0 ? HIGH : LOW);    // Left motor backward

  // Set the actual speed values to PWM outputs
  analogWrite(ENA, abs(leftMotorSpeed));
  analogWrite(ENB, abs(rightMotorSpeed));
  //setMotors();
}



// Knipperlichten
void updateBlinkers() {
  if (alarmBlinkers == 1) {
    unsigned long currentMillisBlinkers = millis();
    if (currentMillisBlinkers - previousMillisBlinkers >= blinkInterval) {
      // save the last time you blinked the LED
      previousMillisBlinkers = currentMillisBlinkers;
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      // set the LED with the ledState of the variable:
      digitalWrite(LED1, !ledState);
      digitalWrite(LED2, ledState);
    }
  } else {
    //if (speedValue == )
    if (steeringValue == 0 && speedValue == 0 && blinkers == 1) {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
    } else if (steeringValue > 0 && blinkers == 1) {
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
    } else if (steeringValue < 0 && blinkers == 1) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
    } else {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
    }
  }
}



// Functie om AT commando's naar de afstandsbediening te sturen
void sendATCommand(String command) {
  BTSerial.println(command);
  Serial.println("Sent AT Command: " + command);
  delay(500);  // Give time for response
}
