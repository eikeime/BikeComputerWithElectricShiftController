#ifndef Button_h

#define Button_h


class Button{
private: 
  int _pin;
  int _latency;
protected:
  long _lastPress;
public: 
  Button(int pin, int latency,bool pullup);
  boolean check();
  void setLatency(int latency);

};
#endif
