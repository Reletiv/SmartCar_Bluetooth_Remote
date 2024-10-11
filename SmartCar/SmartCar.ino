/*
*  Tracktastic Solutions 2024
*  Project Smart Car 
* 
*  SmartCar.ino
*
*/

// Include Mode files
#include "SlaveMode.h"
#include "Autonomous.h"
#include "RemoteControl.h"
#include "PhoneControl.h"

// Include Component files
#include "LCD.h"
#include "Ultrasonic.h"
#include "Bluetooth.h"
#include "MotorController.h"
#include "ioExpander.h"

// Variabelen
int carmode = 0;

// Setup Functie
void setup() {
  // Setup Serial
  Serial.begin(9600);

  // Hardware setup
  setupExpander();
  setupMotors();
  setupUltrasonic();
  setupBluetooth();
  setupLCD();

  // Setup Mode's
  setupRemoteControl();
}

// Loop Functie
void loop() {
  // Check Modus
  carmode = checkMenu();

  // 1 Phone Control
  if (carmode == 1) {
    Serial.println("remtoe");
    //doPhoneControl();
  }

  // 2 Automonous
  else if (carmode == 2) {
    doAutonomous();
  }

  // 3 Slave mode
  else if (carmode == 3) {
    doSlaveMode();
  }

  // 4 Remote Control
  else if (carmode == 4) {
    doRemoteControl();
  }
}
