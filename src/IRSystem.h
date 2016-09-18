#ifndef IRSYS
#define IRSYS

#include "Arduino.h"

#define THRES_HOLD 150

typedef void (*callback) ();

class IRSystem{
public:
  IRSystem(int p1,int p2, int p3);
  void init();
  void run();

  void set_callback(callback);


private:
  int pin1,pin2,pin3;
  callback on_detection=NULL;
};

#endif
