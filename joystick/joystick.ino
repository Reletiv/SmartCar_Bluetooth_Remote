#include <SoftwareSerial.h>

// Define pins for joystick, buttons, and lamps
#define JOYSTICK_FB A0    // Joystick front/back
#define JOYSTICK_LR A1    // Joystick left/right
#define BUTTON_ORANGE 2   // Pushbutton orange
#define BUTTON_RED 3      // Pushbutton red
#define BUTTON_1 4        // Pushbutton 1
#define BUTTON_2 5        // Pushbutton 2
#define BUTTON_3 6        // Pushbutton 3
#define TOGGLE_TOP A5     // Togglebutton top
#define TOGGLE_LEFT A3    // Togglebutton left
#define TOGGLE_RIGHT A4   // Togglebutton right
#define LAMP_WHITE 7      // Lamp white (status light)
#define LAMP_RED 8        // Lamp red
#define LAMP_ORANGE 9     // Lamp orange
#define BTSTATE A2        // Bluetooth state
#define BTSERIAL_RX 10    // Bluetooth serial RX
#define BTSERIAL_TX 11    // Bluetooth serial TX

// SoftwareSerial object for HM-10 Bluetooth communication
SoftwareSerial BTSerial(BTSERIAL_RX, BTSERIAL_TX);

// Joystick settings
const int joystickCenter = 512;   // Center value for the joystick (0-1023 range)
const int maxSpeedValue = 255;    // Maximum speed to send (0-255)

// Bluetooth reconnection parameters
const unsigned long checkInterval = 5000; // Time interval to check for low signals (ms)
const int lowSignalThreshold = 3;        // Threshold of low signals before triggering reconnection
unsigned long lastCheckTime = 0;
int lowSignalCount = 0;

// Function prototypes
void sendJoystickCommands();
void updateLampWhiteStatus();
void reconnectBluetooth();

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  
  // Initialize Bluetooth serial communication
  BTSerial.begin(9600);

  // Configure pin modes
  pinMode(JOYSTICK_FB, INPUT);
  pinMode(JOYSTICK_LR, INPUT);
  pinMode(LAMP_WHITE, OUTPUT); // Set LAMP_WHITE as output for status indication
  pinMode(BTSTATE, INPUT);     // Set BTSTATE as input to read Bluetooth status

  // Configure the Master HM-10 module (optional)
  delay(1000); // Give some time for the HM-10 to initialize
  BTSerial.println("AT+ROLE1"); // Set as Master
  delay(500);
  BTSerial.println("AT+IMME1"); // Enable immediate mode (auto-connect)
  delay(500);
  BTSerial.println("AT+COND0B5C2D7744E"); // Replace with the car's HM-10 address
  delay(2000);
  Serial.println("Master HM-10 configured and ready.");
}

void loop() {
  // Update the LAMP_WHITE status based on the BTSTATE input
  updateLampWhiteStatus();

  // Check if Bluetooth state has been low for a threshold count and try to reconnect if necessary
  if (millis() - lastCheckTime > checkInterval) {
    lastCheckTime = millis();
    
    // If the low signal count exceeds the threshold, attempt reconnection
    if (lowSignalCount >= lowSignalThreshold) {
      reconnectBluetooth();
    }

    // Reset the counter for the next interval
    lowSignalCount = 0;
  }

  // Send joystick-based commands
  sendJoystickCommands();
  delay(100); // Adjust this delay for desired responsiveness
}

// Function to send joystick-based commands for speed and steering
void sendJoystickCommands() {
  int joystickFB = analogRead(JOYSTICK_FB);
  int joystickLR = analogRead(JOYSTICK_LR);

  // Calculate speed and steering values
  int speedValue = map(joystickFB - joystickCenter, -joystickCenter, joystickCenter, -maxSpeedValue, maxSpeedValue);
  int steeringValue = map(joystickLR - joystickCenter, -joystickCenter, joystickCenter, -maxSpeedValue, maxSpeedValue);

  // Send speed and steering commands if there is significant change
  if (speedValue != 0) {
    BTSerial.println("SPEED:" + String(speedValue));
    Serial.println("SPEED:" + String(speedValue));
  }
  
  if (steeringValue != 0) {
    BTSerial.println("STEERING:" + String(steeringValue));
    Serial.println("STEERING:" + String(steeringValue));
  }
}

// Function to update LAMP_WHITE based on the Bluetooth state (BTSTATE)
void updateLampWhiteStatus() {
  bool currentBTState = digitalRead(BTSTATE); // Read the state of the BTSTATE pin

  // If BTSTATE is high, turn on LAMP_WHITE; otherwise, turn it off
  if (currentBTState == HIGH) {
    digitalWrite(LAMP_WHITE, HIGH);
  } else {
    digitalWrite(LAMP_WHITE, LOW);
    lowSignalCount++; // Increment low signal count when BTSTATE is low
  }
}

// Function to reconnect Bluetooth if the state is low for a threshold count
void reconnectBluetooth() {
  Serial.println("Bluetooth low state detected. Attempting to reconnect...");
  
  // Send commands to reset and reconnect Bluetooth
  BTSerial.println("AT"); // Check if the module is responding
  delay(100);
  BTSerial.println("AT+RESET"); // Reset the module
  delay(1000); // Allow time for the module to reset
  BTSerial.println("AT+ROLE1"); // Set as Master
  delay(500);
  BTSerial.println("AT+IMME1"); // Enable immediate mode (auto-connect)
  delay(500);
  BTSerial.println("AT+COND0B5C2D7744E"); // Replace with the car's HM-10 address
  delay(2000);
  Serial.println("Reconnection attempt completed.");
}
