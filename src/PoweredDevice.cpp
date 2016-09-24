#include "PoweredDevice.h"

Device::Device(int p):pin(p){}

void Device::init(){
  pinMode(pin, OUTPUT);
  status = false;
  old_status = false;
}

void Device::set_pin(int p){
  pin = p;
}

void Device::on(){
  digitalWrite(pin, HIGH);
}

void Device::off(){
  digitalWrite(pin, LOW);
}

void Device::commit(){
  if (status == false && old_status == false){
    off();
    return;
  }
  if (status != old_status){
    status==true?on():off();
    old_status = status;
  }
}
