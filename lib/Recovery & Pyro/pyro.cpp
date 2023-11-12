#include "pyro.h"

uint64_t pyro_fire_start_time[2];
uint64_t pyro_prev;

Alerts alerts_pyro("Pyro");
Profiler profiler_pyro;

void pyro_begin()
{
    pinMode(e_pins::pin_pyro_1, OUTPUT);
    pinMode(e_pins::pin_pyro_2, OUTPUT);
    pinMode(e_pins::pin_pyro_1_voltage, INPUT);
    pinMode(e_pins::pin_pyro_2_voltage, INPUT);
}

void update_pyro()
{
    update_mcu_clock();
    profiler_pyro.begin_loop();

    // Only update after 50 milliseconds (50000 microseconds = 50 milliseconds)
    if (Clock.microseconds - pyro_prev < 50000) return;
    pyro_prev = Clock.microseconds;
    
    // Profiler
    profiler_pyro.end_loop(Code_performance.pyro_loop);
    profiler_pyro.begin_function();

    // Disable pyro if it was previously enabled and it's fire duration exceeded the allowed time.
    if (Clock.microseconds - pyro_fire_start_time[0] >= get_active_config().pyro_1_fire_duration && Sensors.pyro_1_fire_status) _pyro_1_deactivate();
    if (Clock.microseconds - pyro_fire_start_time[1] >= get_active_config().pyro_2_fire_duration && Sensors.pyro_2_fire_status) _pyro_2_deactivate();

    // Reads all incoming voltage of pyro channels 
    Sensors.pyro_1_voltage = (float) analogRead(e_pins::pin_pyro_1_voltage) * get_active_config().pyro_voltage_divider_ratio;
    Sensors.pyro_2_voltage = (float) analogRead(e_pins::pin_pyro_2_voltage) * get_active_config().pyro_voltage_divider_ratio;

    profiler_pyro.end_function(Code_performance.pyro_function_duration);
}

void pyro_1_fire()
{
    // Check if pyro functionality is enabled 
    if (!get_active_config().enable_pyro) return;

    // Open pyro 1 mosfet 
    digitalWrite(e_pins::pin_pyro_1, HIGH);
    Sensors.pyro_1_fire_status = 1;

    update_mcu_clock();
    pyro_fire_start_time[0] = Clock.microseconds;

    // Create alert
    alerts_pyro.create_alert(e_alert_type::alert, "Pyro channel 1 activated");
}

void pyro_2_fire()
{
    // Check if pyro functionality is enabled 
    if (!get_active_config().enable_pyro) return;

    // Open pyro 2 mosfet 
    digitalWrite(e_pins::pin_pyro_2, HIGH);
    Sensors.pyro_2_fire_status = 1;

    update_mcu_clock();
    pyro_fire_start_time[1] = Clock.microseconds;

    // Create alert
    alerts_pyro.create_alert(e_alert_type::alert, "Pyro channel 2 activated");
}

void _pyro_1_deactivate()
{
    // Close pyro 1 mosfet
    digitalWrite(e_pins::pin_pyro_1, LOW);
    Sensors.pyro_1_fire_status = 0;

    // Create alert
    alerts_pyro.create_alert(e_alert_type::alert, "Pyro channel 1 deactivated");
}

void _pyro_2_deactivate()
{
    // Close pyro 2 mosfet
    digitalWrite(e_pins::pin_pyro_2, LOW);
    Sensors.pyro_2_fire_status = 0;

    // Create alert
    alerts_pyro.create_alert(e_alert_type::alert, "Pyro channel 2 deactivated");
}