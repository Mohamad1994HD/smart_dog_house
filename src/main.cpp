#include "IRSystem.h"

int i=0;

void on_dog_detected(){
  Serial.println(i++);
}


IRSystem irsys(A0, A1, A2);

void setup(){
  Serial.begin(9600);

  irsys.set_callback(on_dog_detected);
  irsys.init();
}

void loop(){
  irsys.run();
  delay(200);
}
