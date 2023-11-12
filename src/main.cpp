#include <Arduino.h>
//#include <state_logic.h>
#include <state_estimation.h>
#include <data_recorder.h>
#include <state_estimation.h>
//TODO: Add option to format the entire device. ensure that formating closes all files and disables logging for a minute and restores it to default state after.
#include <pyro.h>
#include <recovery.h>

// REPLACE ALL PRINT FUNCTIONS WITH PRINT F AND USE PRINTLN FOR A LINE
Servo s;
void setup() 
{
  Serial.begin();
  Serial.setTimeout(0);
  //recorder_begin();
  set_recorder_serial_update_interval(get_active_config().serial_stream_interval_mode_4);
  //set_mission_state(e_mission_state::startup);
  s.attach(e_pins::pin_servo_3);
}

void loop() 
{
  update_pyro();
  update_cpu_profiler();
  update_mcu_clock();
  update_recorder();
  //update_state_machine();
}

void setup1()
{
  sensors_begin();

}

void loop1()
{

  update_sensors();
}

/* @todo Consider
void loop1()
void setup1()
*/
