#include <Arduino.h>

#include "config.h"
#include "Speed.h"

Speed::Speed() {
  pointwheel = 0;
  pointcranckset = 0;
  wheelcircu = 1620;

}

void Speed::triggerWheel() {
  timewheel[pointwheel] = millis();
  pointwheel = (pointwheel + 1) % SPEED_WHEELDETECTOR_CACHESIZE;
}
void Speed::triggerCranckset() {

  timecranckset[pointcranckset] = millis();
  pointcranckset = (pointcranckset + 1) % SPEED_CRANCKSETDETECTOR_CACHESIZE;

}

int Speed::getCrancksetRpm() {
  long now = millis();
  if (now - timecranckset[(pointcranckset + SPEED_CRANCKSETDETECTOR_CACHESIZE - 1) % SPEED_CRANCKSETDETECTOR_CACHESIZE] > SPEED_CRANCKSET_TIMEOUT) {
    return 0;
  } else if (now > SPEED_CRANCKSET_TIMEOUT) {
    long time =  now - timecranckset[pointcranckset];
    double mspr = time / SPEED_CRANCKSETDETECTOR_CACHESIZE;
    double spr = mspr / 1000;
    int rpm = 60 / spr;
    return rpm;
  } else {
    return 0;
  }
}
int Speed::getWheelRpm() {
  long now = millis();
  if (now - timewheel[(pointwheel + SPEED_WHEELDETECTOR_CACHESIZE - 1) % SPEED_WHEELDETECTOR_CACHESIZE] > SPEED_WHEEL_TIMEOUT) {
    return 0;
  } else if (now > SPEED_WHEEL_TIMEOUT) {
    long time =  now - timewheel[pointwheel];
    double mspr = time / SPEED_WHEELDETECTOR_CACHESIZE;
    double spr = mspr / 1000;
    int rpm = 60 / spr;
    return rpm;
  } else {
    return 0;
  }
}
float Speed::getSpeedKph() {
  Serial.println("-------------------");
  float r = wheelcircu * SPEED_KPH;
  float speedKph = r * getWheelRpm()/1000;
  Serial.println(r);
  Serial.println(speedKph);
  Serial.println("-------------------");
  return speedKph;


}


int Speed::getSpeed() {
  return 30;


}
