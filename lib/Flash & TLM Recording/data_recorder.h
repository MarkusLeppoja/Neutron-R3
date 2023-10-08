#ifndef __DATA_RECORDER_H__
#define __DATA_RECORDER_H__

#include <Arduino.h>
#include <vehicle_configs.h>
#include "flash.h"

/*
    Init flash and open a file during startup. Only write notification data to it until (boolean) enable_flight_telemetry_recording is true.

    user controlled:
    enable_flash_chip - This controls if datalogging happens or not.
    enable_flight_telemetry_recording - For recording flight data to flash chip. Toggeled on after GROUND LOCK state ends and disabled during LANDED / ERROR state
    enable_system_notification_recording - For recording event data to flash chip. 

    software controlled:
    sw_flash_chip_usage - If true then flash chip can be used.

    TODO:
    recorder_begin - setup flash, create file, log test string
    recorder_update - check state (mission), check if logging enabled (tlm || notification), log based on set speed, print to serial if enabled.
    recorder_delete_file
    recorder_print_file_content
    _recorder_convert_data_to_flash_string
    _recorder_log_to_flash
*/

extern uint64_t _recorder_serial_prev, _recorder_flash_prev;

extern uint64_t _recorder_flash_update_interval, _recorder_serial_update_interval;

void set_recorder_flash_update_interval(uint64_t interval);
void set_recorder_serial_update_interval(uint64_t interval);

#include "data_recorder.h"

flash_device recorder_flash_instance;

uint64_t _recorder_micros_prev;

void set_recorder_flash_update_interval(uint64_t interval)
{
    _recorder_flash_update_interval = interval;
}

void set_recorder_serial_update_interval(uint64_t interval)
{
    _recorder_serial_update_interval = interval;
}

int recorder_begin(boolean enable_flash_capture)
{
    return recorder_flash_instance.init(enable_flash_capture);
}

void recorder_update()
{
    update_mcu_clock();

    if (Clock.microseconds - _recorder_flash_prev >= _recorder_flash_update_interval &&)





    //if (Clock.microseconds - _recorder_micros_prev <= recording_speed_or_smt)

    if (!recorder_flash_instance.get_flash_chip_usability_status()) return;

}

int recorder_delete_file(String path)
{
    return recorder_flash_instance.remove_file(path);
}

void recorder_print_file_content(String path)
{
    recorder_flash_instance.read_and_display_all_content(path);
}

void _recorder_convert_data_to_flash_string(String &end_result_inst)
{


}
    
void _recorder_log_to_flash(String end_result_inst)
{
    recorder_flash_instance.write_file_line(end_result_inst);
}


#endif