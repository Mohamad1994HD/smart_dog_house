#include "IRSystem.h"

IRSystem::IRSystem(int p1,int p2,int p3): pin1(p1), pin2(p2), pin3(p3){}

void IRSystem::init(){
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  //Serial.begin(9600);
}

void IRSystem::run(){
    unsigned int val1 = analogRead(pin1);
    unsigned int val2 = analogRead(pin2);
    unsigned int val3 = analogRead(pin3);

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
      on_detection();
    }
}

void IRSystem::set_callback(callback cback){
  on_detection = cback;
}

uint8_t IRSystem::get_sensors_vals(uint8_t p){
  if (p == 1){
    return analogRead(pin1);
  }
  if (p == 2){
    return analogRead(pin2);
  }
  if (p == 3){
    return analogRead(pin3);
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
