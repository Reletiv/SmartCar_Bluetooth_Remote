#include <SoftwareSerial.h>

// Define pins for joystick, buttons, and lamps
#define JOYSTICK_FB A0   // Joystick front/back
#define JOYSTICK_LR A1   // Joystick left/right
#define BUTTON_ORANGE 2  // Pushbutton orange
#define BUTTON_RED 3     // Pushbutton red
#define BUTTON_1 4       // Pushbutton 1
#define BUTTON_2 5       // Pushbutton 2
#define BUTTON_3 6       // Pushbutton 3
#define TOGGLE_TOP A5    // Togglebutton top
#define TOGGLE_LEFT A3   // Togglebutton left
#define TOGGLE_RIGHT A4  // Togglebutton right
#define LAMP_WHITE 7     // Lamp white (status light)
#define LAMP_RED 8       // Lamp red
#define LAMP_ORANGE 9    // Lamp orange
#define BTSTATE A2       // Bluetooth state
#define BTSERIAL_RX 10   // Bluetooth serial RX
#define BTSERIAL_TX 11   // Bluetooth serial TX

// SoftwareSerial object for HM-10 Bluetooth communication
SoftwareSerial BTSerial(BTSERIAL_RX, BTSERIAL_TX);

// Joystick settings
const int joystickCenter = 512;  // Center value for the joystick (0-1023 range)
const int maxSpeedValue = 255;   // Maximum speed to send (0-255)


// global variables to store the last known state of each button and toggle
bool lastToggleLeftState = LOW;
bool lastToggleRightState = LOW;
bool lastToggleTopState = LOW;
bool lastButton1State = LOW;
bool lastButton2State = LOW;
bool lastButton3State = LOW;
bool lastButtonOrangeState = LOW;
bool lastButtonRedState = LOW;


// Bluetooth reconnection parameters
const unsigned long checkInterval = 5000;  // Time interval to check for low signals (ms)
const int lowSignalThreshold = 3;          // Threshold of low signals before triggering reconnection
unsigned long lastCheckTime = 0;
int lowSignalCount = 0;

// Status update
unsigned long lastButtonStateSendTime = 0;
const unsigned long buttonStateSendInterval = 5000;  // 5 seconds

// Global variables to track the toggle state and blinking behavior
bool toggleOrange = false;  // Virtual toggle state for BUTTON_ORANGE
unsigned long lastBlinkTime = 0;  // Timestamp to control the blink interval
bool blinkState = false;  // State to control the blinking on/off
const unsigned long blinkInterval = 500;  // Blink interval in milliseconds



// Function prototypes
void sendJoystickCommands();
void updateLampWhiteStatus();
void reconnectBluetooth();
void sendButtonCommands();
void sendAllButtonStates();

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize Bluetooth serial communication
  BTSerial.begin(9600);

  // Configure pin modes
  pinMode(JOYSTICK_FB, INPUT);
  pinMode(JOYSTICK_LR, INPUT);
  pinMode(LAMP_WHITE, OUTPUT);
  pinMode(LAMP_RED, OUTPUT);
  pinMode(LAMP_ORANGE, OUTPUT);
  pinMode(BTSTATE, INPUT);

  // Set pin modes for the buttons and toggles with pull-up resistors
  pinMode(TOGGLE_LEFT, INPUT_PULLUP);   // Set TOGGLE_LEFT pin as input with pull-up
  pinMode(TOGGLE_RIGHT, INPUT_PULLUP);  // Set TOGGLE_RIGHT pin as input with pull-up
  pinMode(TOGGLE_TOP, INPUT_PULLUP);    // Set TOGGLE_TOP pin as input with pull-up

  pinMode(BUTTON_1, INPUT_PULLUP);       // Set BUTTON_1 pin as input with pull-up
  pinMode(BUTTON_2, INPUT_PULLUP);       // Set BUTTON_2 pin as input with pull-up
  pinMode(BUTTON_3, INPUT_PULLUP);       // Set BUTTON_3 pin as input with pull-up
  pinMode(BUTTON_ORANGE, INPUT_PULLUP);  // Set BUTTON_ORANGE pin as input with pull-up
  pinMode(BUTTON_RED, INPUT_PULLUP);     // Set BUTTON_RED pin as input with pull-up

  // Configure the Master HM-10 module (optional)
  delay(1000);                   // Give some time for the HM-10 to initialize
  BTSerial.println("AT+ROLE1");  // Set as Master
  delay(500);
  BTSerial.println("AT+IMME1");  // Enable immediate mode (auto-connect)
  delay(500);
  //BTSerial.println("AT+COND0B5C2D7744E");  // Replace with the car's HM-10 address
  BTSerial.println("AT+CON84C692874862");  // Replace with the car's HM-10 address School
  delay(2000);
  Serial.println("Master HM-10 configured and ready.");
}

void loop() {
  // Update the LAMP_WHITE status based on the BTSTATE input
  updateLampWhiteStatus();



  // Handle the blinking logic for LAMP_ORANGE if toggleOrange is ON
  if (toggleOrange) {
    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= blinkInterval) {
      // Toggle the blink state and update LAMP_ORANGE
      blinkState = !blinkState;
      digitalWrite(LAMP_ORANGE, blinkState ? HIGH : LOW);
      lastBlinkTime = currentTime;  // Reset the blink timer
    }
  } else {
    // Ensure the orange lamp is off when toggle is not active
    digitalWrite(LAMP_ORANGE, LOW);
  }


  // Send button and toggle commands to the car
  sendButtonCommands();



  unsigned long currentTime = millis();

  // Send button states every 5 seconds
  if (currentTime - lastButtonStateSendTime > buttonStateSendInterval) {
    sendAllButtonStates();
    lastButtonStateSendTime = currentTime;  // Update the last send time
  }





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
  delay(50);  // Adjust this delay for desired responsiveness
}





// Function to send the current state of all buttons
void sendAllButtonStates() {
  // Read the current states of all buttons and toggles
  bool currentToggleLeftState = !digitalRead(TOGGLE_LEFT);
  bool currentToggleRightState = !digitalRead(TOGGLE_RIGHT);
  bool currentToggleTopState = !digitalRead(TOGGLE_TOP);
  bool currentButton1State = !digitalRead(BUTTON_1);
  bool currentButton2State = !digitalRead(BUTTON_2);
  bool currentButton3State = !digitalRead(BUTTON_3);
  bool currentButtonOrangeState = !digitalRead(BUTTON_ORANGE);
  bool currentButtonRedState = !digitalRead(BUTTON_RED);

  // Update red lamp
  digitalWrite(LAMP_RED, !currentToggleRightState);
  // Send all button states
  BTSerial.println("TOGGLE_LEFT:" + String(currentToggleLeftState));
  BTSerial.println("TOGGLE_RIGHT:" + String(currentToggleRightState));
  BTSerial.println("TOGGLE_TOP:" + String(currentToggleTopState));
  BTSerial.println("TOGGLE_O:" + String(toggleOrange));
  BTSerial.println("BUTTON_1:" + String(currentButton1State));
  BTSerial.println("BUTTON_2:" + String(currentButton2State));
  BTSerial.println("BUTTON_3:" + String(currentButton3State));
  BTSerial.println("BUTTON_O:" + String(currentButtonOrangeState));
  BTSerial.println("BUTTON_R:" + String(currentButtonRedState)); 
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
    //Serial.println("SPEED:" + String(speedValue));
  }

  if (steeringValue != 0) {
    BTSerial.println("STEERING:" + String(steeringValue));
    //Serial.println("STEERING:" + String(steeringValue));
  }
}

// Function to update LAMP_WHITE based on the Bluetooth state (BTSTATE)
void updateLampWhiteStatus() {
  bool currentBTState = digitalRead(BTSTATE);  // Read the state of the BTSTATE pin

  // If BTSTATE is high, turn on LAMP_WHITE; otherwise, turn it off
  if (currentBTState == HIGH) {
    digitalWrite(LAMP_WHITE, HIGH);
  } else {
    digitalWrite(LAMP_WHITE, LOW);
    lowSignalCount++;  // Increment low signal count when BTSTATE is low
  }
}

// Function to reconnect Bluetooth if the state is low for a threshold count
void reconnectBluetooth() {
  Serial.println("Bluetooth low state detected. Attempting to reconnect...");

  // Send commands to reset and reconnect Bluetooth
  BTSerial.println("AT");  // Check if the module is responding
  delay(100);
  BTSerial.println("AT+RESET");  // Reset the module
  delay(500);                   // Allow time for the module to reset
  BTSerial.println("AT+ROLE1");  // Set as Master
  delay(100);
  BTSerial.println("AT+IMME1");  // Enable immediate mode (auto-connect)
  delay(100);
  //BTSerial.println("AT+COND0B5C2D7744E");  // Replace with the car's HM-10 address
  BTSerial.println("AT+CON84C692874862");  // Replace with the car's HM-10 address School
  delay(800);
  Serial.println("Reconnection attempt completed.");
}



// Function to send button and toggle statuses
void sendButtonCommands() {
  // Read the current states of all buttons and toggles
  bool currentToggleLeftState = !digitalRead(TOGGLE_LEFT);
  bool currentToggleRightState = !digitalRead(TOGGLE_RIGHT);
  bool currentToggleTopState = !digitalRead(TOGGLE_TOP);
  bool currentButton1State = !digitalRead(BUTTON_1);
  bool currentButton2State = !digitalRead(BUTTON_2);
  bool currentButton3State = !digitalRead(BUTTON_3);
  bool currentButtonOrangeState = !digitalRead(BUTTON_ORANGE);
  bool currentButtonRedState = !digitalRead(BUTTON_RED);



  // Handle virtual toggle for BUTTON_ORANGE
  if (currentButtonOrangeState != lastButtonOrangeState) {
    lastButtonOrangeState = currentButtonOrangeState;

    if (currentButtonOrangeState == HIGH) {
      // Toggle the state when the button is pressed
      toggleOrange = !toggleOrange;
      Serial.println("TOGGLE_ORANGE: " + String(toggleOrange));  // Debug output
      BTSerial.println("TOGGLE_O: " + String(toggleOrange));  // Send toggle state
    }

    // Send original BUTTON_ORANGE state
    BTSerial.println("BUTTON_O:" + String(currentButtonOrangeState));
    Serial.println("BUTTON_ORANGE:" + String(currentButtonOrangeState));  // Debug output
  }



  // Check if any state has changed and send the corresponding command to the car
  if (currentToggleLeftState != lastToggleLeftState) {
    lastToggleLeftState = currentToggleLeftState;
    BTSerial.println("TOGGLE_LEFT:" + String(currentToggleLeftState));
    Serial.println("TOGGLE_LEFT:" + String(currentToggleLeftState));  // Debug output
  }

  if (currentToggleRightState != lastToggleRightState) {
    lastToggleRightState = currentToggleRightState;
    BTSerial.println("TOGGLE_RIGHT:" + String(currentToggleRightState));
    Serial.println("TOGGLE_RIGHT:" + String(currentToggleRightState));  // Debug output
    digitalWrite(LAMP_RED, !currentToggleRightState);
  }

  if (currentToggleTopState != lastToggleTopState) {
    lastToggleTopState = currentToggleTopState;
    BTSerial.println("TOGGLE_TOP:" + String(currentToggleTopState));
    Serial.println("TOGGLE_TOP:" + String(currentToggleTopState));  // Debug output
  }

  if (currentButton1State != lastButton1State) {
    lastButton1State = currentButton1State;
    BTSerial.println("BUTTON_1:" + String(currentButton1State));
    Serial.println("BUTTON_1:" + String(currentButton1State));  // Debug output
  }

  if (currentButton2State != lastButton2State) {
    lastButton2State = currentButton2State;
    BTSerial.println("BUTTON_2:" + String(currentButton2State));
    Serial.println("BUTTON_2:" + String(currentButton2State));  // Debug output
  }

  if (currentButton3State != lastButton3State) {
    lastButton3State = currentButton3State;
    BTSerial.println("BUTTON_3:" + String(currentButton3State));
    Serial.println("BUTTON_3:" + String(currentButton3State));  // Debug output
  }

  //if (currentButtonOrangeState != lastButtonOrangeState) {
  //  lastButtonOrangeState = currentButtonOrangeState;
  //  BTSerial.println("BUTTON_O:" + String(currentButtonOrangeState));
  //  Serial.println("BUTTON_ORANGE:" + String(currentButtonOrangeState));  // Debug output
  //}

  if (currentButtonRedState != lastButtonRedState) {
    lastButtonRedState = currentButtonRedState;
    BTSerial.println("BUTTON_R:" + String(currentButtonRedState));
    Serial.println("BUTTON_RED:" + String(currentButtonRedState));  // Debug output
  }
}
