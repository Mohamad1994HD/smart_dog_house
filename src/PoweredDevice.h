#ifndef PDV
#define PDV

#include "Arduino.h"

class Device{
private:
  int pin;
  bool old_status;

public:
  Device(int p);
  void init();

  void on();
  void off();

  bool status = false;
  void commit();
};

#endif
