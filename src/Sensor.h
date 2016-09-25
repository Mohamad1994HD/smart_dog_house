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
  trigger_status status_trigger;
  callback on_trigger;
  callback on_not_triggered;
public:
  Sensor(uint8_t pin);
  unsigned int time_interval;
  int trigger_val;

  void init();
  virtual void run();
  void set_on_trigger(callback);
  void set_on_not_triggered(callback);
  void set_trigger_status(trigger_status);
  int read();
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
