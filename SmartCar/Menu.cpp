/*
*  Menu.cpp
*  Jochem Wansink
*  27-09-2024
*/
#include "Menu.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void menuSetup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Remote control");
  lcd.setCursor(0, 1);
  lcd.print("LCD Test");
}