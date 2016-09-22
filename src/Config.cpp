#include "Config.h"

Config::Config(uint8_t addr):address(addr){}

void Config::load(){
  buffer = EEPROM.read(address);
}

uint8_t Config::read(){
  return buffer;
}


void Config::update(uint8_t val){
  if (buffer != val){
    buffer = val;
  }
}

void Config::commit(){
  EEPROM.write(address, buffer);
}

uint8_t Config::getAddress(){
  return address;
}
