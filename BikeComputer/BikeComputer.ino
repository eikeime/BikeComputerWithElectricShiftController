#include <Wire.h>
#include <MicroLCD.h>
#include <Servo.h>
#include <EEPROM.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "config.h"
#include "Speed.h"
#include "Button.h"
#include "Shifter.h"


//button
Button buttonUp(BUTTON_UP, ANTISHAKE_TIMELATENCY, true);
Button buttonDown(BUTTON_DOWN, ANTISHAKE_TIMELATENCY, true);
Button buttonLeft(BUTTON_LEFT, ANTISHAKE_TIMELATENCY, true);
Button buttonRight(BUTTON_RIGHT, ANTISHAKE_TIMELATENCY, true);
Button sensorWheel(SENSOR_WHEEL_PIN, SPEED_WHEEL_ANTISHAKE_TIMELATENCY, false);
Button sensorCranckset(SENSOR_CRANCKSET_PIN, SPEED_CRANCKSET_ANTISHAKE_TIMELATENCY, false);
//shifter
Shifter shifter;
int frontPlan[12] = {
  0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3
};
int rearPlan[12] = {
  0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 7
};
int gearno = 3;
//speed
Speed spd;
MPU6050 accmeter;
int16_t ax, ay, az;
int16_t gx, gy, gz;
//display
LCD_SH1106 lcd; /* for SH1106 OLED module */
int pageLevel = 0;
String cursorChar = ">";
String cursorEmpty = " ";
boolean displayXenabled = true;
int pageMax = 5;
int Page[] = {
  0, 0, 0, 0, 0,
};
String menuMain[] = {
  "Menu", "FrontCount    ", "RearCount     ", "FrontSetup    ", "RearSetup     ", "ResetSetup     "
};
int menuCountMain = 5;
int speedrefresh = SPEED_REFRESH;
void setup() {
  Serial.begin(9600);
  lcd.begin();
  accmeter.initialize();
  shifter.setServo(0, SERVO_FRONT, 50);
  shifter.setServo(1, SERVO_REAR, 10);
  //resetEEPROM(EEPROM_SHIFTER_START, EEPROM_SHIFTER_START + 70);
  attachInterrupt(SENSOR_WHEEL_INT, speedInterrupt, FALLING);
  attachInterrupt(SENSOR_CRANCKSET_INT, crancksetInterrupt, FALLING);
  //init gear react
  Page[0] = 3;
}
void loop() {
  accmeter.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("a/g:\t");
  Serial.print(ax / 16384); Serial.print("\t");
  Serial.print(ay / 16384); Serial.print("\t");
  Serial.print(az / 16384); Serial.print("\t");
  Serial.print(gx / 131); Serial.print("\t");
  Serial.print(gy / 131); Serial.print("\t");
  Serial.println(gz / 131);

  Serial.println(spd.getCrancksetRpm());
  // show  info
  if (pageLevel == 0) {
    displayInfo();
  }
  //                                                left
  if (buttonLeft.check()) {
    if (pageLevel > 0) {
      pageLevel--;
    }
    displayMoveX();
    Serial.println("-----------------------------------");
    Serial.println("left");
    Serial.print("page X:");
    Serial.println(pageLevel);
    Serial.print("page Y:");
    Serial.println(Page[pageLevel]);
    Serial.print("page MAX:");
    Serial.println(pageMax);
    Serial.println("-----------------------------------");
  }
  //                                                right
  if (buttonRight.check()) {
    if (displayXenabled) {
      pageLevel++;
    }
    Page[pageLevel] = 0;
    displayMoveX();
    Serial.println("-----------------------------------");
    Serial.println("right");
    Serial.print("page X:");
    Serial.println(pageLevel);
    Serial.print("page Y:");
    Serial.println(Page[pageLevel]);
    Serial.print("page MAX:");
    Serial.println(pageMax);
    Serial.println("-----------------------------------");
  }
  //                                                up
  if (buttonUp.check()) {
    if (Page[pageLevel] > 0) {
      Page[pageLevel]--;
    }
    displayMoveY();
    Serial.println("-----------------------------------");
    Serial.println("up");
    Serial.print("page X:");
    Serial.println(pageLevel);
    Serial.print("page Y:");
    Serial.println(Page[pageLevel]);
    Serial.print("page MAX:");
    Serial.println(pageMax);
    Serial.println("-----------------------------------");
  }
  //                                                down
  if (buttonDown.check()) {
    if (Page[pageLevel] < pageMax - 1) {
      Page[pageLevel]++;
    }
    displayMoveY();
    Serial.println("-----------------------------------");
    Serial.println("down");
    Serial.print("page X:");
    Serial.println(pageLevel);
    Serial.print("page Y:");
    Serial.println(Page[pageLevel]);
    Serial.print("page MAX:");
    Serial.println(pageMax);
    Serial.println("-----------------------------------");
  }
  shifter.correct();
  delay(LOOP_DELAY);
}

//                                                X move
void displayMoveX() {
  lcd.clear();
  switch (pageLevel) {
    case 0://                  info
      pageMax = 5;
      speedrefresh = SPEED_REFRESH;
      break;
    case 1://                                               menu
      displayMenu(menuMain, menuCountMain);
      pageMax = menuCountMain;
      break;
    case 2://
      switch (Page[pageLevel - 1]) {
        case 0://                                               gear count L2
        case 1:
          if (Page[pageLevel - 1] == 0) {
            pageMax = 7;
          }
          else {
            pageMax = 17;
          }
          lcd.setCursor(8, 0);
          lcd.print(menuMain[Page[pageLevel - 1]]);
          lcd.setCursor(30, 4);
          Page[pageLevel] = shifter.getGearCount(Page[pageLevel - 1]);
          lcd.printInt(Page[pageLevel], 2);
          break;
        case 2://                                               gear setup menu L2
        case 3: {
            lcd.setCursor(0, 0);
            lcd.setFontSize(FONT_SIZE_SMALL);
            lcd.print(menuMain[Page[pageLevel - 1] + 1]);
            int _gearcount = shifter.getGearCount(Page[pageLevel - 1] - 2);
            pageMax = _gearcount;
            if (_gearcount > 0) {
              displayNumList(_gearcount);
            }
            else {
              lcd.setCursor(0, 0);
              lcd.setFontSize(FONT_SIZE_LARGE);
              lcd.print("Setup GearCount first");
            }
          }
          break;
        case 4:// reset setup
          lcd.setCursor(0, 1);
          lcd.setFontSize(FONT_SIZE_LARGE);
          lcd.print("R U SURE!!!");

          break;

      }
      break;
    case 3://
      switch (Page[pageLevel - 2]) {
        case 0://                                               save gear count L3
        case 1:
          shifter.setGearCount(Page[pageLevel - 2], Page[pageLevel - 1]);
          pageLevel = 1;
          displayMenu(menuMain, menuCountMain);
          pageMax = menuCountMain;
          break;
        case 2://                                               gear setup uiL3
        case 3: {
            pageMax = 5000;
            lcd.setCursor(8, 0);
            if (Page[pageLevel - 2] == 2) {
              lcd.print("Front");
            }
            else {
              lcd.print("Rear");
            }
            lcd.print(Page[pageLevel - 1]);
            lcd.setCursor(15, 4);
            shifter.shiftTo(Page[pageLevel - 2] - 2, Page[pageLevel - 1]);
            Page[pageLevel] = 3;
            int _freq = shifter.getGearServo(Page[pageLevel - 2] - 2, Page[pageLevel - 1]);
            lcd.printInt(_freq, 4);
          }
          break;

        case 4:
          lcd.setCursor(0, 0);
          lcd.setFontSize(FONT_SIZE_LARGE);
          lcd.print("hahahaha");
          presetEEPROM();
          //resetEEPROM(EEPROM_SHIFTER_START, EEPROM_SHIFTER_START + 70);
          break;


      }
      break;
    case 4:
      switch (Page[pageLevel - 3]) {
        case 2:
        case 3: { //                                               save gear setup L4
            lcd.setCursor(0, 0);
            shifter.setGearServo(Page[pageLevel - 3] - 2, Page[pageLevel - 2] , shifter.getServoPosition(Page[pageLevel - 3] - 2) );


            lcd.setFontSize(FONT_SIZE_SMALL);
            lcd.print(menuMain[Page[pageLevel - 3] + 1]);
            int _gearcount = shifter.getGearCount(Page[pageLevel - 3] - 2);
            pageMax = _gearcount;
            Page[2] = Page[pageLevel - 2];
            pageLevel = 2;
            if (_gearcount > 0) {
              displayNumList(_gearcount);
            }
            else {
              lcd.setCursor(0, 0);
              lcd.setFontSize(FONT_SIZE_LARGE);
              lcd.print("Setup GearCount first");
            }
          }
          break;


      }


      break;
    default:
      break;
  }
}

//                                                Y move

void displayMoveY() {
  switch (pageLevel) {
    case 0:
      if (Page[pageLevel] == 2) {
        Serial.println("up");
        if (gearno < 10) {
          gearno++;
          shifter.shiftTo(0, frontPlan[gearno]);
          shifter.shiftTo(1, rearPlan[gearno]);

        }
      }
      else if (Page[pageLevel] == 4) {
        Serial.println("down");
        if (gearno > 0) {
          gearno--;
          shifter.shiftTo(0, frontPlan[gearno]);
          shifter.shiftTo(1, rearPlan[gearno]);

        }
      }
      Page[pageLevel] = 3;
      break;
    case 1://                                               menu
      displayMenu(menuMain, menuCountMain);
      break;
    case 2://
      switch (Page[pageLevel - 1]) {
        case 0://                                               gear count setup L2
        case 1:
          lcd.setCursor(30, 4);
          lcd.printInt(Page[pageLevel], 2);
          break;
        case 2://                                               gearsetup menu
        case 3: {
            int _gearcount = shifter.getGearCount(Page[pageLevel - 1] - 2);
            pageMax = _gearcount;
            if (_gearcount > 0) {
              displayNumList(_gearcount);
            }
            else {
              lcd.print("Setup GearCount first");
            }
          }
          break;

      }
      break;
    case 3:
      switch (Page[pageLevel - 2]) {
        case 2:
        case 3:
          //shifter
          lcd.setCursor(15, 4);
          if (Page[pageLevel] == 2) {
            lcd.printInt(shifter.tuneUp(Page[pageLevel - 2] - 2), 4);
          }
          else if (Page[pageLevel] == 4) {
            lcd.printInt(shifter.tuneDown(Page[pageLevel - 2] - 2), 4);
          }
          Page[pageLevel] = 3;
          break;




      }



      break;
  }
}


















void displayNumList(int gearcount) {
  lcd.setFontSize(FONT_SIZE_LARGE);
  int offset = 0;
  if (Page[pageLevel] < 1) {
    offset++;
  }
  else if (Page[pageLevel] + 1 == gearcount) {
    if (gearcount > 2) {

      offset--;
    }
  }
  lcd.setCursor(8, 1);
  lcd.print("");
  lcd.setCursor(8, 3);
  lcd.print("");
  lcd.setCursor(8, 5);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print(Page[pageLevel] + offset);
  lcd.setCursor(8, 3);
  lcd.print(Page[pageLevel] + 1 + offset);
  lcd.setCursor(0, 1);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 3);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 5);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 3 - 2 * offset);
  lcd.print(cursorChar);
  if (gearcount > 2) {
    lcd.setCursor(8, 5);
    lcd.print(Page[pageLevel] + 2 + offset);
  }

}

void displayMenu(String* menu, int menuCount) {
  //lcd.clear();
  //title
  lcd.setCursor(0, 0);
  lcd.setFontSize(FONT_SIZE_SMALL);
  lcd.print(menu[0]);
  //menu
  lcd.setFontSize(FONT_SIZE_LARGE);
  int offset = 0;
  if (Page[pageLevel] < 1) {
    offset++;
  }
  else if (Page[pageLevel] + 1 == menuCount) {
    offset--;
  }
  lcd.setCursor(8, 1);
  lcd.print(menu[Page[pageLevel] + offset]);
  lcd.setCursor(8, 3);
  lcd.print(menu[Page[pageLevel] + 1 + offset]);
  lcd.setCursor(8, 5);
  lcd.print(menu[Page[pageLevel] + 2 + offset]);
  lcd.setCursor(0, 1);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 3);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 5);
  lcd.print(cursorEmpty);
  lcd.setCursor(0, 3 - 2 * offset);
  lcd.print(cursorChar);
}




void displayInfo() {
  speedrefresh += LOOP_DELAY;
  if (speedrefresh > SPEED_REFRESH) {
    //speed

    int s = (int)(spd.getSpeedKph() * 10);
    int ss = s % 10;
    int sl = (s - ss) / 10;
    lcd.setCursor(0, 0);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.print("SPD");
    lcd.setCursor(0, 2);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.print("KPH");
    lcd.setCursor(20, 0);
    lcd.setFontSize(FONT_SIZE_XLARGE);
    lcd.printInt(sl, 2);
    lcd.setCursor(54, 1);
    lcd.print(".");
    lcd.setCursor(62, 0);
    lcd.printInt(ss, 1);
    //peddle
    lcd.setCursor(0, 4);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.print("P D");
    lcd.setCursor(0, 6);
    lcd.setFontSize(FONT_SIZE_SMALL);
    lcd.print("PPM");
    lcd.setCursor(30, 4);
    lcd.setFontSize(FONT_SIZE_XLARGE);
    lcd.printInt(spd.getCrancksetRpm(), 3);
    speedrefresh = 0;
  }
  //gear
  lcd.setCursor(96, 0);
  lcd.setFontSize(FONT_SIZE_SMALL);
  lcd.print("GEAR");
  lcd.setCursor(96, 1);
  lcd.setFontSize(FONT_SIZE_LARGE);
  lcd.printInt(gearno + 1, 2);
}




void resetEEPROM(int startAddr, int endAddr) {
  int i ;
  for (i = startAddr; i < endAddr; i++) {
    EEPROM.write(i, 0);
  }
}


void presetEEPROM() {
  int gear_front[] = {3500, 2040, 850, 50};
  int gear_rear[] = { 1200, 1310, 1420, 1530, 1670, 1790, 1920, 2130};
  EEPROM.write(EEPROM_SHIFTER_START, 4);
  EEPROM.write(EEPROM_SHIFTER_START + 1, 8);
  shifter.setGearCount( 0, 4);
  shifter.setGearCount( 1, 8);
  int i = 0;
  for (i = 0; i < 4; i++) {
    shifter.setGearServo(0, i, gear_front[i]);
  }
  for (i = 0; i < 8; i++) {
    shifter.setGearServo(1,  i,  gear_rear[i]);
  }
}

void speedInterrupt() {
  if (sensorWheel.check()) {
    Serial.println("++++++++++++++++++++++");
    Serial.println("speed");
    spd.triggerWheel();
  }
}
void crancksetInterrupt() {
  if (sensorCranckset.check()) {
    Serial.println("-------------------------");
    Serial.println("cranckset");
    spd.triggerCranckset();
  }
}


