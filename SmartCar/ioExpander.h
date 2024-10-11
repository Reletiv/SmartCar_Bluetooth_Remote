/*
*  ioExpander.h
*  Wietse Houwers
*  08-10-2024
*/
#ifndef ioExpander
#define ioExpander

// Include files
#include "settings.h"
#include <Arduino.h>
#include <pcf8574.h>

// Globale Variabelen voor lijn sensor
extern int s1;
extern int s2;
extern int s3;
extern int s4;
extern int s5;

// Functies declareren
void setupExpander();
bool readExpanderPin(int pin);
void checkLineTracker();
void writeExpanderPin(int pin, bool state);

#endif 