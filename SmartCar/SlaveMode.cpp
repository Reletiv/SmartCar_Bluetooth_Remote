/*
*  SlaveMode.cpp
*  Wietse Houwers
*  26-09-2024
*/
#include "SlaveMode.h"
#include "Ultrasonic.h"

// Slave Mode Functie
void doSlaveMode() {
  // Afstand Ophalen
  long distance = getDistance(TRG1, ECHO1);

  // Objectafstand meten om buzzer aan te sturen
  if (distance < SLAVEOBJDIST) {
    writeExpanderPin(BUZZ, HIGH);
  } else {
    writeExpanderPin(BUZZ, LOW);
  }

  // Line Tracker bijwerken
  checkLineTracker();

  // Aan de hand van de positie van de lijn de auto aansturen
  // Middelste sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Vooruit
    setMotors(SLAVESTRAIGHTSPEED, SLAVESTRAIGHTSPEED);
    Serial.println("Midden");
  }

  // Linker sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 0) && (s3 == 1) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Inner Speed
    setMotors(SLAVEINNERSPEED, -20);
    Serial.println("Rechts Binnenste");
  }

  // Meest linker sensor ziet een zwarte lijn
  if ((s1 == 0) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Outer Speed
    setMotors(SLAVEOUTERSPEED, -65);
    Serial.println("Rechts Buitenste");
  }

  // Rechter sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 0) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Links op Inner Speed
    setMotors(-20, SLAVEINNERSPEED);
    Serial.println("Links Binnenste");
  }

  // Meest rechter sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Links op Outer Speed
    setMotors(-65, SLAVEOUTERSPEED);
    Serial.println("Links Buitenste");
  }

  // Midden en rechter sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Links op Inner Speed
    setMotors(-20, SLAVEINNERSPEED);
    Serial.println("Links Binnenste");
  }

  // Midden en linker sensor ziet een zwarte lijn
  if ((s1 == 1) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Inner Speed
    setMotors(SLAVEINNERSPEED, -20);
    Serial.println("Rechts Binnenste");
  }

  // Midden en beide linker sensors zien een zwarte lijn
  if ((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Outer Speed
    setMotors(SLAVEOUTERSPEED, -65);
    Serial.println("Rechts Buitenste");
  }

  // Midden en beide rechter sensors zien een zwarte lijn
  if ((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Links op Outer Speed
    setMotors(-65, SLAVEOUTERSPEED);
    Serial.println("Links Buitenste");
  }

  // Alle sensors zien een zwarte lijn
  if ((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 0) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Stop
    //setMotors(0, 0);
    Serial.println("Stop");
  }

  // Afstand met object te klein
  if (distance < SLAVEOBJDIST) {
    // Stop
    setMotors(0, 0);
    Serial.println("Stop, Object Gedetecteerd");
  }
}
