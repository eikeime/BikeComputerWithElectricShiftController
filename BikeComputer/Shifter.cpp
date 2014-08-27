#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>

#include "config.h"
#include "Shifter.h"



Shifter::Shifter() { // init  data

  gearCount[0] = EEPROM.read(EEPROM_SHIFTER_START);
  gearCount[1] = EEPROM.read(EEPROM_SHIFTER_START + 1);
  _deadzone[0] = EEPROM.read(EEPROM_SHIFTER_START + 2);
  _deadzone[1] = EEPROM.read(EEPROM_SHIFTER_START + 3);
  _gear[0] = EEPROM.read(EEPROM_SHIFTER_START + 4);
  _gear[1] = EEPROM.read(EEPROM_SHIFTER_START + 5);

  int i = 0;
  for (i = 10; i < 22; i += 2) {
    gearServo[0][(i - 10) / 2] = EEPROM.read(EEPROM_SHIFTER_START + i) * 100 + EEPROM.read(EEPROM_SHIFTER_START + i + 1);
    //gearTooth[0][i-4] = EEPROM.read(EEPROM_SHIFTER_START+i+3);
  }
  for (i = 22; i < 54; i += 2) {
    gearServo[1][(i - 22) / 2] = EEPROM.read(EEPROM_SHIFTER_START + i) * 100 + EEPROM.read(EEPROM_SHIFTER_START + i + 1);
    //gearTooth[1][i-10] = EEPROM.read(EEPROM_SHIFTER_START+i+16);
  }
  _servoPosition[0] = getGearServo(0, _gear[0]);
  _servoPosition[1] = getGearServo(1, _gear[1]);
  shifttime = 0;
  needcorrection = false;
}
void Shifter::setServo(int id, int pin, int deadzone) {
  _pin[id] = pin;
  _deadzone[id] = deadzone;
  servo[id].attach(_pin[id]);
  shiftTo(id, _gear[id]);
}


void Shifter::setGearServo(int id, int gearNo, int servoFreq) {
  gearServo[id][gearNo] = servoFreq;
  if (id == 0) {
    int small = servoFreq % 100;
    int large = (servoFreq - small) / 100;
    EEPROM.write(EEPROM_SHIFTER_START + 10 + gearNo * 2, large);
    EEPROM.write(EEPROM_SHIFTER_START + 10 + gearNo * 2 + 1, small);
  }
  else if (id == 1) {
    int small = servoFreq % 100;
    int large = (servoFreq - small) / 100;
    EEPROM.write(EEPROM_SHIFTER_START + 22 + gearNo * 2, large);
    EEPROM.write(EEPROM_SHIFTER_START + 22 + gearNo * 2 + 1, small);
  }


}


void Shifter::setGearCount(int id, int count) {
  gearCount[id] = count;
  EEPROM.write(EEPROM_SHIFTER_START + id, count);
}

//void Shifter::setGeatTooth(int id,int gearNo,int tooth){
//  gearTooth[id][gearNo] = tooth;
//  if(id==0){
//
//    EEPROM.write(EEPROM_SHIFTER_START+7+gearNo,tooth);
//  }
//  else if(id == 1){
//
//    EEPROM.write(EEPROM_SHIFTER_START+26+gearNo,tooth);
//  }
//}


int Shifter::getGearCount(int id) {
  return gearCount[id] ;
}


int Shifter::getGearServo(int id, int gearNo) {
  return gearServo[id][gearNo];
}
int Shifter::getGeatTooth(int id, int gearNo) {
  return gearTooth[id][gearNo];
}
int Shifter::getServoPosition(int id) {
  return _servoPosition[id];


}


int Shifter::tuneUp(int id) {

  _servoPosition[id] += _deadzone[id];
  if (_servoPosition[id] > 3500) {

    _servoPosition[id] = 3500;
  }
  servo[id].writeMicroseconds(_servoPosition[id]);
  return _servoPosition[id];
}


int Shifter::tuneDown(int id) {

  _servoPosition[id] -= _deadzone[id];
  if (_servoPosition[id] < 0) {

    _servoPosition[id] = 0;
  }
  servo[id].writeMicroseconds(_servoPosition[id]);
  return _servoPosition[id];
}


void Shifter::shiftTo(int id, int gearNo) {
  shifttime = millis();
  needcorrection = true;
  if (servo[id].attached()) {

    if (id == 1) {
      if (gearNo > _gear[id]) {
        servo[id].writeMicroseconds(gearServo[id][gearNo] + (gearServo[id][gearNo] - gearServo[id][gearNo - 1]) / 7);
      } else if (gearNo < _gear[id]) {
        servo[id].writeMicroseconds(gearServo[id][gearNo] + (gearServo[id][gearNo] - gearServo[id][gearNo + 1]) / 7);
      }
    } else if (id == 0) {
      servo[id].writeMicroseconds(gearServo[id][gearNo]);
    }
    _servoPosition[id] = gearServo[id][gearNo];
    _gear[id] = gearNo;
    EEPROM.write(EEPROM_SHIFTER_START + 4 + id, gearNo);
  }
}

void Shifter::correct() {
  if (needcorrection) {
    if (millis() - SHIFT_CORRECTION_DELAY > shifttime) {
      if (servo[1].attached()) {
        servo[1].writeMicroseconds(gearServo[1][_gear[1]]);
      }
      needcorrection = false;
    }
  }
}



void Shifter::shiftUp(int id) {
  if (_gear[id] < gearCount[id] - 1) {
    shiftTo(id, _gear[id] + 1);
  }
}
void Shifter::shiftDown(int id) {
  if (_gear[id] > 0) {
    shiftTo(id, _gear[id] - 1);
  }
}
int Shifter::getGear(int id) {
  return _gear[id];

}

















