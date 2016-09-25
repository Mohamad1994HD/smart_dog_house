//#include "Connector.h"
#include "Recources.h"

SoftwareSerial soft_ser(12,13);
SerialExtractor ser(soft_ser);

bool connection = false;

//Configurations
Config TEMP_THRES_HOLD(0);
Config LIGHT_THRES_HOLD(1);
Config FOOD_TIME_SPACE(2);
Config FAN_STATUS(3);

//IR Detection System
IRSystem irsys(A5, A3, A4);

//Devices
Device Fan(3);
Device Heater(4);
Device Led_strip(5);
//Food-water Dispenser unit
Dispenser dispenser(8, 9, 10, 11, 7);

//Sensors
Sensor water_sensor(A0);
Sensor light_sensor(A1);

DHT dht(6, DHT22);
TempSensor Temp_sensor(6, dht);

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
      dispenser.set_dispense_time(1000);
      if (command == 1){
      dispenser.set_dispenser_status(DISPENSE_WATER);
    }else if (command == 2){
      dispenser.set_dispenser_status(DISPENSE_FOOD);
    }
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

void on_request(){
  int light = light_sensor.read();
  int temp = Temp_sensor.read();
  int timer = 4;
  soft_ser.print(temp); soft_ser.print(":"); soft_ser.print(light);
  soft_ser.print(":"); soft_ser.println(timer);
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
    }else if (a[0] == 201){
      Serial.println("Request");
      on_request();
    }
    else if(a[0]==300){
      // config
      Serial.println("Config");
      // update configuration list
      on_config(a);
    }
  }
}

void on_water_need(){
  Serial.println("Dispensing water");
  dispenser.set_dispense_time(2000);
  dispenser.set_dispenser_status(DISPENSE_FOOD);
}

void on_light_dim(){
  Serial.println("Dim light");
  Led_strip.status = true;
}

void on_temp(){
  Serial.println("Temperature Low");
  Serial.println("Turning On Heater..");
  Serial.println(Temp_sensor.get_val());
  Fan.status = true;
  Heater.status = true;
}

void on_light(){
  Serial.println("Light light");
  //Led_strip.status = false;
}

void on_dog_inside(){
  Serial.println("Dog is inside");
  Serial.print(irsys.get_sensors_vals(1)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(2)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(3)); Serial.print(" ");
}

void setup() {
  // // Load Configuration
  TEMP_THRES_HOLD.load();
  LIGHT_THRES_HOLD.load();
  FOOD_TIME_SPACE.load();
  FAN_STATUS.load();

  connection = false;

  //init IRsystem
  irsys.set_callback(on_dog_inside);
  irsys.set_time_interval(1000);
  irsys.set_threshold(150);
  irsys.init();
  // init devices
  Fan.init();
  Heater.init();
  Led_strip.init();

  //init Sensors
  water_sensor.init();
  water_sensor.set_trigger_val(200);
  water_sensor.set_trigger_status(SMALLER);
  water_sensor.set_time_interval(2000);
  water_sensor.set_on_trigger(on_water_need);

  light_sensor.init();
  light_sensor.set_trigger_val(500);
  light_sensor.set_trigger_status(SMALLER);
  light_sensor.set_time_interval(4000);
  light_sensor.set_on_trigger(on_light_dim);
  light_sensor.set_on_not_triggered(on_light);

  Temp_sensor.init();
  Temp_sensor.set_trigger_val(30);
  Temp_sensor.set_time_interval(1000);
  Temp_sensor.set_trigger_status(SMALLER);
  Temp_sensor.set_on_trigger(on_temp);

  dispenser.init();

  ser.SetCallBack(on_msg_received);
  ser.SetDelimeter(":");
  ser.SetEndIndicator('#');
  ser.init(9600);

  Serial.begin(9600);
}

long t=0;
unsigned long BLINK_TIME = 1000;


void loop() {
  // Sensory systems
irsys.run();
water_sensor.run();
light_sensor.run();
Temp_sensor.run();
//  Led_strip.commit();
//
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
    if (light_sensor.get_val() > 500 ){
      Led_strip.status = false;
    }

    //if dog infront of water sensor
    // done via callback

    // if food time

    // if dog inside
      // check fan config
      // if temperature > thresshold
        if (Temp_sensor.get_val() > 30){
          Fan.status = false; Heater.status=false;
        }
        // check fan config

  }

  Fan.commit();
  Heater.commit();
  Led_strip.commit();
  dispenser.commit();
  //Fan.commit(); Heater.commit();
}
