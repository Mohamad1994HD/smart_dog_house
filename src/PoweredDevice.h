#ifndef PDV
#define PDV

#include "Arduino.h"

class Device{
private:
  int pin;
  bool old_status;

public:
  Device(int p);
  void set_pin(int p);
  void init();

  void on();
  void off();

  bool status = false;
  void commit();
};

enum Mode{SUMMER, WINTER};
class Fan:public Device{
public:
  Fan(int);
  Mode mode;
};
#endif
