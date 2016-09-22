//#include "Connector.h"
#include "Arduino.h"
#include "SerialExtractor.h"
#include "Config.h"
#include "PoweredDevice.h"

SerialExtractor ser;
bool connection = false;

//Configurations
Config TEMP_THRES_HOLD(0);
Config LIGHT_THRES_HOLD(1);
Config FOOD_TIME_SPACE(2);
Config FAN_STATUS(3);

//devices
Device Fan(8);
Device Heater(9);
Device Led_strip(7);

void on_command(int device_num, uint8_t command){
    switch(device_num){
      case 1:
      // Fan
      Fan.status = (bool)command;
      break;
      case 2:
      // Heater
      Heater.status = (bool)command;
      break;
      case 3:
      // Lights
      Led_strip.status = (bool)command;
      break;
      case 4:
      // Water
      break;
      case 5:
      // Food
      break;
    }
}

void on_config(int a[]){
  // update the keys with vals
  TEMP_THRES_HOLD.update(a[1]);
  LIGHT_THRES_HOLD.update(a[2]);
  FOOD_TIME_SPACE.update(a[3]);
  FAN_STATUS.update(a[4]);
}

void sendConf(){
  Serial.print("TEMP:");
  Serial.println(TEMP_THRES_HOLD.read());
  Serial.print("LIGHT:");
  Serial.println(LIGHT_THRES_HOLD.read());
  Serial.print("FOOD:");
  Serial.println(FOOD_TIME_SPACE.read());
  Serial.print("FAN:");
  Serial.println(FAN_STATUS.read());
}

void on_connect(){
  // send Configurations
  sendConf();
}


void on_disconnect(){
  //commit changes to config
  TEMP_THRES_HOLD.commit();
  FAN_STATUS.commit();
  FOOD_TIME_SPACE.commit();
  LIGHT_THRES_HOLD.commit();

  Serial.println("Connection Destroyed");
  //reset the MCU
  delay(1000);
  setup();
}


void on_msg_received(int a[], int sz)
{
  if (a[0] == 100 && !connection){
  // connection established
  Serial.println("Connected");
  connection = true;
  on_connect();
  }
  else if (a[0] == 900 && connection){
  // connection destroyed
  Serial.println("Disconnected");
  connection = false;
  on_disconnect();
  }

  if(connection){
    // check msg status command/config
    if(a[0]==200){
      // command
      Serial.println("Command");
      // dispatch the command write/read
      on_command(a[1], a[2]);
    }
    else if(a[0]==300){
      // config
      Serial.println("Config");
      // update configuration list
      on_config(a);
    }
  }
}



void setup() {
  // // Load Configuration
  TEMP_THRES_HOLD.load();
  LIGHT_THRES_HOLD.load();
  FOOD_TIME_SPACE.load();
  FAN_STATUS.load();

  connection = false;

  // init devices
  Fan.init();
  Heater.init();
  Led_strip.init();

  ser.SetCallBack(on_msg_received);
  ser.SetDelimeter(":");
  ser.SetEndIndicator('#');
  Serial.begin(9600);
}

long t=0;
long BLINK_TIME = 1000;


void loop() {
  // put your main code here, to run repeatedly:
  ser.Run();
  if (connection){
    if(millis() - t >= BLINK_TIME){
        // if ledstatus = !ledstatus
        Led_strip.status = !Led_strip.status;
        t = millis();
    }
  }else{
    //check fan config

    // if light < thresshold

    // if dog infront of water sensor

    // if food time

    // if dog inside
      // check fan config
      // if temperature < thresshold
        // check fan config

  }

  Fan.commit();
  Heater.commit();
  Led_strip.commit();
}
