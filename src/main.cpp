#include <Arduino.h>
#include <data_recorder.h>
#include <state_estimation.h>
#include <recovery.h>
#include <pyro.h>
#include <profiler.h>

// Create a debug_mode  boolean that if enabled

// Automatically build a .csv string based on the enables for ground control and flash.csv logging Im not going to rewrite the csv string layout every time
// Every time that something gets toggeled in datasave, we should regenerate the csv layout and print it to flash & serial 

//Neutron checklist: @YTFurys Ensure debug modes (@YTFurys What are debug modes?)
// TODO:
// Kalman filter for orientation
// Kalman filter for position
// Communication file (include state transition in there)
// State transition
// State indication (Import and rework legacy code)



void begin()
{
  Serial.begin(115200);
  Serial.setTimeout(0);


  uint8_t startup_status;
  // Startup
  pyro_begin();
  recovery_begin();
  startup_status += recorder_begin();
  startup_status += sensors_begin();
  // indicator_begin();

  if (startup_status != 2)
  {
    //alert
    //set_mission_state startup_failed
    return;
  }

  // Configure
  set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_1);
  set_recorder_serial_update_interval(active_vehicle_config.serial_stream_interval_mode_0);

  //alert startup compleate, moving to {next_state}
}

void prepare_for_logging()
{
  recorder_create_file(active_vehicle_config.flash_data_file_name, active_vehicle_config.flash_data_file_format);
  
  String csv_layout;  // Temp instance
  _recorder_create_csv_layout(csv_layout);
  _recorder_log_to_flash(csv_layout);
}

void setup() 
{
  begin();
}

uint64_t sds, dsd;  // @todo remove
void loop() 
{
  update_pyro();
  update_sensors();
  update_recorder();


  if (micros() - dsd >= 1000000)
  {
    dsd = micros();
    cast_all_notifications_to_serial();
  }
}

/* @todo Consider
void loop1()
void setup1()
*/
