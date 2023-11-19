#include "data_recorder.h"
#include "storing_logic.h"
#include "flash.h"
#define zero 0
#define one 1

uint64_t file_0_update_interval, file_1_update_interval = 1000000;
uint64_t serial_0_cast_interval, serial_1_cast_interval = 1000000;

uint64_t file_0_update_prev, file_1_update_prev;
uint64_t serial_0_cast_prev, serial_1_cast_prev;

boolean is_file_0_tlm_recent, is_file_1_tlm_recent;

Profiler profiler_file_func_dur;

//TODO: Add logging logic for alerts. Make it interrupt based
//TODO: Log active_vehicle_config data

void set_flash_log_interval(uint64_t interval, int file_count)
{
    switch (file_count)
    {
    case 1:
        file_0_update_interval = interval;
        break;
    case 2:
        file_1_update_interval = interval;
        break;
    
    default: break;
    }
}

void set_serial_log_interval(uint64_t interval, int file_count)
{
    switch (file_count)
    {
    case 1:
        serial_0_cast_interval = interval;
        break;
    case 2:
        serial_1_cast_interval = interval;
        break;
    
    default: break;
    }
}

int recorder_begin()
{
    return flash_device.init(active_vehicle_config.enable_flash_log);
}

void update_recorder()
{
    _flash_update();
    _serial_update();
}

void _flash_update()
{
    if (!active_vehicle_config.enable_flash_log) return;

    update_mcu_clock();
    if (Clock.microseconds - file_0_update_prev >= file_0_update_interval && active_vehicle_config.enable_flash_file_0_log && flash_device.is_file_open(flash_device.get_file_instance(zero)))
    {
        file_0_update_prev = Clock.microseconds;
        _file_0_conversion_logic(); // Call conversion function for file 0
        is_file_0_tlm_recent = 1;   // Indicate that the tlm conversion function was recently called.
        flash_device.write_line(storing_logic.get_main_buffer_instance(), flash_device.get_file_instance(zero));  // Log data to file 0
    }

    update_mcu_clock();
    if (Clock.microseconds - file_1_update_prev >= file_1_update_interval && active_vehicle_config.enable_flash_file_1_log && flash_device.is_file_open(flash_device.get_file_instance(one)))
    {
        file_1_update_prev = Clock.microseconds;
        _file_1_conversion_logic(); // Call conversion function for file 0
        is_file_1_tlm_recent = 1;   // Indicate that the tlm conversion function was recently called.
        flash_device.write_line(storing_logic.get_main_buffer_instance(), flash_device.get_file_instance(one));  // Log data to file 0
    }
}

void _serial_update()
{
    if (!active_vehicle_config.enable_serial_stream) return;

    update_mcu_clock();
    if (Clock.microseconds - serial_0_cast_prev >= serial_0_cast_interval && active_vehicle_config.enable_serial_file_0_stream)
    {
        serial_0_cast_prev = Clock.microseconds;
        if (!is_file_0_tlm_recent) 
        {
            _file_0_conversion_logic();  // Call conversion function for file 0
        }
        is_file_0_tlm_recent = 0;
        Serial.printf(storing_logic.get_main_buffer_instance().c_str());
        Serial.printf("\n");
    }

    update_mcu_clock();
    if (Clock.microseconds - serial_1_cast_prev >= serial_1_cast_interval && active_vehicle_config.enable_serial_file_1_stream)
    {
        serial_1_cast_prev = Clock.microseconds;
        if (!is_file_1_tlm_recent) 
        {
            _file_1_conversion_logic();  // Call conversion function for file 1
        }
        is_file_1_tlm_recent = 0;
        Serial.printf(storing_logic.get_main_buffer_instance().c_str());
        Serial.printf("\n");
    }
}

void _file_0_conversion_logic()
{
    profiler_file_func_dur.begin_function();
    storing_logic.clear_buffer();

    if (active_vehicle_config.enable_datasave_basic)
    {
        storing_logic.convert_variable(Clock.seconds, -8, 3);
        storing_logic.convert_variable(Clock.mission_duration, -8, 3);
        storing_logic.convert_variable(get_mission_state());
    }

    if (active_vehicle_config.enable_datasave_imu_data)
    {
        storing_logic.convert_variable(Sensors.acceleration.x, -8, 2);
        storing_logic.convert_variable(Sensors.acceleration.y, -8, 2);
        storing_logic.convert_variable(Sensors.acceleration.z, -8, 2);
        storing_logic.convert_variable(Sensors.gyro_velocity_dps.x, -8, 2);
        storing_logic.convert_variable(Sensors.gyro_velocity_dps.y, -8, 2);
        storing_logic.convert_variable(Sensors.gyro_velocity_dps.z, -8, 2);
        storing_logic.convert_variable(Sensors.orientation.x, -8, 2);
        storing_logic.convert_variable(Sensors.orientation.y, -8, 2);
        storing_logic.convert_variable(Sensors.orientation.z, -8, 2);
    }

    if (active_vehicle_config.enable_datasave_baro_data)
    {
        storing_logic.convert_variable(Sensors.raw_baro_altitude_wo_bias, -7, 2);
        storing_logic.convert_variable(Sensors.position.z, -8, 3);
        storing_logic.convert_variable(Sensors.velocity.z, -7, 3);
    }

    if (active_vehicle_config.enable_datasave_performance)
    {
        storing_logic.convert_variable(Code_performance.imu_loop, 6, 2);
        storing_logic.convert_variable(Code_performance.baro_loop, 6, 2);
        storing_logic.convert_variable(Code_performance.voltage_divider_loop, 6, 2);
        storing_logic.convert_variable(Code_performance.imu_function_duration, 6, 2);
        storing_logic.convert_variable(Code_performance.baro_function_duration, 6, 2);
        storing_logic.convert_variable(Code_performance.voltage_divider_function_duration, 6, 2);
        storing_logic.convert_variable(Code_performance.data_recorder_file_0_func_duration, 6, 2);
        storing_logic.convert_variable(Code_performance.data_recorder_file_1_func_duration, 6, 2);
    }

    storing_logic.remove_previous_comma();
    Code_performance.data_recorder_file_0_func_duration = profiler_file_func_dur.end_function();
}

void _file_0_conversion_layout()
{
    storing_logic.clear_buffer();

    if (active_vehicle_config.enable_datasave_basic)
    {
        storing_logic.convert_variable("On Time (sec)");
        storing_logic.convert_variable("Mission Duration (sec)");
        storing_logic.convert_variable("Mission State");
    }

    if (active_vehicle_config.enable_datasave_imu_data)
    {
        storing_logic.convert_variable("Accel x (m/s2)");
        storing_logic.convert_variable("Accel y (m/s2)");
        storing_logic.convert_variable("Accel z (m/s2)");
        storing_logic.convert_variable("Angular Vel x (dps)");
        storing_logic.convert_variable("Angular Vel y (dps)");
        storing_logic.convert_variable("Angular Vel z (dps)");
        storing_logic.convert_variable("Orientation x (deg)");
        storing_logic.convert_variable("Orientation y (deg)");
        storing_logic.convert_variable("Orientation z (deg)");
    }

    if (active_vehicle_config.enable_datasave_baro_data)
    {
        storing_logic.convert_variable("Raw Altitude (m)");
        storing_logic.convert_variable("Altitude (m)");
        storing_logic.convert_variable("Velocity (m)");
    }

    if (active_vehicle_config.enable_datasave_performance)
    {
        storing_logic.convert_variable("IMU Loop (ms)");
        storing_logic.convert_variable("Baro Loop (ms)");
        storing_logic.convert_variable("V-Div Loop (ms)");
        storing_logic.convert_variable("IMU F Duration (ms)");
        storing_logic.convert_variable("Baro F Duration (ms)");
        storing_logic.convert_variable("V-Div F Duration (ms)");
        storing_logic.convert_variable("File 0 F Duration (ms)");
        storing_logic.convert_variable("File 1 F Duration (ms)");
    }

    storing_logic.remove_previous_comma();
}

void _file_1_conversion_logic()
{
    profiler_file_func_dur.begin_function();
    storing_logic.clear_buffer();

    if (active_vehicle_config.enable_datasave_basic)
    {
        storing_logic.convert_variable(Clock.seconds, -8, 3);
        storing_logic.convert_variable(Clock.mission_duration, -8, 3);
        storing_logic.convert_variable(get_mission_state());
    }

    if (active_vehicle_config.enable_datasave_pyro)
    {
        storing_logic.convert_variable(Sensors.pyro_1_fire_status);
        storing_logic.convert_variable(Sensors.pyro_1_voltage, -5, 2);
        storing_logic.convert_variable(Sensors.pyro_2_fire_status);
        storing_logic.convert_variable(Sensors.pyro_2_voltage, -5, 2);
    }
    
    if (active_vehicle_config.enable_datasave_imu_data)
    {
        storing_logic.convert_variable(Sensors.raw_acceleometer_temperature, -7, 2);
    }

    if (active_vehicle_config.enable_datasave_baro_data)
    {
        storing_logic.convert_variable(Sensors.raw_baro_altitude, -8, 2);
        storing_logic.convert_variable(Sensors.raw_baro_pressure, -8, 2);
        storing_logic.convert_variable(Sensors.raw_baro_temperature, -6, 2);
    }

    if (active_vehicle_config.enable_voltage_divider)
    {
        storing_logic.convert_variable(Sensors.internal_voltage, -6, 2);
    }

    storing_logic.remove_previous_comma();
    Code_performance.data_recorder_file_1_func_duration = profiler_file_func_dur.end_function();
}

void _file_1_conversion_layout()
{
    storing_logic.clear_buffer();

    if (active_vehicle_config.enable_datasave_basic)
    {
        storing_logic.convert_variable("On Time (sec)");
        storing_logic.convert_variable("Mission Duration (sec)");
        storing_logic.convert_variable("Mission State");
    }

    if (active_vehicle_config.enable_datasave_pyro)
    {
        storing_logic.convert_variable("Pyro 1 Fire");
        storing_logic.convert_variable("Pyro 1 Voltage");
        storing_logic.convert_variable("Pyro 2 Fire");
        storing_logic.convert_variable("Pyro 2 Voltage");
    }

    if (active_vehicle_config.enable_datasave_imu_data)
    {
        storing_logic.convert_variable("IMU Temperature (*C)");
    }

    if (active_vehicle_config.enable_datasave_baro_data)
    {
        storing_logic.convert_variable("Raw Baro Altitude (m)");
        storing_logic.convert_variable("Pressure (hPa)");
        storing_logic.convert_variable("Baro Temperature (*C)");
    }

    if (active_vehicle_config.enable_datasave_v_divider_data)
    {
        storing_logic.convert_variable("Voltage (V)");
    }

    storing_logic.remove_previous_comma();
}