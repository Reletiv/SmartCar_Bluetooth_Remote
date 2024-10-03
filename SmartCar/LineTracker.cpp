/*
*  LineTracker.cpp
*  Wietse Houwers
*  26-09-2024
*/
#include "LineTracker.h"
#include "Ultrasonic.h"

// PCF8574
PCF8574 ex1(0x20);  // PCF8574 initialiseren op addres 0x20

// Line Tracker setup functie
void setupLineTracker() {
  // Pinmode voor PCF8574 
  pinMode(ex1, LINE1, INPUT); 
  pinMode(ex1, LINE2, INPUT); 
  pinMode(ex1, LINE3, INPUT);
  pinMode(ex1, LINE4, INPUT);
  pinMode(ex1, LINE5, INPUT); 
}

// Line Tracker Functie
void doLineTracker() {
  // Sensor waarden ophalen
  int s1 = digitalRead(ex1, LINE1); 
  int s2 = digitalRead(ex1, LINE2); 
  int s3 = digitalRead(ex1, LINE3); 
  int s4 = digitalRead(ex1, LINE4);  
  int s5 = digitalRead(ex1, LINE5); 
  long distance = getDistance(TRG1, ECHO1);
  Serial.println(distance);
  
  // Middelste sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Vooruit
    analogWrite(ENA, SLAVESTRAIGHTSPEED);
    analogWrite(ENB, SLAVESTRAIGHTSPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    Serial.println("Voor");
  }

  // Linker sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 0) && (s3 == 1) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Inner Speed
    analogWrite(ENA, SLAVEINNERSPEED);
    analogWrite(ENB, SLAVEINNERSPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Meest linker sensor ziet een zwarte lijn
  if((s1 == 0) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Outer Speed
    analogWrite(ENA, SLAVEOUTERSPEED);
    analogWrite(ENB, SLAVEOUTERSPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  // Rechter sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 0) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Links op Inner Speed
    analogWrite(ENA, SLAVEINNERSPEED);
    analogWrite(ENB, SLAVEINNERSPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // Meest rechter sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 1) && (s3 == 1) && (s4 == 1) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Links op Outer Speed
    analogWrite(ENA, SLAVEOUTERSPEED);
    analogWrite(ENB, SLAVEOUTERSPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // Midden en rechter sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Links op Inner Speed
    analogWrite(ENA, SLAVEINNERSPEED);
    analogWrite(ENB, SLAVEINNERSPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // Midden en linker sensor ziet een zwarte lijn
  if((s1 == 1) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Inner Speed
    analogWrite(ENA, SLAVEINNERSPEED);
    analogWrite(ENB, SLAVEINNERSPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Midden en beide linker sensors zien een zwarte lijn
  if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 1) && (s5 == 1) && (distance > SLAVEOBJDIST)) {
    // Rechts op Outer Speed
    analogWrite(ENA, SLAVEOUTERSPEED);
    analogWrite(ENB, SLAVEOUTERSPEED);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Midden en beide rechter sensors zien een zwarte lijn
  if((s1 == 1) && (s2 == 1) && (s3 == 0) && (s4 == 0) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Links op Outer Speed
    analogWrite(ENA, SLAVEOUTERSPEED);
    analogWrite(ENB, SLAVEOUTERSPEED);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  // Alle sensors zien een zwarte lijn
  if((s1 == 0) && (s2 == 0) && (s3 == 0) && (s4 == 0) && (s5 == 0) && (distance > SLAVEOBJDIST)) {
    // Stop 
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("Stop");
  }
  if(distance < SLAVEOBJDIST)
  {
    // Stop 
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    Serial.println("Stop");   
  }
}
