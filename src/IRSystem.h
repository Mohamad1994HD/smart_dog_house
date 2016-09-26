#ifndef IRSYS
#define IRSYS

#include "Arduino.h"
#include "Sensor.h"

typedef void (*callback) (void);

class IRSystem{
public:
  IRSystem(Sensor &s1, Sensor &s2, Sensor &s3);
  unsigned int time_interval;
  unsigned int trigger_val;

  void init();
  void run();

  void set_callback(callback);
  void set_not_callback(callback);
  uint8_t get_sensors_vals(uint8_t);
  void set_threshold(int);
private:
  Sensor &sens1, &sens2, &sens3;
  callback on_detection;
  callback on_not_detection;
};

#endif
