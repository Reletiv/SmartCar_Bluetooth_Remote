/*
*  LCD.cpp
*  Jochem Wansink
*  27-09-2024
*/
#include "LCD.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

int confirmbtnState = HIGH;
int confirmlastbtnState = HIGH;
int cyclebtnState = HIGH;
int cyclelastbtnState = HIGH;

byte upArrow[8] = { B00000, B00100, B00100, B01110, B01110, B11111, B11111, B00100 };
byte downArrow[8] = { B00100, B11111, B11111, B01110, B01110, B00100, B00100, B00000 };
byte rightArrow[8] = { B01000, B01100, B01110, B11111, B01110, B01100, B01000, B00000 };
byte leftArrow[8] = { B00010, B00110, B01110, B11111, B01110, B00110, B00010, B00000 };
byte horLine[8] = { B00000, B00000, B00000, B11111, B00000, B00000, B00000, B00000 };
byte verLine[8] = { B00100, B00100, B00100, B00100, B00100, B00100, B00100, B00100 };
const byte UP_ARROW = 0;
const byte DOWN_ARROW = 1;
const byte LEFT_ARROW = 2;
const byte RIGHT_ARROW = 3;
const byte HOR_LINE = 4;
const byte VER_LINE = 5;

unsigned long previousMillis = 0;
unsigned long modeSelectMillis = 0;
const int returnTimeout = 5000;

int cycle = 1;
int mode = 4;
bool selectingMode = false;

const int address = 0;
unsigned long previousMillisEEPROM = 0;
int workTime = 0;
unsigned long intervalEEPROM = 60000;

void setupLCD() {
  lcd.init();
  lcd.backlight();

  lcd.createChar(UP_ARROW, upArrow);
  lcd.createChar(DOWN_ARROW, downArrow);
  lcd.createChar(RIGHT_ARROW, rightArrow);
  lcd.createChar(LEFT_ARROW, leftArrow);
  lcd.createChar(HOR_LINE, horLine);
  lcd.createChar(VER_LINE, verLine);
  lcd.clear();

  // RESET EEPROM
  // for (int i = 0 ; i < EEPROM.length() ; i++) { //Resets the EEPROM
  //   EEPROM.write(i, 0);
  // }

  workTime = readWorkTime();
}


int checkMenu() {
  unsigned long currentMillis = millis();

  cyclebtnState = readExpanderPin(SW1);    //MENU
  confirmbtnState = readExpanderPin(SW2);  //ENTER
  //Serial.println(confirmbtnState);

  if (cyclebtnState != cyclelastbtnState) {
    if (cyclebtnState == LOW) {
      selectingMode = true;
      lcd.clear();
      selectMode();
      modeSelectMillis = currentMillis;
    }
    cyclelastbtnState = cyclebtnState;
  }

  if (selectingMode && (confirmbtnState != confirmlastbtnState)) {
    if (confirmbtnState == LOW) {
      mode = cycle;
      selectingMode = false;
      lcd.clear();
      currentMode(mode);
    }
    confirmlastbtnState = confirmbtnState;
  }

  if (selectingMode && (currentMillis - modeSelectMillis >= returnTimeout)) {
    selectingMode = false;
    lcd.clear();
    currentMode(mode);
  }

  if (!selectingMode) {
    currentMode(mode);
  }

  if (currentMillis - previousMillisEEPROM >= intervalEEPROM) {
    previousMillisEEPROM = currentMillis;
    workTime++;

    if (workTime % 5 == 0) {
      saveWorkTime(workTime);
    }
  }
  return mode;
}

void selectMode() {
  cycle++;
  if (cycle > 4) { cycle = 1; }
  if (cycle == 1) {
    lcd.setCursor(0, 0);
    lcd.print("*Remote control");
  } else if (cycle == 2) {
    lcd.setCursor(0, 0);
    lcd.print("*Autonomous mode");
  } else if (cycle == 3) {
    lcd.setCursor(0, 0);
    lcd.print("*Slave mode");
  } else if (cycle == 4) {
    lcd.setCursor(0, 0);
    lcd.print("*Joystick");
  }
}

void currentMode(int m) {
  int speed = 100;
  int direction = 1;

  if (m == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Remote");
  } else if (m == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Autonomous");
  } else if (m == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Slave");
  } else if (m == 4) {
    lcd.setCursor(0, 0);
    lcd.print("Joystick");
  }

  lcd.setCursor(0, 1);
  lcd.print(String(workTime) + " min " + String(speed) + "% ");
  if (direction == 1) {  //straight
    lcd.setCursor(14, 0);
    lcd.write(UP_ARROW);
    lcd.setCursor(14, 1);
    lcd.write(VER_LINE);
  } else if (direction == 2) {  // back
    lcd.setCursor(14, 1);
    lcd.write(DOWN_ARROW);
    lcd.setCursor(14, 0);
    lcd.write(VER_LINE);
  } else if (direction == 3) {  // left
    lcd.setCursor(15, 1);
    lcd.write(HOR_LINE);
    lcd.setCursor(14, 1);
    lcd.write(LEFT_ARROW);
  } else {  // right
    lcd.setCursor(13, 1);
    lcd.write(HOR_LINE);
    lcd.setCursor(14, 1);
    lcd.write(RIGHT_ARROW);
  }
}

void saveWorkTime(int time) {
  EEPROM.put(address, time);
}

int readWorkTime() {
  int savedTime = 0;
  EEPROM.get(address, savedTime);
  return savedTime;
}
