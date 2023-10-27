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
  if (!active_vehicle_config.enable_coms) return;

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
    if (active_vehicle_config.enable_coms_message_echo) 
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

  add_special_indicator_to_queue(e_event_options::event_command_received);
  
  switch (command)
  {
  case 0:
    return;
  break;
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
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_gnss status to " + String(!active_vehicle_config.enable_gnss));
    active_vehicle_config.enable_gnss = !active_vehicle_config.enable_gnss;
  break;
  case 5:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_voltage_divider status to " + String(!active_vehicle_config.enable_voltage_divider));
    active_vehicle_config.enable_voltage_divider = !active_vehicle_config.enable_voltage_divider;
  break;
  case 6:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_mag status to " + String(!active_vehicle_config.enable_mag));
    active_vehicle_config.enable_mag = !active_vehicle_config.enable_mag;
  break;
  case 7:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_gnss_debug status to " + String(!active_vehicle_config.enable_gnss_debug));
    active_vehicle_config.enable_gnss_debug = !active_vehicle_config.enable_gnss_debug;
  break;
  case 8:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_fds_servo status to " + String(!active_vehicle_config.enable_fds_servo));
    active_vehicle_config.enable_fds_servo = !active_vehicle_config.enable_fds_servo;
  break;
  case 9:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_log status to " + String(!active_vehicle_config.enable_flash_log));
    active_vehicle_config.enable_flash_log = !active_vehicle_config.enable_flash_log;
  break;
  case 10:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_stream status to " + String(!active_vehicle_config.enable_serial_stream));
    active_vehicle_config.enable_serial_stream = !active_vehicle_config.enable_serial_stream;
  break;
  case 11:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_telemetry_log status to " + String(!active_vehicle_config.enable_flash_telemetry_log));
    active_vehicle_config.enable_flash_telemetry_log = !active_vehicle_config.enable_flash_telemetry_log;
  break;
  case 12:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_flash_notification_log status to " + String(!active_vehicle_config.enable_flash_notification_log));
    active_vehicle_config.enable_flash_notification_log = !active_vehicle_config.enable_flash_notification_log;
  break;
  case 13:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_telemetry_stream status to " + String(!active_vehicle_config.enable_serial_telemetry_stream));
    active_vehicle_config.enable_serial_telemetry_stream = !active_vehicle_config.enable_serial_telemetry_stream;
  break;
  case 14:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_serial_notification_stream status to " + String(!active_vehicle_config.enable_serial_notification_stream));
    active_vehicle_config.enable_serial_notification_stream = !active_vehicle_config.enable_serial_notification_stream;
  break;
  case 15:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_general status to " + String(!active_vehicle_config.enable_datasave_general));
    active_vehicle_config.enable_datasave_general = !active_vehicle_config.enable_datasave_general;
  break;
  case 16:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_pyro status to " + String(!active_vehicle_config.enable_datasave_pyro));
    active_vehicle_config.enable_datasave_pyro = !active_vehicle_config.enable_datasave_pyro;
  break;
  case 17:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_imu_data status to " + String(!active_vehicle_config.enable_datasave_imu_data));
    active_vehicle_config.enable_datasave_imu_data = !active_vehicle_config.enable_datasave_imu_data;
  break;
  case 18:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_baro_data status to " + String(!active_vehicle_config.enable_datasave_baro_data));
    active_vehicle_config.enable_datasave_baro_data = !active_vehicle_config.enable_datasave_baro_data;
  break;
  case 19:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_gnss_data status to " + String(!active_vehicle_config.enable_datasave_gnss_data));
    active_vehicle_config.enable_datasave_gnss_data = !active_vehicle_config.enable_datasave_gnss_data;
  break;
  case 20:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_v_divider_data status to " + String(!active_vehicle_config.enable_datasave_v_divider_data));
    active_vehicle_config.enable_datasave_v_divider_data = !active_vehicle_config.enable_datasave_v_divider_data;
  break;
  case 21:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_mag_data status to " + String(!active_vehicle_config.enable_datasave_mag_data));
    active_vehicle_config.enable_datasave_mag_data = !active_vehicle_config.enable_datasave_mag_data;
  break;
  case 22:
    coms_alerts.create_alert(e_alert_type::alert, "Beginning GROUND LOCK - PAD IDLE countdown");
    Booleans.sw_begin_pad_idle_ground_lock_exit_countdown = true;
  break;
  case 23:
    coms_alerts.create_alert(e_alert_type::alert, "Stopping GROUND LOCK - PAD IDLE countdown");
    Booleans.sw_begin_pad_idle_ground_lock_exit_countdown = false;
  break;
  case 24:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_profiler_sensors_loop status to " + String(!active_vehicle_config.enable_datasave_profiler_sensors_loop));
    active_vehicle_config.enable_datasave_profiler_sensors_loop = !active_vehicle_config.enable_datasave_profiler_sensors_loop;
  break;
  case 25:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_datasave_profiler_sensors_duration status to " + String(!active_vehicle_config.enable_datasave_profiler_sensors_duration));
    active_vehicle_config.enable_datasave_profiler_sensors_duration = !active_vehicle_config.enable_datasave_profiler_sensors_duration;
  break;
  case 26:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_led status to " + String(!active_vehicle_config.enable_led));
    active_vehicle_config.enable_led = !active_vehicle_config.enable_led;
  break;
  case 27:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_buzzer status to " + String(!active_vehicle_config.enable_buzzer));
    active_vehicle_config.enable_buzzer = !active_vehicle_config.enable_buzzer;
  break;
  case 28:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_indicator status to " + String(!active_vehicle_config.enable_indicator));
    active_vehicle_config.enable_indicator = !active_vehicle_config.enable_indicator;
  break;
  case 29:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_coms status to " + String(!active_vehicle_config.enable_coms));
    active_vehicle_config.enable_coms = !active_vehicle_config.enable_coms;
  break;
  case 30:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_coms_message_echo status to " + String(!active_vehicle_config.enable_coms_message_echo));
    active_vehicle_config.enable_coms_message_echo = !active_vehicle_config.enable_coms_message_echo;
  break;
  case 31:
    coms_alerts.create_alert(e_alert_type::alert, "Set enable_pyrotechnics status to " + String(!active_vehicle_config.enable_pyrotechnics));
    active_vehicle_config.enable_pyrotechnics = !active_vehicle_config.enable_pyrotechnics;
  break;
  case 32:
    coms_alerts.create_alert(e_alert_type::alert, "Set mission_state equal to ascent");
    active_mission_state = e_mission_state::ascent;

    // Begin faster logging
    set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_2);

    // Reset mission duration time
    Clock.mission_duration = 0;
    Clock.mission_begin_time = Clock.seconds;

    // Reset orientation
    reset_ori();
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
    coms_alerts.create_alert(e_alert_type::alert, "Casting current mission state to serial");
    Serial.println(String(get_mission_state()));
  break;
  case 52:
    coms_alerts.create_alert(e_alert_type::alert, "Set flash update mode to 0");
    set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_0);
  break;
  case 53:
    coms_alerts.create_alert(e_alert_type::alert, "Set flash update mode to 1");
    set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_1);
  break;
  case 54:
    coms_alerts.create_alert(e_alert_type::alert, "Set flash update mode to 2");
    set_recorder_flash_update_interval(active_vehicle_config.flash_log_interval_mode_2);
  break;
  case 55:
    coms_alerts.create_alert(e_alert_type::alert, "Set serial cast mode to 0");
    set_recorder_flash_update_interval(active_vehicle_config.serial_stream_interval_mode_0);
  break;
  case 56:
    coms_alerts.create_alert(e_alert_type::alert, "Set serial cast mode to 1");
    set_recorder_flash_update_interval(active_vehicle_config.serial_stream_interval_mode_1);
  break;
  case 57:
    coms_alerts.create_alert(e_alert_type::alert, "Set serial cast mode to 2");
    set_recorder_flash_update_interval(active_vehicle_config.serial_stream_interval_mode_2);
  break;
  case 58:
    coms_alerts.create_alert(e_alert_type::alert, "Casting all notifications to serial");
    cast_all_notifications_to_serial();
  break;
  case 59:
    coms_alerts.create_alert(e_alert_type::alert, "Printing .csv layout string to serial");
    _recorder_create_csv_layout(temp_string);
    Serial.println("<" + temp_string + ">");
  break;
  case 60:
    coms_alerts.create_alert(e_alert_type::alert, "Creating a file with AVC data and logging csv string");
    recorder_create_file(active_vehicle_config.flash_data_file_name, active_vehicle_config.flash_data_file_format);
    _recorder_create_csv_layout(temp_string);
    _recorder_log_to_flash(temp_string);
  break;
  case 61:
    coms_alerts.create_alert(e_alert_type::alert, "Opening file using AVC data. File is in READ ONLY mode");
    recorder_open_file(active_vehicle_config.flash_data_file_name + active_vehicle_config.flash_data_file_format, 0);
  break;
  case 62:
    coms_alerts.create_alert(e_alert_type::alert, "Deleting file with AVC data");
    recorder_delete_file(active_vehicle_config.flash_data_file_name + active_vehicle_config.flash_data_file_format);
  break;
  case 63:
    coms_alerts.create_alert(e_alert_type::alert, "Casting all content from flash chip to serial");
    recorder_print_file_content(active_vehicle_config.flash_data_file_name + active_vehicle_config.flash_data_file_format);
  break;
  case 64:
    coms_alerts.create_alert(e_alert_type::alert, "Closing currently open file on flash");
    recorder_close_file();
  break;
  case 65:
    coms_alerts.create_alert(e_alert_type::alert, "Beginning recorder");
    recorder_begin();;
  break;
  case 66:
    coms_alerts.create_alert(e_alert_type::alert, "Firing pyro channel 1");
    pyro_1_fire();
  break;
  case 67:
    coms_alerts.create_alert(e_alert_type::alert, "Firing pyro channel 2");
    pyro_2_fire();
  break;
  case 68:
    coms_alerts.create_alert(e_alert_type::alert, "Deactivating pyro channel 1");
    _pyro_1_deactivate();
  break;
  case 69:
    coms_alerts.create_alert(e_alert_type::alert, "Deactivating pyro channel 2");
    _pyro_2_deactivate();
  break;
  case 70:
    coms_alerts.create_alert(e_alert_type::alert, "Beginning recovery");
    recovery_begin();
  break;
  case 71:
    coms_alerts.create_alert(e_alert_type::alert, "Arming FDS");
    recovery_arm();
  break;
  case 72:
    coms_alerts.create_alert(e_alert_type::alert, "Deploying FDS");
    recovery_deploy_parachute();
  break;
  case 73:
    coms_alerts.create_alert(e_alert_type::alert, "Removing latest special indicator from queue");
    remove_latest_special_indicator_from_queue();
  break;
  case 74:
    coms_alerts.create_alert(e_alert_type::alert, "Add special indicator [COMMAND_RECEIVED] to queue");
    add_special_indicator_to_queue(e_event_options::event_command_received);
  break;
  case 75:
    coms_alerts.create_alert(e_alert_type::alert, "Add special indicator [USB_STATUS_CHANGED] to queue");
    add_special_indicator_to_queue(e_event_options::event_usb_connection_changed);
  break;
  case 76:
    coms_alerts.create_alert(e_alert_type::alert, "Add special indicator [MISSION_ABORT] to queue");
    add_special_indicator_to_queue(e_event_options::event_mission_abort);
  break;
  case 77:
    coms_alerts.create_alert(e_alert_type::alert, "Add special indicator [ERROR] to queue");
    add_special_indicator_to_queue(e_event_options::event_error);
  break;
  case 78:
    coms_alerts.create_alert(e_alert_type::alert, "Enable Baro calibration");
    baro_enable_calibration();
  break;
  case 79:
    coms_alerts.create_alert(e_alert_type::alert, "Enable IMU calibration");
    imu_enable_calibration();
  break;
  case 80:
    coms_alerts.create_alert(e_alert_type::alert, "Resetting orientation");
    reset_ori();
  break;
  case 81:
    coms_alerts.create_alert(e_alert_type::alert, "Resetting altitude kalman filter");
    reset_altitude_kalman();
  break;
  case 82:
    coms_alerts.create_alert(e_alert_type::alert, "Erasing all of the flash chip. NOTE! This removes the filesystem. Meaning you have to reformat to create files");
    recorder_earase_flash_chip();
  break;
  case 83:
    //coms_alerts.create_alert(e_alert_type::alert, "");

  break;
    
  default:
  
  break;
  }
}