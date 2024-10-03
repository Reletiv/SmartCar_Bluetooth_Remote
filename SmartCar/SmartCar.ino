/*
*
*  SmartCar.ino
*
*/

//Include files
#include "LineTracker.h"
#include "Ultrasonic.h"
#include "Bluetooth.h"
#include "Menu.h"
#include "MotorController.h"
#include "Joystick.h"


// Setup Functie
void setup() {
  Serial.begin(9600);
  //setupLineTracker();
  setupUltrasonic();
  setupJoystick();
  menuSetup();
  setupMotors();
}

// Loop Functie
void loop() {
  //doLineTracker();
  //doAutonomous();
  doJoystick();
}
