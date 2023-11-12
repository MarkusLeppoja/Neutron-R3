#include "data_recorder.h"

//TODO: REPLACE ALL SERIAL.PRINT WITH PRINTF

// Instances
flash_device recorder_flash_instance;
uint64_t _recorder_serial_prev, _recorder_flash_prev;
uint64_t _recorder_flash_update_interval = 1000000, _recorder_serial_update_interval = 1000000;
String _recorder_file_0_log_string, _recorder_file_1_log_string;
String _recorder_serial_prev_notification;
String _flash_temp_log_string, _serial_temp_log_string;



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
    return recorder_flash_instance.init(get_active_config().enable_flash_log);
}

void update_recorder()
{
    update_mcu_clock();
    if (Clock.microseconds - _recorder_flash_prev >= _recorder_flash_update_interval && get_active_config().enable_flash_log)
    {
        _recorder_flash_prev = Clock.microseconds;
        _flash_update();
    }

    update_mcu_clock();
    if (Clock.microseconds - _recorder_serial_prev >= _recorder_serial_update_interval && get_active_config().enable_serial_stream)
    {
        _recorder_serial_prev = Clock.microseconds;
        _serial_update();
    }
}

void _flash_update()
{
    // Don't try to log data if no data file is open or the flash chip isn't usable
    if (!recorder_flash_instance.get_flash_chip_usability_status() || !recorder_flash_instance.is_file_open()) return;
    
    if (get_active_config().enable_flash_notification_log && get_alert_buffer_size() != 0)
    {
        _flash_temp_log_string = "";

        // Load the buffer onto a string and clear it
        for (uint8_t i = 0; i < get_alert_buffer_size(); i++)
        {
            _flash_temp_log_string += _alert_buffer[i];
        }

        // Reset buffer & its size
        clear_alert_buffer();
        
        // Log the string to flash
        _recorder_log_to_flash(_flash_temp_log_string);
    }

    // Log telemetry to flash if functionality is enabled
    if (get_active_config().enable_flash_telemetry_log)
    {
        _flash_temp_log_string = "";
        
        // Convert all data to a string
        _recorder_convert_data_to_string(_flash_temp_log_string);

        // Log the string to flash
        _recorder_log_to_flash(_flash_temp_log_string);
    }
}

void cast_all_notifications_to_serial()
{
    // Check if alert buffer contains an alert and if flash notification logging is toggled
    if (!get_active_config().enable_serial_notification_stream || !get_active_config().enable_serial_stream) return;

    // Cast the notification string
    Serial.printf(get_all_alerts().c_str());
    Serial.println();

}

void _serial_update()
{
    // Check if serial telemetry stream functionality is enabled. 
    if (get_active_config().enable_serial_telemetry_stream)
    {
        //_serial_temp_log_string = "";

        // Convert all data to a string
        _recorder_convert_data_to_string(_serial_temp_log_string);

        // Cast string over serial
        Serial.printf(_serial_temp_log_string.c_str());
        Serial.println();
    }

    if (get_active_config().enable_serial_notification_stream)
    {
        //_serial_temp_log_string = "";

        _serial_temp_log_string = get_most_recent_alert();

        if (_serial_temp_log_string == _recorder_serial_prev_notification) return;
        _recorder_serial_prev_notification = _serial_temp_log_string;

        // Cast new notification to serial
        Serial.printf(_recorder_serial_prev_notification.c_str());
    }
}



void _recorder_convert_data_to_string(String &end_result_inst)
{
    // Reset string
    //_recorder_file_0_log_string = "";
    String sfsa;
//TODO: REPLACE ALL CONVERTS WITH SERIAL PRINT F AND TEST THE PERFORMANCE IMPACT
    /* General */
    if (get_active_config().enable_datasave_basic)
    {
        sfsa = String(Clock.seconds) + ", ";
        sfsa += String(Clock.mission_duration) + ", ";
        sfsa += String(get_mission_state()) + ", ";
        /*_convert_var_to_string(Clock.seconds, 6, 2);
        _convert_var_to_string(Clock.mission_duration, 6, 2);
        _convert_var_to_string(get_mission_state());*/
    }

    /* Pyro */
    if (get_active_config().enable_datasave_pyro)
    {
        _convert_var_to_string(Sensors.pyro_1_fire_status);
        _convert_var_to_string(Sensors.pyro_1_voltage, 4, 2);
        _convert_var_to_string(Sensors.pyro_2_fire_status);
        _convert_var_to_string(Sensors.pyro_2_voltage, 4, 2);
    }

    /* Sensors */
    if (get_active_config().enable_datasave_imu_data)
    {
        sfsa += String(Sensors.acceleration.x) + ", ";
        sfsa += String(Sensors.acceleration.y) + ", ";
        sfsa += String(Sensors.acceleration.z) + ", ";
        sfsa += String(Sensors.gyro_velocity_dps.x) + ", ";
        sfsa += String(Sensors.gyro_velocity_dps.y) + ", ";
        sfsa += String(Sensors.gyro_velocity_dps.z) + ", ";
        sfsa += String(Sensors.raw_acceleometer_temperature) + ", ";
        sfsa += String(Sensors.orientation.x) + ", ";
        sfsa += String(Sensors.orientation.y) + ", ";
        sfsa += String(Sensors.orientation.z) + ", ";

        /*_convert_var_to_string(Sensors.acceleration.x, 6, 2);
        _convert_var_to_string(Sensors.acceleration.y, 6, 2);
        _convert_var_to_string(Sensors.acceleration.z, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity_dps.x, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity_dps.y, 6, 2);
        _convert_var_to_string(Sensors.gyro_velocity_dps.z, 6, 2);
        _convert_var_to_string(Sensors.raw_acceleometer_temperature, 5, 2);
        _convert_var_to_string(Sensors.orientation.x, 5, 2);
        _convert_var_to_string(Sensors.orientation.y, 5, 2);
        _convert_var_to_string(Sensors.orientation.z, 5, 2);*/
    }

    // Baro
    if (get_active_config().enable_datasave_baro_data)
    {
        sfsa += String(Sensors.raw_baro_altitude) + ", ";
        sfsa += String(Sensors.raw_baro_altitude_wo_bias) + ", ";
        sfsa += String(Sensors.raw_baro_pressure) + ", ";
        sfsa += String(Sensors.raw_baro_temperature) + ", ";
        sfsa += String(Sensors.position.z) + ", ";
        sfsa += String(Sensors.velocity.z) + ", ";
        /*_convert_var_to_string(Sensors.raw_baro_altitude, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_altitude_wo_bias, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_pressure, 7, 2);
        _convert_var_to_string(Sensors.raw_baro_temperature, 5, 2);
        _convert_var_to_string(Sensors.position.z, 7, 2);
        _convert_var_to_string(Sensors.velocity.z, 6, 2);*/
    }
    
    // Voltage divider
    if (get_active_config().enable_datasave_v_divider_data)
    {
        sfsa += String(Sensors.internal_voltage) + ", ";

        //_convert_var_to_string(Sensors.internal_voltage, 5, 2);
    }

    if (get_active_config().enable_datasave_performance)
    {
        sfsa += String(Code_performance.cpu_clock_frequency) + ", ";
        sfsa += String(Code_performance.cpu_used_heap) + ", ";
        sfsa += String(Code_performance.cpu_free_heap) + ", ";
        sfsa += String(Code_performance.imu_loop) + ", ";
        sfsa += String(Code_performance.baro_loop) + ", ";
        sfsa += String(Code_performance.voltage_divider_loop) + ", ";
        sfsa += String(Code_performance.pyro_loop) + ", ";
        sfsa += String(Code_performance.imu_function_duration) + ", ";
        sfsa += String(Code_performance.baro_function_duration) + ", ";
        sfsa += String(Code_performance.voltage_divider_function_duration) + ", ";
        sfsa += String(Code_performance.pyro_function_duration);

        /*_convert_var_to_string(Code_performance.cpu_clock_frequency);
        _convert_var_to_string(Code_performance.cpu_used_heap);
        _convert_var_to_string(Code_performance.cpu_free_heap);
        _convert_var_to_string(Code_performance.imu_loop, 7, 2);
        _convert_var_to_string(Code_performance.baro_loop, 7, 2);
        _convert_var_to_string(Code_performance.voltage_divider_loop, 7, 2);
        _convert_var_to_string(Code_performance.pyro_loop, 7, 2);
        _convert_var_to_string(Code_performance.imu_function_duration, 7, 2);
        _convert_var_to_string(Code_performance.baro_function_duration, 7, 2);
        _convert_var_to_string(Code_performance.voltage_divider_function_duration, 7, 2);
        _convert_var_to_string(Code_performance.pyro_function_duration, 7, 2);*/
    }

    end_result_inst = sfsa;
    //_recorder_file_0_log_string = "";
    // Prevents crash by returning if size < 3 char
    /*if (end_result_inst.length() < 3) return;

    // Removes last 2 chars from the end of the string. This is needed to have correct .csv string
    end_result_inst = end_result_inst.substring(0, end_result_inst.length() - 2);*/
}

//TODO: Re-do this based on upper
void _recorder_create_csv_layout(String &layout_inst)
{
    //_recorder_string_buffer[0] = 0;

    // General 
    if (get_active_config().enable_datasave_basic)
    {
        _convert_var_to_string("VOT (s)");
        _convert_var_to_string("MT (s)");
        _convert_var_to_string("State");
    }

    // Pyro 
    if (get_active_config().enable_datasave_pyro)
    {
        _convert_var_to_string("Pyro 1 Fire");
        _convert_var_to_string("Pyro 1 Voltage (V)");
        _convert_var_to_string("Pyro 2 Fire");
        _convert_var_to_string("Pyro 2 Voltage (V)");
    }

    // Sensors
    if (get_active_config().enable_datasave_imu_data)
    {
        _convert_var_to_string("Accel X (m/s^2)");
        _convert_var_to_string("Accel Y (m/s^2)");
        _convert_var_to_string("Accel Z (m/s^2)");
        _convert_var_to_string("Gyro Vel X (dps)");
        _convert_var_to_string("Gyro Vel Y (dps)");
        _convert_var_to_string("Gyro Vel Z (dps)");
        _convert_var_to_string("IMU Temp (*C)");
        _convert_var_to_string("Ori X (deg)");
        _convert_var_to_string("Ori Y (deg)");
        _convert_var_to_string("Ori Z (deg)");
    }

    // Baro
    if (get_active_config().enable_datasave_baro_data)
    {
        _convert_var_to_string("Baro Alt Raw (m)");
        _convert_var_to_string("Baro Alt Raw & WO Bias (m)");
        _convert_var_to_string("Air Pressure (hPa)");
        _convert_var_to_string("Baro Temp (*C)");
        _convert_var_to_string("Position Z (m)");
        _convert_var_to_string("Velocity Z (m/s)");
    }

    // Voltage divider
    if (get_active_config().enable_datasave_v_divider_data)
    {
        _convert_var_to_string("Voltage");
    }
    
    // Code performance
    if (get_active_config().enable_datasave_performance)
    {
        _convert_var_to_string("CPU Freq");
        _convert_var_to_string("Used Memory");
        _convert_var_to_string("Free Memory");
        _convert_var_to_string("IMU Loop (ms)");
        _convert_var_to_string("Baro Loop (ms)");
        _convert_var_to_string("Voltage Divider Loop (ms)");
        _convert_var_to_string("Pyro Loop (ms)");
        _convert_var_to_string("IMU Func Dur (ms)");
        _convert_var_to_string("Baro Func Dur (ms)");
        _convert_var_to_string("Voltage Divider Func Dur (ms)");
        _convert_var_to_string_wo_coma("Pyro Func Dur (ms)");
    }

    // Save the buffer to string
    //layout_inst = _recorder_file_0_log_string;
}



void _convert_var_to_string_wo_coma(String data)
{
    _recorder_file_0_log_string += String(data);
}

void _convert_var_to_string(String data)
{
    _recorder_file_0_log_string += String(data) + ", ";
}

void _convert_var_to_string(float data, int min_width, int decimal_place)
{
    _recorder_file_0_log_string += String(data) + ", ";
}

void _convert_var_to_string(int data)
{
    _recorder_file_0_log_string += String(data) + ", ";
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

int recorder_check_if_file_exists(String path)
{
    return recorder_flash_instance.does_file_exist(path);
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

void recorder_earase_flash_chip()
{
    recorder_flash_instance.erase_chip();

}