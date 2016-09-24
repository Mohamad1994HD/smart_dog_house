#ifndef SENS
#define SENS

#include "Arduino.h"
#include "DHT.h"

typedef void (*callback)(void);

enum trigger_status {EQUAL, GREATER, SMALLER, GREATER_OR_EQUAL, SMALLER_OR_EQUAL};

class Sensor{
private:
  uint8_t pin;
  int old_val;
  int trigger_val;
  int time_interval;
  unsigned long last_check;

  trigger_status status_trigger;
  callback on_trigger;
  callback on_not_triggered;
public:
  Sensor(uint8_t pin);
  void init();
  virtual void run();

  void set_time_interval(int);
  void set_trigger_val(int);
  void set_on_trigger(callback);
  void set_on_not_triggered(callback);
  void set_trigger_status(trigger_status);
  virtual int get_val();
};


class TempSensor:public Sensor{
private:
  DHT &dht;
  float temp;

public:
  TempSensor(uint8_t p, DHT &);
  void init();
  int get_val();

};
#endif
