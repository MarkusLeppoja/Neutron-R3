#include "coms.h"
//#include "filters.h"
#include "state_estimation.h"

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
  if (!active_vehicle_config.enable_coms) return;

  if (Serial.available() < 0) return;

  char rec_char = Serial.read();

  switch (rec_char)
  {
  case start_indicator:
    _serial_char_buffer[0] = 0;
    is_receiving_in_process = true;
  break;
  case end_indicator:
    is_receiving_in_process = false;
    _serial_buffer_index = 0;

    // Convert message to a (int) command
    _received_command = atoi(_serial_char_buffer);
    _serial_char_buffer[0] = 0;
    
    // Command
    _execute_command_from_list(_received_command);

    // Echo message if true
    if (active_vehicle_config.enable_coms_message_echo) 
    {
      cast_message_to_serial(String(_received_command));
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

String temp_string;
void _execute_command_from_list(int command)
{

  add_special_indicator_to_queue(e_event_options::event_command_received);
  
  switch (command)
    {
    case 1:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_sensors status to " + String(!active_vehicle_config.enable_sensors));
      active_vehicle_config.enable_sensors = !active_vehicle_config.enable_sensors;
      break;
    case 2:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_imu status to " + String(!active_vehicle_config.enable_imu));
      active_vehicle_config.enable_imu = !active_vehicle_config.enable_imu;
      break;
    case 3:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_baro status to " + String(!active_vehicle_config.enable_baro));
      active_vehicle_config.enable_baro = !active_vehicle_config.enable_baro;
      break;
    case 4:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_voltage_divider status to " + String(!active_vehicle_config.enable_voltage_divider));
      active_vehicle_config.enable_voltage_divider = !active_vehicle_config.enable_voltage_divider;
      break;
    case 5:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_recovery status to " + String(!active_vehicle_config.enable_recovery));
      active_vehicle_config.enable_recovery = !active_vehicle_config.enable_recovery;
      break;
    case 6:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_servo status to " + String(!active_vehicle_config.enable_servo));
      active_vehicle_config.enable_servo = !active_vehicle_config.enable_servo;
      break;
    case 7:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_pyro status to " + String(!active_vehicle_config.enable_pyro));
      active_vehicle_config.enable_pyro = !active_vehicle_config.enable_pyro;
      break;
    case 8:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_log status to " + String(!active_vehicle_config.enable_flash_log));
      active_vehicle_config.enable_flash_log = !active_vehicle_config.enable_flash_log;
      break;
    case 9:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_notification_log status to " + String(!active_vehicle_config.enable_flash_notification_log));
      active_vehicle_config.enable_flash_notification_log = !active_vehicle_config.enable_flash_notification_log;
      break;
    case 10:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_file_0_log status to " + String(!active_vehicle_config.enable_flash_file_0_log));
      active_vehicle_config.enable_flash_file_0_log = !active_vehicle_config.enable_flash_file_0_log;
      break;
    case 11:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_file_1_log status to " + String(!active_vehicle_config.enable_flash_file_1_log));
      active_vehicle_config.enable_flash_file_1_log = !active_vehicle_config.enable_flash_file_1_log;
      break;
    case 12:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_stream status to " + String(!active_vehicle_config.enable_serial_stream));
      active_vehicle_config.enable_serial_stream = !active_vehicle_config.enable_serial_stream;
      break;
    case 13:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_notification_stream status to " + String(!active_vehicle_config.enable_serial_notification_stream));
      active_vehicle_config.enable_serial_notification_stream = !active_vehicle_config.enable_serial_notification_stream;
      break;
    case 14:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_file_0_stream status to " + String(!active_vehicle_config.enable_serial_file_0_stream));
      active_vehicle_config.enable_serial_file_0_stream = !active_vehicle_config.enable_serial_file_0_stream;
      break;
    case 15:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_file_1_stream status to " + String(!active_vehicle_config.enable_serial_file_1_stream));
      active_vehicle_config.enable_serial_file_1_stream = !active_vehicle_config.enable_serial_file_1_stream;
      break;
    case 16:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_basic status to " + String(!active_vehicle_config.enable_datasave_basic));
      active_vehicle_config.enable_datasave_basic = !active_vehicle_config.enable_datasave_basic;
      break;
    case 17:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_pyro status to " + String(!active_vehicle_config.enable_datasave_pyro));
      active_vehicle_config.enable_datasave_pyro = !active_vehicle_config.enable_datasave_pyro;
      break;
    case 18:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_imu_data status to " + String(!active_vehicle_config.enable_datasave_imu_data));
      active_vehicle_config.enable_datasave_imu_data = !active_vehicle_config.enable_datasave_imu_data;
      break;
    case 19:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_baro_data status to " + String(!active_vehicle_config.enable_datasave_baro_data));
      active_vehicle_config.enable_datasave_baro_data = !active_vehicle_config.enable_datasave_baro_data;
      break;
    case 20:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_v_divider_data status to " + String(!active_vehicle_config.enable_datasave_v_divider_data));
      active_vehicle_config.enable_datasave_v_divider_data = !active_vehicle_config.enable_datasave_v_divider_data;
      break;
    case 21:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_performance status to " + String(!active_vehicle_config.enable_datasave_performance));
      active_vehicle_config.enable_datasave_performance = !active_vehicle_config.enable_datasave_performance;
      break;
    case 22:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_status_indicator status to " + String(!active_vehicle_config.enable_status_indicator));
      active_vehicle_config.enable_status_indicator = !active_vehicle_config.enable_status_indicator;
      break;
    case 23:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_led status to " + String(!active_vehicle_config.enable_led));
      active_vehicle_config.enable_led = !active_vehicle_config.enable_led;
      break;
    case 24:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_buzzer status to " + String(!active_vehicle_config.enable_buzzer));
      active_vehicle_config.enable_buzzer = !active_vehicle_config.enable_buzzer;
      break;
    case 25:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_coms status to " + String(!active_vehicle_config.enable_coms));
      active_vehicle_config.enable_coms = !active_vehicle_config.enable_coms;
      break;
    case 26:
      coms_alerts.create_alert(e_alert_type::alert, "Set enable_coms_message_echo status to " + String(!active_vehicle_config.enable_coms_message_echo));
      active_vehicle_config.enable_coms_message_echo = !active_vehicle_config.enable_coms_message_echo;
      break;
    case 27:
      coms_alerts.create_alert(e_alert_type::alert, "Set sw_begin_pad_idle_ground_lock_exit_countdown status to " + String(!Booleans.sw_begin_pad_idle_ground_lock_exit_countdown));
      Booleans.sw_begin_pad_idle_ground_lock_exit_countdown = !Booleans.sw_begin_pad_idle_ground_lock_exit_countdown;
      break;
    case 28:
      //coms_alerts.create_alert(e_alert_type::alert, "Set  status to " + String(!active_vehicle_config.));
      //active_vehicle_config. = !active_vehicle_config.;
      break;
    case 29:

      break;
    case 30:

      break;
    case 31:

      break;
    case 32:

      break;
    case 33:
          
      break;
    case 34:
          
      break;
    case 35:
          
      break;
    case 36:
          
      break;
    case 37:
          
      break;
    case 38:
          
      break;
    case 39:
          
      break;
    case 40:
          
      break;
    case 41:
          
      break;
    case 42:
          
      break;
    case 43:
          
      break;
    case 44:
          
      break;
    case 45:
          
      break;
    case 46:
          
      break;
    case 47:
          
      break;
    case 48:
          
      break;
    case 49:
          
      break;
    case 50:

      break;
    case 51:
      coms_alerts.create_alert(e_alert_type::alert, "Attempting to open all 3 flash files");
      open_all_flash_files();
      break;
    case 52:
      coms_alerts.create_alert(e_alert_type::alert, "Attempting to close all 3 flash files");
      close_all_flash_files();
      break;
    case 53:
      coms_alerts.create_alert(e_alert_type::alert, "Attempting to delete all 3 flash files");
      delete_all_flash_files();
      break;
    case 54:
      coms_alerts.create_alert(e_alert_type::alert, "Casting all alerts to serial");
      cast_message_to_serial(get_all_alerts());
      break;
    case 55:
      coms_alerts.create_alert(e_alert_type::alert, "Casting latest alert to serial");
      cast_message_to_serial(get_most_recent_alert());
      break;
    case 56:
      coms_alerts.create_alert(e_alert_type::alert, "Fireing pyro channel nr 1");
      pyro_1_fire();
      break;
    case 57:
      coms_alerts.create_alert(e_alert_type::alert, "Fireing pyro channel nr 2");
      pyro_2_fire();
      break;
    case 58:
      coms_alerts.create_alert(e_alert_type::alert, "Lock recovery mechanism");
      recovery_arm();
      break;
    case 59:
      coms_alerts.create_alert(e_alert_type::alert, "Deploy recovery mechanism");
      recovery_deploy_parachute();
      break;
    case 60:
      coms_alerts.create_alert(e_alert_type::alert, "Reset orientation");
      reset_ori();
      break;
    case 61:
      coms_alerts.create_alert(e_alert_type::alert, "Reset position & velocity");
      reset_position_kalman();
      break;
    case 62:
      coms_alerts.create_alert(e_alert_type::alert, "Enable Baro calibration");
      baro_enable_calibration();
      break;
    case 63:
      coms_alerts.create_alert(e_alert_type::alert, "Enable IMU calibration");
      imu_enable_calibration();
      break;
    case 64:
      coms_alerts.create_alert(e_alert_type::alert, "Remove first special indicator from queue");
      remove_latest_special_indicator_from_queue();
      break;
    case 65:
      coms_alerts.create_alert(e_alert_type::alert, "Cast file 0 data to serial");
      cast_file_0_data();
      break;
    case 66:
      coms_alerts.create_alert(e_alert_type::alert, "Cast file 1 data to serial");
      cast_file_1_data();
      break;
    case 67:
      //coms_alerts.create_alert(e_alert_type::alert, "");
      break;

    default: break;
  }
}