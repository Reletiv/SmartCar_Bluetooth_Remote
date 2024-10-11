/*
*  MotorController.h
*  Dennis Bollema
*  27-09-2024
*/
#ifndef MotorController
#define MotorController

//Include files
#include "settings.h"
#include <Arduino.h>

//Declare functions
void setupMotors(void);
void setMotors(int Lspeed, int Rspeed);

#endif 