#include "definitions.h"
#include "vehicle_configs.h"


Neutron_Vehicle_Config *active_vehicle_config = Electron_Flight_5_Config;

s_clock Clock;

void update_mcu_clock()
{
    Clock.microseconds = micros();
    Clock.milliseconds = Clock.microseconds / 1000;
    Clock.seconds = Clock.microseconds / 1000;

    /*
    if (is_mission_active)
    {
        Clock.mission_duration = Clock.seconds - Clock.mission_begin_time;
    }
    */
}