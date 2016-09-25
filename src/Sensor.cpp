#include "Sensor.h"

Sensor::Sensor(uint8_t p):pin(p){}

void Sensor::init(){
  pinMode(pin, INPUT);
}

void Sensor::set_trigger_val(int v){
  trigger_val = v;
}

void Sensor::set_on_trigger(callback cback){
  on_trigger = cback;
}

void Sensor::set_trigger_status(trigger_status ts){
  status_trigger = ts;
}

void Sensor::set_time_interval(int t){
  time_interval = t;
}

void Sensor::run(){
  if(millis() - last_check < time_interval){
    return;
  }
  last_check = millis();

  int val = get_val();

  if (val == old_val){
    return;
  }

  old_val = val;

  //bool dispached = false;

  switch (status_trigger) {
    case EQUAL:
      if (val == trigger_val){
        on_trigger();// dispached = true;
      }
    break;
    case GREATER_OR_EQUAL:
      if (val >= trigger_val){
        on_trigger();// dispached = true;
      }
    break;
    case SMALLER_OR_EQUAL:
      if (val <= trigger_val){
        on_trigger();// dispached = true;
      }
    break;
    case GREATER:
      if (val > trigger_val){
        on_trigger(); //dispached = true;
      }
    break;
    case SMALLER:
      if (val < trigger_val){
        on_trigger();// dispached = true;
      }
    break;
    default:
    break;
  }
  //if (!dispached){
//    on_not_triggered();
//  }
}

void Sensor::set_on_not_triggered(callback cback){
  on_not_triggered = cback;
}

int Sensor::get_val(){
  return analogRead(pin);
}

int Sensor::read(){
  return old_val;
}
//
TempSensor::TempSensor(uint8_t p,DHT &d):Sensor(p),dht(d){}

void TempSensor::init(){
//  Sensor::init();
  dht.begin();
}

int TempSensor::get_val(){
  float c = dht.readTemperature();
  if (isnan(c)){
    return 999;
  }
  return (int)c;
}
