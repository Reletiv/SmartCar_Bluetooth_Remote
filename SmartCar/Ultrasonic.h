/*
*  Ultrasonic.h
*  Teun Peters
*  27-09-2024
*/
#ifndef Ultrasonic
#define Ultrasonic

//Include files
#include "settings.h"
#include <Arduino.h>

//Declare functions
void setupUltrasonic();
void doAutonomous();
long getDistance(int trigPin, int echoPin);
#endif 
