#include "definitions.h"



Neutron_Vehicle_Config_t active_vehicle_config = Electron_Test_Config;

s_clock Clock;
s_booleans Booleans;
s_sensors Sensors;

void update_mcu_clock()
{
    Clock.microseconds = micros();
    Clock.milliseconds = Clock.microseconds / 1000;
    Clock.seconds = Clock.microseconds / 1000.f;

    /*
    if (is_mission_active)
    {
        Clock.mission_duration = Clock.seconds - Clock.mission_begin_time;
    }
    */
}