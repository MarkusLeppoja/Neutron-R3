#include "coms.h"


const char start_indicator = '<', end_indicator = '>';
boolean is_receiving_in_process;
char _serial_char_buffer[128];
int _serial_buffer_index;
int _received_command;
String _temp_conversion_string;
Alerts coms_alerts("Communication");


void update_communication()
{
  // Return if coms isn't enabled
  if (!get_active_config().enable_coms) return;

  if (Serial.available() < 0) return;

  char rec_char = Serial.read();

  switch (rec_char)
  {
  case start_indicator:
    is_receiving_in_process = true;
    _serial_char_buffer[0] = 0;
  break;
  case end_indicator:
    is_receiving_in_process = false;
    _serial_buffer_index = 0;

    // Convert message to a (int) command
    _received_command = atoi(_serial_char_buffer);
    
    // Command
    _execute_command_from_list(_received_command);

    // Echo message if true
    if (get_active_config().enable_coms_message_echo) 
    {
      Serial.println(_received_command);
    }
  break;
  default:
    // Receive and add inc char to buffer
    if (!is_receiving_in_process) return;

    // Prevent overflow
    if (_serial_buffer_index > 127){
      is_receiving_in_process = true;
      _serial_char_buffer[0] = 0;
      _serial_buffer_index = 0;
    }

    _serial_char_buffer[_serial_buffer_index] = rec_char;
    _serial_buffer_index++;
  break;
  }
}

void _execute_command_from_list(int command)
{
  String temp_string;

  //add_special_indicator_to_queue(e_event_options::event_command_received);
  
  switch (command)
  {
  case 0:
  break;  
    
  default:
  
  break;
  }
}