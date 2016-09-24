#include "28BYJ48.h"

Motor::Motor(uint8_t p1, uint8_t p2,
             uint8_t p3, uint8_t p4):pin1(p1),pin2(p2),pin3(p3),pin4(p4){}

void Motor::init(){
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  status = STOP;
  goal_set = false;
}

void Motor::set_time_between_pulses(int t){
  if (t < 0){
    t*=-1;
  }
  time_between_pulses = t;
}


void Motor::set_rotation_status(rotation_status st){
  if (!goal_set){
    status = st;
  }
}

//blocking
void Motor::step(int num_of_steps){
  if (status == STOP){
    return;
  }
  // check provided direction
  if(status == CW){
    while(true){
      for (int i=7; i>=0; i--){
        out(i);
        delayMicroseconds(time_between_pulses);
      }
      steps++;
      if (steps >= num_of_steps){
        steps=0;
        status = STOP;
        break;
      }
    }
  }else{
    while(true){
    for (int i=0; i<8; i++){
      out(i);
      delayMicroseconds(time_between_pulses);
      }
    steps++;
    if (steps >= num_of_steps){
      steps=0;
      status = STOP;
      break;
      }
    }
  }
}

void Motor::rotate(int degrees){
  if (status == STOP){
    return;
  }
  // convert degrees to steps
  int num_of_steps=map(degrees, 0, 360, 0, 512);
  // check provided direction
  if(status == CW){
    while(true){
      for (int i=7; i>=0; i--){
        out(i);
        delayMicroseconds(time_between_pulses);
      }
      steps++;
      if (steps >= num_of_steps){
        steps=0;
        status = STOP;
        break;
      }
    }
  }else{
    while(true){
    for (int i=0; i<8; i++){
      out(i);
      delayMicroseconds(time_between_pulses);
      }
    steps++;
    if (steps >= num_of_steps){
      steps=0;
      status = STOP;
      break;
      }
    }
  }
}

//Non-blocking
void Motor::rotate_by(int degrees){
  if(!goal_set){
    goal_set = true;
    to_angle = degrees;
    to_steps = map(to_angle, 0, 360, 0, 512);
  }
}

void Motor::rotate_by_steps(int steps){
    if(!goal_set){
      goal_set = true;
      to_steps = steps;
    }
}

void Motor::commit(){
  if(!goal_set){
    return ;
  }
  if(status == STOP){
    return ;
  }

  if (micros() - t >= time_between_pulses){
    if (status == CW){
      for(int i=7; i>=0; i--){
        out(i);
      }
    }else{
      for(int i=0; i<8; i++){
        out(i);
      }
    }
    steps++;
    if(steps == to_steps){
      status = STOP;
      steps = 0;
      return;
    }
  }
}

//
void Motor::out(int digit){
  digitalWrite(pin1, bitRead(lookup[digit], 0));
  digitalWrite(pin2, bitRead(lookup[digit], 1));
  digitalWrite(pin3, bitRead(lookup[digit], 2));
  digitalWrite(pin4, bitRead(lookup[digit], 3));
}
