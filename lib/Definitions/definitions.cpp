#include "definitions.h"


e_mission_state active_mission_state;
s_clock Clock;
s_booleans Booleans;
s_sensors Sensors;
s_code_performance Code_performance;
Neutron_Vehicle_Config_t active_vehicle_config = Electron_Test_Config;

e_mission_state get_mission_state()
{
    return active_mission_state;
}

Neutron_Vehicle_Config_t get_active_config()
{
    if (!active_mission_state)
    {
        //active_vehicle_config = *Electron_Test_Config;
    }

    return active_vehicle_config;
}

uint8_t is_mission_active()
{
    return (get_mission_state() >= e_mission_state::ascent && get_mission_state() < e_mission_state::landed) ? 1 : 0;
}

void update_mcu_clock()
{
    Clock.microseconds = micros();
    Clock.milliseconds = Clock.microseconds / 1000;
    Clock.seconds = Clock.milliseconds / 1000.f;
    
    /*if (is_mission_active())
    {
        Clock.mission_duration = Clock.seconds - Clock.mission_begin_time;
    }*/
}
uint64_t prev;//TODO: REMOVE OR REPLACE
void update_cpu_profiler()
{
    if (micros() - prev >= 5000)
    {
        prev = micros();
        Code_performance.cpu_clock_frequency = CLOCKS_BASE; //TODO: REVERT OR FIX
        //Code_performance.cpu_free_heap = clockCyclesPerMicrosecond;//TODO: REVERT OR FIX
        Code_performance.cpu_used_heap = F_CPU;//TODO: REVERT OR FIX
    }
}