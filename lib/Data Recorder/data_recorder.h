#ifndef __DATA_RECORDER_H__
#define __DATA_RECORDER_H__

#include <Arduino.h>
#include <definitions.h>
#include <profiler.h>

void set_flash_log_interval(uint64_t interval, int file_count);
void set_serial_log_interval(uint64_t interval, int file_count);

int recorder_begin();
void update_recorder();

void _flash_update();
void _serial_update();

void _file_0_conversion_logic();
void _file_0_conversion_layout();

void _file_1_conversion_logic();
void _file_1_conversion_layout();

void cast_alert_to_data_recorder(String alert);

void open_all_flash_files();
void close_all_flash_files();
void delete_all_flash_files();

void cast_message_to_serial(String message);

void cast_file_0_data();
void cast_file_1_data();
void cast_file_2_data();

void check_if_3_files_exists();

#endif