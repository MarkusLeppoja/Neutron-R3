#include <Arduino.h>
//#include <state_logic.h>
#include <data_recorder.h>
#include <state_estimation.h>
#include <pyro.h>
#include <recovery.h>


//TODO: Add option to format the entire device. ensure that formating closes all files and disables logging for a minute and restores it to default state after.
void setup() 
{
  Serial.begin();
  Serial.setTimeout(0);
  sensors_begin();
  recorder_begin();
  set_serial_log_interval(active_vehicle_config.serial_stream_interval_mode_4, 1);
  set_serial_log_interval(active_vehicle_config.serial_stream_interval_mode_4, 2);
  //set_mission_state(e_mission_state::startup);
  delay(2000);
  imu_enable_calibration();
  baro_enable_calibration();
}

void loop() 
{
  //update_pyro();
  //update_cpu_profiler();
  //update_mcu_clock();
  //update_state_machine();
  update_sensors();
  update_recorder();
}

void setup1()
{

}

void loop1()
{

}

/* @todo Consider
void loop1()
void setup1()
*/
