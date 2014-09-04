#ifndef Speed_h

#define Speed_h

class Speed {
  private:
    long tripCount,totalCount;
       
    long timewheel[SPEED_WHEELDETECTOR_CACHESIZE];
    long timecranckset[SPEED_CRANCKSETDETECTOR_CACHESIZE];
    int pointwheel, pointcranckset;
int wheelcircu;

    unsigned int speed;


  public:
    Speed();

    void triggerWheel();
    void triggerCranckset();
    int getCrancksetRpm();
    int getWheelRpm();
    float getSpeedKph();
    long getTripDistance();

};


#endif
