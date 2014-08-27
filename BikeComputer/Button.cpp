#include <Arduino.h>
#include "Button.h"
Button::Button(int pin, int latency, bool pullup ) {
  _pin = pin;
  _latency = latency;
  //_buttonState=false;
  _lastPress = 0;
  pinMode(_pin, INPUT);
  if (pullup == true) {
    digitalWrite(_pin, HIGH);
  }
}
boolean Button::check() {
  if (digitalRead(_pin) == LOW) {
    long p = millis();
    if ( p - _lastPress > _latency) {
      _lastPress = p;

      return true;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}
void Button::setLatency(int latency) {
  _latency = latency;
}











