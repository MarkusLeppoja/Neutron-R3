#include <Arduino.h>
#include <data_recorder.h>
#include <state_estimation.h>
//#include <alerts.h>

// Create a debug_mode  boolean that if enabled

// Automatically build a .csv string based on the enables for ground control and flash.csv logging Im not going to rewrite the csv string layout every time


//Neutron checklist: Ensure debug modes, unnessesary profiler prints are disabled
void setup() 
{
  Serial.begin(115200);
  Serial.setTimeout(0);
  sensors_init();

  delay(1000);
}

/*void setup1()
{
}*/

uint64_t sds, dsd;
void loop() 
{
  update_sensors();
  delay(1);
  if (micros() - sds >= 2000000)
  {
    sds = micros();
    cast_all_notifications_to_serial();
  }

  if (micros() - dsd >= 50000)
  {
    dsd = micros();

    Serial.print("Accel X: ");
    Serial.print(String(Sensors.raw_accel.x));
    Serial.print(" Y: ");
    Serial.print(String(Sensors.raw_accel.y));
    Serial.print(" Z: ");
    Serial.print(String(Sensors.raw_accel.z));

    Serial.print(" Gyro X: ");
    Serial.print(String(Sensors.raw_gyro_velocity.x));
    Serial.print(" Y: ");
    Serial.print(String(Sensors.raw_gyro_velocity.y));
    Serial.print(" Z: ");
    Serial.print(String(Sensors.raw_gyro_velocity.z));

    Serial.print(" Accel Temp: ");
    Serial.print(String(Sensors.raw_accel_temp));
    Serial.print(" Baro Temp: ");
    Serial.print(String(Sensors.raw_baro_temperature));

    Serial.print(" Pressure: ");
    Serial.print(String(Sensors.raw_baro_pressure));
    Serial.print(" Altitude: ");
    Serial.print(String(Sensors.raw_baro_altitude));

    Serial.print(" Voltage: ");
    Serial.println(String(Sensors.system_voltage));

  }
}

/*void loop1()
{

}*/

