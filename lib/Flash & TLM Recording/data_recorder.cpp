#include "data_recorder.h"

// Instance
flash_device recorder_flash_instance;

// Other variables used in code. (DO NOT MANUALLY UPDATE)
uint64_t _recorder_serial_prev, _recorder_flash_prev;
uint64_t _recorder_flash_update_interval, _recorder_serial_update_interval;



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

void recorder_update()
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
    // Return if flash chip functionality isn't enabled
    if (!recorder_flash_instance.get_flash_chip_usability_status()) return;
    
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
    if (active_vehicle_config.enable_serial_telemetry_stream)
    {
        // Temp string instance
        String _serial_log_string;

        // Convert all data to a string
        _recorder_convert_data_to_string(_serial_log_string);

        // Cast string over serial
        Serial.println(_serial_log_string);
    }
}

void _recorder_convert_data_to_string(String &end_result_inst)
{
    // TODO: Time (milliseconds)
    /*
     * Time (millis)
     * Mission time
     * Mission state
     * sensor data
     * pyro stuff
     * 
     * optional:
     * all sensor profiler stuff
    */
   //TODO: test how big the file is when all optional things are enabled, prevent overflow and ensure all data gets sent / delivered to flash / serial



    end_result_inst = "this is a nice empty end result that is supposed to be replaced with actual telemetry which comes from the sensors and other orgins and they all get collected to definitions file where it is taken and put here.";
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
