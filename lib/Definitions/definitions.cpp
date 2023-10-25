#include "definitions.h"


e_mission_state active_mission_state;

e_mission_state get_mission_state()
{
    return active_mission_state;
}

Neutron_Vehicle_Config_t active_vehicle_config = Electron_Flight_Config;

s_clock Clock;
s_booleans Booleans;
s_sensors Sensors;

void update_mcu_clock()
{
    Clock.microseconds = micros();
    Clock.milliseconds = Clock.microseconds / 1000;
    Clock.seconds = Clock.milliseconds / 1000.f;

    // @bug Clock.seconds returns 0 after 4294.95 seconds and this crashes the device
    
    if (is_mission_active())
    {
        Clock.mission_duration = Clock.seconds - Clock.mission_begin_time;
    }
}

boolean is_mission_active()
{
    return (active_mission_state >= e_mission_state::ascent && active_mission_state < e_mission_state::landed) ? true : false;
}