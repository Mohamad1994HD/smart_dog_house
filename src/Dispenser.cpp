#include "Dispenser.h"

Dispenser::Dispenser(uint8_t p1, uint8_t p2,
                     uint8_t p3, uint8_t p4, uint8_t vib_pin):Motor(p1,p2,p3,p4),
                                                              vibrator_pin(vib_pin)
                                                              {}
void Dispenser::init(){
  Motor::init();
  Motor::set_time_between_pulses(800);

  pinMode(vibrator_pin, OUTPUT);
  digitalWrite(vibrator_pin, LOW);
  status = OFF;
}

void Dispenser::commit(){
  if (status == OFF){
    digitalWrite(vibrator_pin, OFF);
    return;
  }
  Motor::set_rotation_status(CW);
  Motor::step(256);

  if (status == DISPENSE_FOOD){
    digitalWrite(vibrator_pin, HIGH);
  }else{
    // dispenser water
  }
  delay(dispense_time);

  Motor::set_rotation_status(CCW);
  Motor::step(256);
  status=OFF;
}

void Dispenser::set_dispense_time(int t){
  dispense_time = t;
}

void Dispenser::set_dispenser_status(dispenser_status st){
  status = st;
}
