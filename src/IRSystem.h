#ifndef IRSYS
#define IRSYS

#include "Arduino.h"


typedef void (*callback) (void);

class IRSystem{
public:
  IRSystem(int p1,int p2, int p3);
  unsigned int time_interval;
  unsigned int trigger_val;

  void init();
  void run();

  void set_callback(callback);

  uint8_t get_sensors_vals(uint8_t);
  void set_threshold(int);
private:
  int pin1,pin2,pin3;
  callback on_detection=NULL;
};

#endif
