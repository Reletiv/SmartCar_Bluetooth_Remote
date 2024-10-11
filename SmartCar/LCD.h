/*
*  LCD.h
*  Jochem Wansink
*  27-09-2024
*/
#ifndef LCD
#define LCD

//Include files
#include "settings.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "ioExpander.h"
#include <EEPROM.h>

//Declare functions
int checkMenu();
void setupLCD();
void selectMode();
void currentMode(int m);
void saveWorkTime(int time);
int readWorkTime();

#endif 