#ifndef Shifter_h

#define Shifter_h
class Shifter{
private:
  int _pin[2];
  int _deadzone[2];
  int gearServo[2][16];
  int gearCount[2];
  int gearTooth[2][16];
  Servo servo[2] ;
  int _servoPosition[2];
  int _gear[2];
  long shifttime;
  bool needcorrection;

public:
  Shifter();
  void setServo(int id,int pin,int deadzone);
  void setGearServo(int id,int gearNo,int servoFreq);
  void setGearCount(int id,int count);
  void setGeatTooth(int id,int gearNo,int tooth);
  int getGearCount(int id);
  int getGearServo(int id,int gearNo);
  int getGeatTooth(int id,int gearNo);
  int getServoPosition(int id);
  int tuneUp(int id);
  int tuneDown(int id);
  void shiftUp(int id);
  void shiftDown(int id);
  void shiftTo(int id,int gearNo);
  int getGear(int id);
  void correct();
};
#endif




