#include "Recources.h"

SoftwareSerial soft_ser(12,13);
SerialExtractor ser(soft_ser);

bool connection = false;
bool is_dog_inside = false;
//Configurations
Config TEMP_THRES_HOLD(0);
Config LIGHT_THRES_HOLD(1);
Config FOOD_TIME_SPACE(2);
Config Fan_STATUS(3);

//IR Detection System
Sensor s1(A3);
Sensor s2(A4);
Sensor s3(A5);
IRSystem irsys(s1, s2, s3);

//Devices
Fan fan(4);
Device Heater(3);
Device Led_strip(5);
//Food-water Dispenser unit
Dispenser dispenser(8, 9, 10, 11, 7);

//Sensors
Sensor water_sensor(A0);
Sensor light_sensor(A1);

DHT dht(6, DHT22);
TempSensor Temp_sensor(6, dht);

// Timer
Timer clock_;

void on_command(int device_num, uint8_t command){
    switch(device_num){
      case 1:
      // fan
      fan.status = (bool)command;
      break;
      case 2:
      // Heater
      Heater.status = (bool)command;
      break;
      case 3:
      // Lights
      //Led_strip.status = (bool)command;
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
  Fan_STATUS.update(a[4]);
}

void logConf(){
  Serial.print("TEMP:");
  Serial.println(TEMP_THRES_HOLD.read());
  Serial.print("LIGHT:");
  Serial.println(LIGHT_THRES_HOLD.read());
  Serial.print("FOOD:");
  Serial.println(FOOD_TIME_SPACE.read());
  Serial.print("fan:");
  Serial.println(Fan_STATUS.read());
}

void on_connect(){
  // send Configurations
  logConf();
}


void on_disconnect(){
  //commit changes to config
  TEMP_THRES_HOLD.commit();
  Fan_STATUS.commit();
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


////// Sensory callbacks
// light
void on_not_light_triggered(){
  Serial.print("Light light: ");
  Serial.println(light_sensor.read());
  if(!connection){
    Led_strip.status = false;
  }
}

void on_light_triggered(){
  Serial.print("Dim light:");
  Serial.println(light_sensor.read());
  if (!connection){
    Led_strip.status = true;
  }
}

// Temperature
void on_temp_triggered(){
  if(!is_dog_inside){return;}

  Serial.println("Temperature Low");
  Serial.println("Turning On Heater..");
  Serial.println(Temp_sensor.read());
  fan.status = true;
  Heater.status = true;

}

void on_not_temp_triggered(){
// Check fan mode winter/summer
  if(fan.mode == SUMMER){
    if(is_dog_inside){fan.status=true;}
  }else{ // so no cold air would pass through
    fan.status = false;
  }

  Serial.print("Temperature good: "); Serial.println(Temp_sensor.read());
  Heater.status = false;
}

// water need Sensor
void on_water_need(){
  Serial.println("Dispensing water");
  dispenser.set_dispenser_status(DISPENSE_WATER);
}

void on_not_water_need(){}
// food time
void on_food_time(){
  Serial.println("Dispensing food");
  dispenser.set_dispenser_status(DISPENSE_WATER);
}
void on_not_food_time(){}

// Dog inside detection
void on_dog_inside(){
  Serial.println("Dog is inside");
  // if fan mode == summer turn it on, else nop
  fan.status = true;

  Serial.print(irsys.get_sensors_vals(1)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(2)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(3)); Serial.print(" ");
}

void on_dog_outside(){
  Serial.println("Dog is outside");
  fan.status = false;
}

// action callback to commit changes to outputs
void on_action_schedule(){
  Serial.println("Commiting changes");
  fan.commit();
  Heater.commit();
  Led_strip.commit();
  dispenser.commit();
}

// Sensors callbacks interfaces
// due to difficulty of passing class method as callback
void run_temp_sensor(){
  Temp_sensor.run();
}

void run_light_sensor(){
  light_sensor.run();
}

void run_water_sensor(){
  water_sensor.run();
}

void run_irsys(){
  irsys.run();
}
////

void setup() {
  // // Load Configuration
  TEMP_THRES_HOLD.load();
  LIGHT_THRES_HOLD.load();
  FOOD_TIME_SPACE.load();
  Fan_STATUS.load();

  connection = false;

  // init devices
  fan.init();
  Heater.init();
  Led_strip.init();

  dispenser.set_dispense_time(2000);
  dispenser.init();

  //init Sensors
  // Water sensor IR
  water_sensor.init();
  water_sensor.time_interval = 1000;
  water_sensor.trigger_val = 200;
  water_sensor.set_trigger_status(SMALLER);
  water_sensor.set_on_trigger(on_water_need);
  water_sensor.set_on_not_triggered(on_not_water_need);
  // Light Intensity Sensor
  light_sensor.init();
  light_sensor.time_interval = 2500;
  light_sensor.trigger_val = 500;
  light_sensor.set_trigger_status(SMALLER);
  light_sensor.set_on_trigger(on_light_triggered);
  light_sensor.set_on_not_triggered(on_not_light_triggered);
  // Temperature Sensor
  Temp_sensor.init();
  Temp_sensor.time_interval = 2000;
  Temp_sensor.trigger_val = 30;
  Temp_sensor.set_trigger_status(SMALLER);
  Temp_sensor.set_on_trigger(on_temp_triggered);
  Temp_sensor.set_on_not_triggered(on_not_temp_triggered);
  //init IRsystem
  irsys.set_callback(on_dog_inside);
  irsys.set_not_callback(on_dog_outside);
  irsys.time_interval = 5000;
  irsys.trigger_val = 150;
  irsys.init();
  // initialize SerialExtractor
  ser.SetCallBack(on_msg_received);
  ser.SetDelimeter(":");
  ser.SetEndIndicator('#');
  ser.init(9600);
 // initialize serial port for logging messages
  Serial.begin(9600);

// initialize timer callbacks
 clock_.every(ACTION_TIME, on_action_schedule);
 clock_.every(light_sensor.time_interval, run_light_sensor);
 clock_.every(Temp_sensor.time_interval, run_temp_sensor);
 clock_.every(water_sensor.time_interval, run_water_sensor);
 clock_.every(irsys.time_interval, run_irsys);
  // food timer
}

unsigned long BLINK_TIME = 1000;


void loop() {
  ser.Run();
  if (connection){
    auto flasher = []()->void{
      Led_strip.status = !Led_strip.status;
    };
    clock_.after(BLINK_TIME, flasher);

  }else{
    //check fan config

    // if light < thresshold
    // if (light_sensor.read() > light_sensor.trigger_val ){
    //   Led_strip.status = false;
    // }

    //if dog infront of water sensor
    // done via callback

    // if food time

    // if dog inside
      // check fan config
      // if temperature > thresshold
        // if (Temp_sensor.read() > Temp_sensor.trigger_val){
        //   fan.status = false; Heater.status=false;
        // }
        // check fan config

  }

  clock_.update();
}
