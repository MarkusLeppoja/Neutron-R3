#include <Arduino.h>
#include <data_recorder.h>
#include <state_estimation.h>
#include <recovery.h>
#include <pyro.h>
#include <profiler.h>

// Create a debug_mode  boolean that if enabled

// Automatically build a .csv string based on the enables for ground control and flash.csv logging Im not going to rewrite the csv string layout every time
// Every time that something gets toggeled in datasave, we should regenerate the csv layout and print it to flash & serial 

//Neutron checklist: Ensure debug modes, unnessesary profiler prints are disabled
void setup() 
{
  Serial.begin(115200);
  delay(5000);
  Serial.setTimeout(0);
  pyro_init();
  sensors_init();

  /*recorder_begin();
  recorder_create_file(active_vehicle_config.flash_data_file_name, active_vehicle_config.flash_data_file_format);*/
  set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_1);
  set_recorder_serial_update_interval(active_vehicle_config.serial_stream_interval_mode_0);

  /*String csv_layout;
  _recorder_create_csv_layout(csv_layout);
  _recorder_log_to_flash(csv_layout);*/

  pinMode(e_pins::pin_led_rgb_r, OUTPUT);
  pinMode(e_pins::pin_led_rgb_g, OUTPUT);
  pinMode(e_pins::pin_led_rgb_b, OUTPUT);
  /*digitalWrite(e_pins::pin_led_rgb_r, HIGH);
  digitalWrite(e_pins::pin_led_rgb_g, HIGH);
  digitalWrite(e_pins::pin_led_rgb_b, HIGH);*/

}

uint64_t sds, dsd;
void loop() 
{
  update_pyro();
  update_sensors();
  update_recorder();

  if (micros() - sds >= 10000000)
  {
    sds = micros();
  }

  if (micros() - dsd >= 1000000)
  {
    dsd = micros();
    cast_all_notifications_to_serial();
  }
}

/*void loop1()
{

}*/

/*void setup1()
{
}*/
