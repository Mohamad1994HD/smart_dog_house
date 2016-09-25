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

// Timer
Timer clock_;

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
  Serial.println("Temperature Low");
  Serial.println("Turning On Heater..");
  Serial.println(Temp_sensor.read());
  Fan.status = true;
  Heater.status = true;

}

void on_not_temp_triggered(){
  Serial.print("Temperature good: "); Serial.println(Temp_sensor.read());
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

void on_dog_inside(){
  Serial.println("Dog is inside");
  Serial.print(irsys.get_sensors_vals(1)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(2)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(3)); Serial.print(" ");
}

// action callback to commit changes to outputs
void on_action_schedule(){
  Serial.println("Commiting changes");
  Fan.commit();
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
  Serial.println("Checking light intensity");
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
  FAN_STATUS.load();

  connection = false;

  //init IRsystem
  irsys.set_callback(on_dog_inside);
  irsys.time_interval = 5000;
  irsys.trigger_val = 150;
  irsys.init();
  // init devices
  Fan.init();
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
        //   Fan.status = false; Heater.status=false;
        // }
        // check fan config

  }

  clock_.update();
}
