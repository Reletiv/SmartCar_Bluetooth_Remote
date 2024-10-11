/*
*  ioExpander.cpp
*  Wietse Houwers
*  08-10-2024
*/
#include "ioExpander.h"

// PCF8574
PCF8574 ex1(0x20);  // PCF8574 initialiseren op addres 0x20

// Variablen 
int s1 = 0;
int s2 = 0;
int s3 = 0;
int s4 = 0;
int s5 = 0;

// io expander setup functie
void setupExpander() {
  // input pinmodes
  pinMode(ex1, LINE1, INPUT);
  pinMode(ex1, LINE2, INPUT);
  pinMode(ex1, LINE3, INPUT);
  pinMode(ex1, LINE4, INPUT);
  pinMode(ex1, LINE5, INPUT);
  pinMode(ex1, SW1, INPUT_PULLUP);
  pinMode(ex1, SW2, INPUT_PULLUP);

  // output pinmodes
  pinMode(ex1, BUZZ, OUTPUT);
  digitalWrite(ex1, BUZZ, LOW);
}

// functie om status van een pin uit te lezen
bool readExpanderPin(int pin)
{
  bool state = digitalRead(ex1, pin);
  return state;
}

// functie om naar een pin te schrijven
void writeExpanderPin(int pin, bool state)
{
  digitalWrite(ex1, pin, state); 
}

void checkLineTracker()
{
  s1 = digitalRead(ex1, LINE1);
  s2 = digitalRead(ex1, LINE2);
  s3 = digitalRead(ex1, LINE3);
  s4 = digitalRead(ex1, LINE4);
  s5 = digitalRead(ex1, LINE5);
}