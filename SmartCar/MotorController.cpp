/*
*  MotorController.cpp
*  Dennis Bollema
*  27-09-2024
*/
#include "MotorController.h"

void setupMotors(void) {
  // Pinmode motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Stop car
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Car Stopped");
}

// Functie gebruikt door de Joysticks en de lijn tracker
void setMotors(int Lspeed, int Rspeed) {
    // Snelheden limiteren tussen -255 en +255 voor pwm
    Lspeed = constrain(Lspeed, -255, 255);
    Rspeed = constrain(Rspeed, -255, 255);

    // Linker motorsnelheid verwerken
    if (Lspeed > 0) {
        // Vooruit
        analogWrite(ENB, Lspeed);
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    } else if (Lspeed < 0) {
        // Achteruit
        analogWrite(ENB, -Lspeed); // - gebruiken om negatief getal te inverteren
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    } else {
        // Stop
        analogWrite(ENB, 0);
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
    }

    // Rechter motorsnelheid verwerken
    if (Rspeed > 0) {
        // Vooruit
        analogWrite(ENA, Rspeed);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
    } else if (Rspeed < 0) {
        // Achteruit
        analogWrite(ENA, -Rspeed); // - gebruiken om negatief getal te inverteren
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
    } else {
        // Stop
        analogWrite(ENA, 0);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
    }

    // Knipperlicht aansturen
    if(Lspeed > Rspeed)
    {
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, LOW);
    } else if (Lspeed < Rspeed)
    {
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, HIGH);
    }
}
