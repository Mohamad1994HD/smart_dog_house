#ifndef CONF
#define CONF

#include <EEPROM.h>

class Config{
private:
  uint8_t address;
  uint8_t buffer;

public:
  Config(uint8_t addr);
  void load();
  uint8_t read();
  void update(uint8_t);
  void commit();
  uint8_t getAddress();
};

#endif
