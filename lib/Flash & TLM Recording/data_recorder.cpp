#include "data_recorder.h"


// Instances
flash_device recorder_flash_instance;
uint64_t _recorder_serial_prev, _recorder_flash_prev;
uint64_t _recorder_flash_update_interval = 1000000, _recorder_serial_update_interval = 1000000;
char _recorder_char_buffer[10], _recorder_string_buffer[512];



void recorder_earase_flash_chip()
{
    recorder_flash_instance.erase_chip();
}

void set_recorder_flash_update_interval(uint64_t interval)
{
    _recorder_flash_update_interval = interval;
}

void set_recorder_serial_update_interval(uint64_t interval)
{
    _recorder_serial_update_interval = interval;
}

int recorder_begin()
{
    return recorder_flash_instance.init(active_vehicle_config.enable_flash_log);
}

void update_recorder()
{
    update_mcu_clock();
    if (Clock.microseconds - _recorder_flash_prev >= _recorder_flash_update_interval && active_vehicle_config.enable_flash_log)
    {
        _recorder_flash_prev = Clock.microseconds;
        _flash_update();
    }

    update_mcu_clock();
    if (Clock.microseconds - _recorder_serial_prev >= _recorder_serial_update_interval && active_vehicle_config.enable_serial_stream)
    {
        _recorder_serial_prev = Clock.microseconds;
        _serial_update();
    }
}

void _flash_update()
{
    // Don't try to log data if no data file is open or the flash chip isn't usable
    if (!recorder_flash_instance.get_flash_chip_usability_status() || !recorder_flash_instance.is_file_open()) return;
    
    if (active_vehicle_config.enable_flash_notification_log && _alert_buffer_size != 0)
    {
        // Load the buffer onto a string and clear it
        String _flash_log_string;
        for (uint8_t i = 0; i < _alert_buffer_size; i++)
        {
            _flash_log_string += _alert_buffer[i];
        }

        // Reset buffer & its size
        clear_alert_buffer();
        
        // Log the string to flash
        _recorder_log_to_flash(_flash_log_string);
    }

    // Log telemetry to flash if functionality is enabled
    if (active_vehicle_config.enable_flash_telemetry_log)
    {
        // Temp string instance
        String _flash_log_string;

        // Convert all data to a string
        _recorder_convert_data_to_string(_flash_log_string);

        // Log the string to flash
        _recorder_log_to_flash(_flash_log_string);
    }
}

void cast_all_notifications_to_serial()
{
    // Check if alert buffer contains an alert and if flash notification logging is toggled
    if (active_vehicle_config.enable_serial_notification_stream && active_vehicle_config.enable_serial_stream)
    {
        // Cast the notification string
        Serial.println(get_all_alerts());
    }
}

void _serial_update()
{
    // Check if serial telemetry stream functionality is enabled. 
    if (!active_vehicle_config.enable_serial_telemetry_stream) return;
    
    // Temp string instance
    String _serial_log_string;

    // Convert all data to a string
    _recorder_convert_data_to_string(_serial_log_string);

    // Cast string over serial
    Serial.println(_serial_log_string);
    
}









void _recorder_convert_data_to_string(String &end_result_inst)
{
    // Reset buffer
    _recorder_string_buffer[0] = 0;

    /* General */
    if (active_vehicle_config.enable_datasave_general)
    {
        _convert_var_to_string(Clock.seconds, 6, 2);
        _convert_var_to_string(Clock.mission_duration, 6, 2);
        _convert_var_to_string(get_mission_state());
        //_convert_var_to_string();
        //_convert_var_to_string();

        // @todo mission state, mass (calulate mass based on acceleratinon), thrust
    }

    /* Pyro */
    if (active_vehicle_config.enable_datasave_pyro)
    {
        _convert_var_to_string(Sensors.pyro_1_fire_status);
        _convert_var_to_string(Sensors.pyro_1_voltage, 4, 2);
        _convert_var_to_string(Sensors.pyro_2_fire_status);
        _convert_var_to_string(Sensors.pyro_2_voltage, 4, 2);
    }

    /* Sensors */
    if (active_vehicle_config.enable_datasave_imu_data)
    {
        _convert_var_to_string(Sensors.accel.x, 6, 2);
        _convert_var_to_string(Sensors.accel.y, 6, 2);
        _convert_var_to_string(Sensors.accel.z, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity.x, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity.y, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity.z, 6, 2);
        _convert_var_to_string(Sensors.raw_accel_temp, 5, 2);


        // @todo filtered accel, gyro ang vel and ori, total velocity.
    }

    // Baro
    if (active_vehicle_config.enable_datasave_baro_data)
    {
        _convert_var_to_string(Sensors.raw_baro_altitude, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_altitude_wo_bias, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_pressure, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_temperature, 5, 2);
        _convert_var_to_string(Sensors.altitude, 8, 2);
        _convert_var_to_string(Sensors.velocity, 6, 2);

        // @todo filtered altitude
    }

    // GNSS @todo Future stuff
    if (active_vehicle_config.enable_datasave_gnss_data){}
    
    // Voltage divider
    if (active_vehicle_config.enable_datasave_v_divider_data)
    {
        _convert_var_to_string(Sensors.system_voltage, 5, 2);
    }

    // Mag @todo Future stuff
    if (active_vehicle_config.enable_datasave_mag_data){}

    /* Profiler */
    if (active_vehicle_config.enable_datasave_profiler_sensors_loop)
    {
        _convert_var_to_string(Sensors.profiler_imu_loop, 6, 2);
        _convert_var_to_string(Sensors.profiler_baro_loop, 6, 2);
        _convert_var_to_string(Sensors.profiler_voltage_divider_loop, 6, 2);
        _convert_var_to_string(Sensors.profiler_pyro_loop, 6, 2);

        // @todo gnss, mag
    } 
    
    if (active_vehicle_config.enable_datasave_profiler_sensors_duration)
    {
        _convert_var_to_string(Sensors.profiler_imu_function_duration, 6, 2);
        _convert_var_to_string(Sensors.profiler_baro_function_duration, 6, 2);
        _convert_var_to_string(Sensors.profiler_voltage_divider_function_duration, 6, 2);
        // @todo gnss, mag
    }

    end_result_inst = _recorder_string_buffer;

    // Prevents crash by returning if size < 3 char
    if (end_result_inst.length() < 3) return;

    // Removes last 2 chars from the end of the string. This is needed to have correct .csv string
    end_result_inst = end_result_inst.substring(0, end_result_inst.length() - 2);
}

void _recorder_create_csv_layout(String &layout_inst)
{
    _recorder_string_buffer[0] = 0;

    /* General */
    if (active_vehicle_config.enable_datasave_general)
    {
        _convert_var_to_string_wo_coma("VOT (Sec)");
        _convert_var_to_string_wo_coma("MT (Sec)");
        _convert_var_to_string_wo_coma("State");
        //_convert_var_to_string_wo_coma();
        //_convert_var_to_string_wo_coma();

        // @todo mission state, mass (calulate mass based on acceleratinon), thrust
    }

    /* Pyro */
    if (active_vehicle_config.enable_datasave_pyro)
    {
        _convert_var_to_string_wo_coma("Pyro 1 Fire");
        _convert_var_to_string_wo_coma("Pyro 1 Voltage");
        _convert_var_to_string_wo_coma("Pyro 2 Fire");
        _convert_var_to_string_wo_coma("Pyro 2 Voltage");
    }

    /* Sensors */
    if (active_vehicle_config.enable_datasave_imu_data)
    {
        _convert_var_to_string_wo_coma("Accel X");
        _convert_var_to_string_wo_coma("Accel Y");
        _convert_var_to_string_wo_coma("Accel Z");
        _convert_var_to_string_wo_coma("Gyro Vel X");
        _convert_var_to_string_wo_coma("Gyro Vel Y");
        _convert_var_to_string_wo_coma("Gyro Vel Z");
        _convert_var_to_string_wo_coma("IMU Temp");


        // @todo filtered accel, gyro ang vel and ori, total velocity.
    }

    // Baro
    if (active_vehicle_config.enable_datasave_baro_data)
    {
        _convert_var_to_string_wo_coma("Baro Alt (RAW)");
        _convert_var_to_string_wo_coma("Baro Alt (RAW & W.O Bias)");
        _convert_var_to_string_wo_coma("Air Pressure");
        _convert_var_to_string_wo_coma("Baro Temp");
        _convert_var_to_string_wo_coma("Altitude");
        _convert_var_to_string_wo_coma("Velocity");

        // @todo filtered altitude
    }

    // GNSS @todo Future stuff
    if (active_vehicle_config.enable_datasave_gnss_data){}
    
    // Voltage divider
    if (active_vehicle_config.enable_datasave_v_divider_data)
    {
        _convert_var_to_string_wo_coma("Voltage (V)");
    }

    // Mag @todo Future stuff
    if (active_vehicle_config.enable_datasave_mag_data){}


    /* Profiler */
    if (active_vehicle_config.enable_datasave_profiler_sensors_loop)
    {
        _convert_var_to_string_wo_coma("PF IMU Loop");
        _convert_var_to_string_wo_coma("PF Baro Loop");
        _convert_var_to_string_wo_coma("PF Voltage Divider Loop");
        _convert_var_to_string_wo_coma("PF Pyro Loop");

        // @todo gnss, mag
    } 
    
    if (active_vehicle_config.enable_datasave_profiler_sensors_duration)
    {
        _convert_var_to_string_wo_coma("PF IMU Func Dur");
        _convert_var_to_string_wo_coma("PF Baro Func Dur");
        _convert_var_to_string_wo_coma("PF Voltage Divider Func Dur");
        // @todo gnss, mag
    }

    // Set the buffer to string
    layout_inst = _recorder_string_buffer;
}









void _convert_var_to_string_wo_coma(String data)
{
    strcat(_recorder_string_buffer, data.c_str());
}

void _convert_var_to_string(String data)
{
    strcat(_recorder_string_buffer, data.c_str());
    strcat(_recorder_string_buffer, ", ");
}

void _convert_var_to_string(float data, int min_width, int decimal_place)
{
    if (min_width > 9) return;
    dtostrf(data, min_width, decimal_place, _recorder_char_buffer);
    strcat(_recorder_string_buffer, _recorder_char_buffer);
    strcat(_recorder_string_buffer, ", ");
}

void _convert_var_to_string(int data)
{
    itoa(data, _recorder_char_buffer, 10);
    strcat(_recorder_string_buffer, _recorder_char_buffer);
    strcat(_recorder_string_buffer, ", ");
}

int recorder_create_file(String path, String format)
{
    return recorder_flash_instance.create_file(path, format);
}

int recorder_open_file(String path, oflag_t oFlag)
{
    return recorder_flash_instance.open_file(path, oFlag);
}

int recorder_delete_file(String path)
{
    return recorder_flash_instance.remove_file(path);
}

void recorder_print_file_content(String path)
{
    recorder_flash_instance.read_and_display_all_content(path);
}

void _recorder_log_to_flash(String end_result_inst)
{
    recorder_flash_instance.write_file_line(end_result_inst);
}

void recorder_close_file()
{
    recorder_flash_instance.close_file();
}
