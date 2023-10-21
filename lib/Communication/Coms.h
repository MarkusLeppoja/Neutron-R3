#ifndef __COMS_H__
#define __COMS_H__

#include <Arduino.h>
#include <definitions.h>
#include <alerts.h>



extern const char start_indicator, end_indicator;
extern boolean is_receiving_in_process;
extern char _serial_char_buffer[];
extern int _serial_buffer_index;
extern int _received_command;
extern String _temp_conversion_string;



void update_communication();
void _execute_command_from_list(int command);

#endif