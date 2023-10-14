#ifndef __DATA_RECORDER_H__
#define __DATA_RECORDER_H__

#include <Arduino.h>
#include <definitions.h>
#include <flash.h>

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

// @brief Ensures that contents inside recorder_update run at correct frequency
extern uint64_t _recorder_serial_prev, _recorder_flash_prev;

// @brief Internal variable to be set by the set_recorder_..._update_interval function
// @note (DO NOT MANUALLY UPDATE)
extern uint64_t _recorder_flash_update_interval, _recorder_serial_update_interval;



// @brief Set the flash chip log rate. Interval must be in microseconds (1s = 1,000,000 ms)
void set_recorder_flash_update_interval(uint64_t interval = 10^6);

// @brief Set the serial print rate. Interval must be in microseconds (1s = 1,000,000 ms)
void set_recorder_serial_update_interval(uint64_t interval = 10^6);

// @brief Begins the flash chip and the filesystem on it. Returns false if failed. True if succeeded
int recorder_begin();

// @brief General function for logging / printing data to flash / serial
void recorder_update();

// @brief Prints all alerts onto serial
void cast_all_notifications_to_serial();

// @brief Special update function for flash chip
void _flash_update();

// @brief Special update function for serial casting
void _serial_update();

// @brief Converts all flight telemetry onto a string 
void _recorder_convert_data_to_string(String &end_result_inst);

// @brief Creates a file on flash
int recorder_create_file(String path, String format);

// @brief Opens a pre-existing file
// @note Returns 0 if file doesn't exist
int recorder_open_file(String path, oflag_t oFlag);

// @brief Removes the file on given path
int recorder_delete_file(String path);

// @brief Casts all content of a file to serial
// @note This doesn't delete the file, only reads it
void recorder_print_file_content(String path);

// @brief Saves given string to flash
void _recorder_log_to_flash(String end_result_inst);

// @brief Closes the currently open file
void recorder_close_file();

#endif