#include "IRSystem.h"

IRSystem::IRSystem(int p1,int p2,int p3): pin1(p1), pin2(p2), pin3(p3){}

void IRSystem::init(){
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  //Serial.begin(9600);
}

void IRSystem::run(){
    uint8_t val1 = analogRead(pin1);
    uint8_t val2 = analogRead(pin2);
    uint8_t val3 = analogRead(pin3);

    auto check_existence = [](uint8_t v1,uint8_t v2,
                         uint8_t v3) -> bool{
      if ((v1 < THRES_HOLD && v2 < THRES_HOLD) ||
          (v1 < THRES_HOLD && v3 < THRES_HOLD) ||
          (v2 < THRES_HOLD && v3 < THRES_HOLD)){
            return true;
          }
      return false;
    };

    if (check_existence(val1, val2, val3)){
      on_detection();
    }
}

void IRSystem::set_callback(callback cback){
  on_detection = cback;
}


/*
void IRSystem::run(){
  Serial.print(analogRead(pin1));
  Serial.print(" ");
  Serial.print(analogRead(pin2));
  Serial.print(" ");
  Serial.println(analogRead(pin3));
}
*/
