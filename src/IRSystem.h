#ifndef IRSYS
#define IRSYS

#include "Arduino.h"


typedef void (*callback) (void);

class IRSystem{
public:
  IRSystem(int p1,int p2, int p3);
  void init();
  void run();

  void set_callback(callback);
  uint8_t get_sensors_vals(uint8_t);
  void set_time_interval(int);
  void set_threshold(int);
private:
  int pin1,pin2,pin3;
  unsigned int time_interval;
  unsigned int THRES_HOLD;
  unsigned long t;
  callback on_detection=NULL;
};

#endif
