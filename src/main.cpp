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
Timer commit_clock;

// Timers ids
uint8_t LED_TIMER;

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
  TEMP_THRES_HOLD.update((uint8_t)a[1]);
  LIGHT_THRES_HOLD.update((uint8_t)a[2]);
  FOOD_TIME_SPACE.update((uint8_t)a[3]);
  Fan_STATUS.update((uint8_t)a[4]);
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

void FLASH(){
  Led_strip.status = !Led_strip.status;
  Serial.println("FLASH");
}

// This function will clear the registered callbacks with the clock
void reset_clock(){
  for (uint8_t i=0; i<255; i++){
    clock_.stop(i);
  }
}
void on_connect(){
  // send Configurations

  LED_TIMER = clock_.every(1000, FLASH);

  logConf();
}


void on_disconnect(){
  //commit changes to config
  TEMP_THRES_HOLD.commit();
  Fan_STATUS.commit();
  FOOD_TIME_SPACE.commit();
  LIGHT_THRES_HOLD.commit();

  Serial.println("Connection Destroyed");
  reset_clock();
  //reset the MCU
  delay(1000);
  setup();
}

void on_request(){
  int light = light_sensor.read();
  int temp = Temp_sensor.read();
  soft_ser.print(temp); soft_ser.print(":"); soft_ser.print(light);
  soft_ser.flush();
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
  if(connection){return;}
  Serial.println("Light light");
  if(!connection){
    Led_strip.status = false;
  }
}

void on_light_triggered(){
  if(connection){return;}
  Serial.println("Dim light");
  if (!connection){
    Led_strip.status = true;
  }
}

// Temperature
void on_temp_triggered(){
  Serial.println("TEMP_TRG");
  if(connection){return;}
  // if the dog isnt inside.. turn off the heater + fan
  if(!is_dog_inside){
    fan.status = false;
    Heater.status = false;
    return;
  }

  Serial.println("Temperature Low");
  Serial.println("Turning On Heater..");

  fan.status = true;
  Heater.status = true;

}

void on_not_temp_triggered(){
// Check fan mode winter/summer
  if(connection){return;}
  Serial.println("Temperature good");
  // heater off
  Heater.status = false;

  if(!is_dog_inside){fan.status = false; return;}
  // if dog is inside
  // if its winter turn off
  // if its summer turn on
  fan.mode == WINTER? fan.status = false: fan.status=true;

}

// water need Sensor
void on_water_need(){
  if(connection){return;}
  Serial.println("Dispensing water");
  dispenser.set_dispense_time(2000);

  dispenser.set_dispenser_status(DISPENSE_WATER);
}

void on_not_water_need(){}
// food time
void on_food_time(){
  if(connection){return;}
  Serial.println("Dispensing food");
  dispenser.set_dispense_time(2000);

  dispenser.set_dispenser_status(DISPENSE_FOOD);
}
void on_not_food_time(){}

// Dog inside detection
void on_dog_inside(){
  Serial.println("Dog is inside");
  // if fan mode == summer turn it on, else nop
  //fan.status = true;
  is_dog_inside = true;

  Serial.print(irsys.get_sensors_vals(1)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(2)); Serial.print(" ");
  Serial.print(irsys.get_sensors_vals(3)); Serial.print(" ");
}

void on_dog_outside(){
  Serial.println("Dog is outside");

  is_dog_inside = false;
//  fan.status = false;
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
  Serial.print("Temperature val: "); Serial.println(Temp_sensor.read());
}

void run_light_sensor(){
  light_sensor.run();
  Serial.print("Light:");Serial.println(light_sensor.read());
}

void run_water_sensor(){
  water_sensor.run();
}


void run_irsys(){
  irsys.run();
}

////
void run_serial_ext(){
  ser.Run();
}


void setup() {

  Serial.begin(9600);

  Serial.println("Initializing the system");
  // // Load Configuration
  TEMP_THRES_HOLD.load();
  LIGHT_THRES_HOLD.load();
  FOOD_TIME_SPACE.load();
  Fan_STATUS.load();



  connection = false;

  // init devices
  fan.mode = Fan_STATUS.read()==1? WINTER:SUMMER;
  fan.init();
  Heater.init();
  Led_strip.init();

  dispenser.init();

  //init Sensors
  // Water sensor IR
  water_sensor.init();
  water_sensor.time_interval = 1100;
  water_sensor.trigger_val = 200;
  water_sensor.set_trigger_status(SMALLER);
  water_sensor.set_on_trigger(on_water_need);
  water_sensor.set_on_not_triggered(on_not_water_need);
  // Light Intensity Sensor
  light_sensor.init();
  light_sensor.time_interval = 1500;
  light_sensor.trigger_val = map((int)LIGHT_THRES_HOLD.read(), 0, 255, 0, 1023);
  light_sensor.set_trigger_status(SMALLER);
  light_sensor.set_on_trigger(on_light_triggered);
  light_sensor.set_on_not_triggered(on_not_light_triggered);
  // Temperature Sensor
  Temp_sensor.init();
  Temp_sensor.time_interval = 1000;
  Temp_sensor.trigger_val = TEMP_THRES_HOLD.read();
  Temp_sensor.set_trigger_status(SMALLER);
  Temp_sensor.set_on_trigger(on_temp_triggered);
  Temp_sensor.set_on_not_triggered(on_not_temp_triggered);

  //init IRsystem
  irsys.set_callback(on_dog_inside);
  irsys.set_not_callback(on_dog_outside);
  irsys.time_interval = 1000;
  irsys.trigger_val = 150;
  irsys.init();
  // initialize SerialExtractor
  ser.SetCallBack(on_msg_received);
  ser.SetDelimeter(":");
  ser.SetEndIndicator('#');
  ser.init(9600);
 // initialize serial port for logging messages

// initialize timer callbacks
 clock_.every(300, on_action_schedule);
 clock_.every(light_sensor.time_interval, run_light_sensor);
 clock_.every(Temp_sensor.time_interval, run_temp_sensor);
 clock_.every(water_sensor.time_interval, run_water_sensor);

 if (FOOD_TIME_SPACE.read() < 10){
   FOOD_TIME_SPACE.update(60);
   FOOD_TIME_SPACE.commit();
   setup();
 }
 long fdt = FOOD_TIME_SPACE.read();
 fdt *= 1000;

 clock_.every(fdt, on_food_time);
 clock_.every(irsys.time_interval, run_irsys);
 clock_.every(100, run_serial_ext);
  // food timer


  Serial.println("-------------------");
  Serial.print("Light Trigger: "); Serial.println(light_sensor.trigger_val);
  Serial.print("Temp Trigger: "); Serial.println(TEMP_THRES_HOLD.read());
  Serial.print("Food every: "); Serial.print(fdt); Serial.println("ms");
  Serial.print("Fan Mode: "); Serial.println(fan.mode);
  Serial.println("-------------------");
}

unsigned long BLINK_TIME = 1000;


void loop() {
  clock_.update();
}
