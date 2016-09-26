#ifndef DISP
#define DISP

#include "28BYJ48.h"
#include "PoweredDevice.h"

enum dispenser_status {OFF, DISPENSE_WATER, DISPENSE_FOOD};

class Dispenser:public Motor{
private:
    uint8_t vibrator_pin;
    dispenser_status status;
    int dispense_time;
public:
  Dispenser(uint8_t p1, uint8_t p2,
            uint8_t p3, uint8_t p4, uint8_t vib_pin);
  void set_dispense_time(int);
  void set_dispenser_status(dispenser_status);
  void init();
  void commit();

};

#endif
