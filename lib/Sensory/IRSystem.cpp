#include "IRSystem.h"

IRSystem::IRSystem(Sensor &s1,Sensor &s2,Sensor &s3): sens1(s1), sens2(s2), sens3(s3){}

void IRSystem::init(){
  sens1.init();
  sens2.init();
  sens3.init();
}

void IRSystem::run(){
    sens1.run();
    sens2.run();
    sens3.run();

    unsigned int val1 = sens1.read();
    unsigned int val2 = sens2.read();
    unsigned int val3 = sens3.read();

    auto check_existence = [](int v1,int v2,
                         int v3, int th) -> bool{
      if ((v1 < th && v2 < th) ||
          (v1 < th && v3 < th) ||
          (v2 < th && v3 < th)){
            return true;
          }
      return false;
    };

    if (check_existence(val1, val2, val3, trigger_val)){
      if(on_detection){on_detection();}
    }else{
      if(on_not_detection){on_not_detection();}
    }
}

void IRSystem::set_callback(callback cback){
  on_detection = cback;
}

void IRSystem::set_not_callback(callback cback){
  on_not_detection = cback;
}

uint8_t IRSystem::get_sensors_vals(uint8_t p){
  if (p == 1){
    return sens1.read();
  }
  if (p == 2){
    return sens2.read();
  }
  if (p == 3){
    return sens3.read();
  }
}

/*
 maher and mohammad 4/9/2016
void IRSystem::run(){
  Serial.print(analogRead(pin1));
  Serial.print(" ");
  Serial.print(analogRead(pin2));
  Serial.print(" ");
  Serial.println(analogRead(pin3));
}
*/
