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

void set_flash_log_interval(uint64_t interval, int file_count)
{
    switch (file_count)
    {
    case 0:
        file_0_update_interval = interval;
        break;
    case 1:
        file_1_update_interval = interval;
        break;
    
    default: break;
    }
}

void set_serial_log_interval(uint64_t interval, int file_count)
{
    switch (file_count)
    {
    case 0:
        serial_0_cast_interval = interval;
        break;
    case 1:
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
    if (Clock.microseconds - file_0_update_prev >= file_0_update_interval && active_vehicle_config.enable_flash_file_0_log && flash_device.is_file_open(data_file_0))
    {
        file_0_update_prev = Clock.microseconds;
        _file_0_conversion_logic(); // Call conversion function for file 0
        is_file_0_tlm_recent = 1;   // Indicate that the tlm conversion function was recently called.
        flash_device.write_line(storing_logic.get_main_buffer_instance(), data_file_0);  // Log data to file 0

        _file_1_conversion_logic();
        is_file_1_tlm_recent = 1;   // Indicate that the tlm conversion function was recently called.
        flash_device.write_line(storing_logic.get_main_buffer_instance(), data_file_0);  // Log data to file 0
        flash_device.write_line("\n", data_file_0);
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

void cast_alert_to_data_recorder(String alert)
{
    // Save string to RAM
    _alert_string.concat(alert);
    _alert_string.concat("\n");

    // Serial implementation
    if (active_vehicle_config.enable_serial_stream && active_vehicle_config.enable_serial_notification_stream)
    {
        Serial.printf(alert.c_str());
        Serial.printf("\n");
    }

    flash_device.write_line(alert.c_str(), data_file_2);
    flash_device.write_line("\n", data_file_2);
}

void open_all_flash_files()
{
    flash_alerts.create_alert(e_alert_type::alert, "Creating all flight files");

    flash_device.create_file(active_vehicle_config.flash_data_file_0_name, data_file_0, active_vehicle_config.flash_data_file_0_format);
    //flash_device.create_file(active_vehicle_config.flash_data_file_1_name, data_file_1, active_vehicle_config.flash_data_file_1_format);
    flash_device.create_file(active_vehicle_config.flash_data_file_2_name, data_file_2, active_vehicle_config.flash_data_file_2_format);
    delay(1);

    _file_0_conversion_layout();
    flash_device.write_line(storing_logic.get_main_buffer_instance().c_str(), data_file_0);

    _file_1_conversion_layout();
    flash_device.write_line(storing_logic.get_main_buffer_instance().c_str(), data_file_0); //TODO: 0 - 1

    flash_device.write_line(get_all_alerts().c_str(), data_file_2);
}

void close_all_flash_files()
{
    flash_device.close_file(data_file_0);
    flash_device.close_file(data_file_2);
}

void delete_all_flash_files()
{
    flash_device.remove_file(active_vehicle_config.flash_data_file_0_name + active_vehicle_config.flash_data_file_0_format);
    flash_device.remove_file(active_vehicle_config.flash_data_file_2_name + active_vehicle_config.flash_data_file_2_format);
}

void cast_message_to_serial(String message)
{
    if (!active_vehicle_config.enable_serial_stream) return;

    Serial.printf(message.c_str());
    Serial.printf("\n");
}

void cast_file_0_data()
{
    flash_device.print_file_content(active_vehicle_config.flash_data_file_0_name + active_vehicle_config.flash_data_file_0_format, data_file_0);
}

void cast_file_1_data()
{
    //flash_device.print_file_content(active_vehicle_config.flash_data_file_1_name + active_vehicle_config.flash_data_file_1_format, data_file_1);
}

void cast_file_2_data()
{
    flash_device.print_file_content(active_vehicle_config.flash_data_file_2_name + active_vehicle_config.flash_data_file_2_format, data_file_2);
}

void check_if_3_files_exists()
{
    if (flash_device.does_file_exist(active_vehicle_config.flash_data_file_0_name + active_vehicle_config.flash_data_file_0_format))
    {
        flash_alerts.create_alert(e_alert_type::warning, "File " + active_vehicle_config.flash_data_file_0_name + active_vehicle_config.flash_data_file_0_format + " exists");
    }

    /*if (flash_device.does_file_exist(active_vehicle_config.flash_data_file_1_name + active_vehicle_config.flash_data_file_1_format))
    {
        flash_alerts.create_alert(e_alert_type::warning, "File " + active_vehicle_config.flash_data_file_1_name + active_vehicle_config.flash_data_file_1_format + " exists");
    }*/

    if (flash_device.does_file_exist(active_vehicle_config.flash_data_file_2_name + active_vehicle_config.flash_data_file_2_format))
    {
        flash_alerts.create_alert(e_alert_type::warning, "File " + active_vehicle_config.flash_data_file_2_name + active_vehicle_config.flash_data_file_2_format + " exists");
    }
}