/*
*  Ultrasonic.cpp
*  Teun Peters
*  27-09-2024
*/
#include "Ultrasonic.h"

// Lokale Functie prototypes (MOETEN NOG WEG)
long getDistance(int trigPin, int echoPin);
void stuurLinks();
void stuurRechts();
void rijdRechtdoor();


int RechtLinks = 0;

// Definieer een drempelwaarde voor obstakeldetectie in centimeters
const int threshold = 30;  // Als de afstand kleiner is dan 20 cm, stuur bij

// Variabelen om de tijd en afstand op te slaan
long distanceFront, distanceLeft, distanceRight;

// Variabele om de stuurstatus bij te houden
bool isSturenIngezet = false;  // Wanneer true, is sturen bezig en mag er niets anders gebeuren

// Rijden variable
int speed0 = 0;
int speed50 = 65;
int speed75 = 80;
int speed100 = 255;


void setupUltrasonic() {
  Serial.begin(9600);  // Start de seriële communicatie

  // Stel de pinmodi in
  pinMode(TRG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(TRG3, OUTPUT);
  pinMode(ECHO3, INPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void doAutonomous() {
  // Lees de afstanden van alle drie de sensoren
  distanceFront = getDistance(TRG1, ECHO1);
  distanceLeft = getDistance(TRG2, ECHO2);
  distanceRight = getDistance(TRG3, ECHO3);

  // Toon de afstanden in de seriële monitor
  Serial.print("Voor: ");
  Serial.print(distanceFront);
  Serial.print(" cm, Links: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Rechts: ");
  Serial.print(distanceRight);
  Serial.println(" cm");

  // Controleer of sturen al is ingezet
  if (!isSturenIngezet) {
    // Stuur op basis van de afstanden als er nog niet gestuurd wordt
    if (distanceRight < threshold) {
      // Obstakel aan de rechterkant, stuur naar links
      Serial.println("Obstakel rechts! Stuur naar links.");
      stuurLinks();
      isSturenIngezet = true;  // Zet de stuurstatus op true
    } else if (distanceLeft < threshold) {
      // Obstakel aan de linkerkant, stuur naar rechts
      Serial.println("Obstakel links! Stuur naar rechts.");
      stuurRechts();
      isSturenIngezet = true;  // Zet de stuurstatus op true
    } else if (distanceFront < threshold) {
      if (RechtLinks < 2) {
        stuurRechts();
        RechtLinks++;
      } else {
        stuurLinks();
        RechtLinks--;
      }
      isSturenIngezet = true;  // Stuurstatus op true
    } else {
      // Geen obstakels, rijd rechtdoor
      Serial.println("Geen obstakels. Rijd rechtdoor.");
      rijdRechtdoor();
    }
  } else {
    // Controleer of sturen beëindigd kan worden
    if (distanceLeft > threshold && distanceRight > threshold && distanceFront > threshold) {
      // Geen obstakels meer binnen de drempelafstand, reset sturen
      Serial.println("Geen obstakels meer, stuurstatus reset.");
      isSturenIngezet = false;  // Zet stuurstatus weer op false
    } else {
      Serial.println("Sturen is nog bezig, geen nieuwe acties mogelijk.");
    }
  }
}


/*
void doAutonomous() {
  // Lees de afstanden van alle drie de sensoren
  distanceFront = getDistance(TRG1, ECHO1);
  distanceLeft = getDistance(TRG2, ECHO2);
  distanceRight = getDistance(TRG3, ECHO3);

  // Toon de afstanden in de seriële monitor
  Serial.print("Voor: ");
  Serial.print(distanceFront);
  Serial.print(" cm, Links: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Rechts: ");
  Serial.print(distanceRight);
  Serial.println(" cm");

  // Controleer of sturen al is ingezet
  if (!isSturenIngezet) {
    // Stuur op basis van de afstanden als er nog niet gestuurd wordt
    if (distanceRight < threshold) {
      // Obstakel aan de rechterkant, stuur naar links
      Serial.println("Obstakel rechts! Stuur naar links.");
      stuurLinks();
      isSturenIngezet = true;  // Zet de stuurstatus op true
    } else if (distanceLeft < threshold) {
      // Obstakel aan de linkerkant, stuur naar rechts
      Serial.println("Obstakel links! Stuur naar rechts.");
      stuurRechts();
      isSturenIngezet = true;  // Zet de stuurstatus op true
    } else if (distanceFront < threshold) {
      if (RechtLinks < 2) {
        stuurRechts();
        RechtLinks = RechtLinks + 1;
      } else {
        stuurLinks();
        RechtLinks = RechtLinks - 1;
      }
    } else {
      // Geen obstakels, rijd rechtdoor
      Serial.println("Geen obstakels. Rijd rechtdoor.");
      rijdRechtdoor();
    }
  } else {
    // Controleer of sturen beëindigd kan worden
    if (distanceLeft > threshold && distanceRight > threshold) {
      // Geen obstakels meer binnen de drempelafstand, reset sturen
      Serial.println("Geen obstakels meer, stuurstatus reset.");
      isSturenIngezet = false;  // Zet stuurstatus weer op false
    } else {
      Serial.println("Sturen is nog bezig, geen nieuwe acties mogelijk.");
    }
  }
}
*/

// Functie om afstand te berekenen
long getDistance(int trigPin, int echoPin) {
  // Zorg ervoor dat de trigger laag is
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger een 10 microseconden puls
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Meet de tijd dat de echo binnenkomt met een Timeout van echo op 10 ms
  long duration = pulseIn(echoPin, HIGH, 10000); 

  // Bereken de afstand in cm
  long distance = duration * 0.034 / 2;

  // Voorkom afstand = 0 (out of range)
  if(distance == 0) 
  {
    distance = 9999;
    }
  return distance;
}

// Functie om naar links te sturen
void stuurLinks() {
  // Voeg hier de code in om de auto naar links te sturen
  // Bijvoorbeeld: stuur een signaal naar de motorcontroller of stuurservo
  Serial.println("Sturen naar links...");

  analogWrite(ENA, speed75);
  analogWrite(ENB, speed75);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

// Functie om naar rechts te sturen
void stuurRechts() {
  // Voeg hier de code in om de auto naar rechts te sturen
  // Bijvoorbeeld: stuur een signaal naar de motorcontroller of stuurservo
  Serial.println("Sturen naar rechts...");

  analogWrite(ENA, speed75);
  analogWrite(ENB, speed75);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

// Functie om rechtdoor te rijden
void rijdRechtdoor() {
  // Voeg hier de code in om de auto rechtdoor te laten rijden
  // Bijvoorbeeld: stuur een signaal naar de motorcontroller
  Serial.println("Rijden rechtdoor...");

  analogWrite(ENA, speed50);
  analogWrite(ENB, speed50);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}
