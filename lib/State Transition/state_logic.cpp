#include "state_logic.h"

//TODO: Add option to format the entire device. ensure that formating closes all files and disables logging for a minute and restores it to default state after.
uint64_t _state_machine_prev, _state_begin_time;
uint16_t _detect_state_counter_1, _detect_state_counter_2;
float _state_duration;
boolean _has_calibration_started;

Alerts alerts_state_logic("State Logic");


void set_mission_state(e_mission_state state)
{
    active_mission_state = state;

    // Disable indicators
    set_rgb_led_color(1,1,1);
    set_buzzer_tone(0,false);

    alerts_state_logic.create_alert(e_alert_type::alert, "Mission state changed to " + String(state));

    // Event responder
    _on_mission_state_change(state);
}

void _on_mission_state_change(e_mission_state new_state)
{

    switch (new_state)
    {
    case e_mission_state::startup:
        // Begin serial communication
        Serial.begin(115200);
        Serial.setTimeout(0);

        // Software Info //
        alerts_state_logic.create_alert(e_alert_type::alert, active_vehicle_config.software_name + " V" + active_vehicle_config.software_version + " Combile Date " + active_vehicle_config.software_compile_date);

        // All important function startups
        uint8_t startup_status;
        startup_status += sensors_begin();
        startup_status += recorder_begin();
        pyro_begin();
        indicator_begin();
        recovery_begin();

        // Check if startup was successful
        if (startup_status != 2)
        {
          alerts_state_logic.create_alert(e_alert_type::error, "Startup failed! Changing state to STARTUP_FAILED");
          set_mission_state(e_mission_state::startup_failed); 
          return;
        }

        // Configure
        Clock.mission_duration = 0;
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_2, 0);
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_0, 1);

        set_serial_log_interval(active_vehicle_config.serial_stream_interval_mode_2, 0);
        set_serial_log_interval(active_vehicle_config.serial_stream_interval_mode_2, 1);

        // Reset STARTUP state indicator
        reset_power_on_indication();

        list_all_existing_files();
        // Create log 
        alerts_state_logic.create_alert(e_alert_type::success, "Startup was successful! Changing state to Navigation Startup");
        // Change state and begin calibration
        set_mission_state(e_mission_state::navigation_startup);
    break;
    case e_mission_state::navigation_startup:
        // Reset mission duration just in case
        Clock.mission_duration = 0;

        // Begin calibration timer
        _state_begin_time = micros();
        _state_duration = 0;
    break;
    case e_mission_state::ground_locked:
        Clock.mission_duration = 0;

        // Disable flash telemetry log to save some space until ground locked has ended
        active_vehicle_config.enable_flash_log = true;
        active_vehicle_config.enable_flash_file_0_log = false;
        active_vehicle_config.enable_flash_file_1_log = false;
        active_vehicle_config.enable_flash_notification_log = true;   
    break;
    case e_mission_state::pad_idle:
        // Put the flash update rate to before launch config
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_2, 0);
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_0, 1);

        // Reset mission duration just in case
        Clock.mission_duration = 0;

        // Disable serial stream and ensure all flash log is enabled
        active_vehicle_config.enable_serial_stream = false;

        active_vehicle_config.enable_flash_log = true;
        active_vehicle_config.enable_flash_file_0_log = true;
        active_vehicle_config.enable_flash_file_1_log = true;
        active_vehicle_config.enable_flash_notification_log = true;        
    break;
    case e_mission_state::ascent:
        // Begin faster logging
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_3, 0);
        set_flash_log_interval(active_vehicle_config.flash_log_interval_mode_2, 1);

        // Disable LED & buzzer to save power
        active_vehicle_config.enable_led = false;
        active_vehicle_config.enable_buzzer = false;
        set_rgb_led_color(1,1,1);
        set_buzzer_tone(0, false);

        // Reset mission duration time
        Clock.mission_duration = 0;
        Clock.mission_begin_time = Clock.seconds;

        // Reset orientation
        reset_ori();
    break;
    case e_mission_state::ballistic_decent:
        recovery_deploy_parachute();

        set_mission_state(e_mission_state::chute_decent);
    break;
    case e_mission_state::chute_decent:
        
    break;
    case e_mission_state::landed:
        alerts_state_logic.create_alert(e_alert_type::alert, "Vehicle apogee was " + String(Sensors.apogee_altitude) + " meters");
        close_all_flash_files();

        // Re-Enable lED & buzzer
        active_vehicle_config.enable_led = true;
        active_vehicle_config.enable_buzzer = true;
    break;
    case e_mission_state::startup_failed:
        Clock.mission_duration = 0;

        close_all_flash_files();
        active_vehicle_config.enable_flash_log = false;
        active_vehicle_config.enable_serial_stream = true;
        active_vehicle_config.enable_serial_notification_stream = true;
    break;
    
    default: break;
    }

}

void update_state_machine()
{
    update_sensors();
    update_pyro();
    update_indicator();
    update_communication();

    update_mcu_clock();
    // Calculate state duration
    _state_duration = (Clock.microseconds - _state_begin_time) / 1000000.f;

    // update only after 10 milliseconds
    if (Clock.microseconds - _state_machine_prev <= 10000.f) return;
    _state_machine_prev = Clock.microseconds;

    switch (get_mission_state())
    {
    case e_mission_state::navigation_startup:
        // Only run this part once
        if (_state_duration >= 60 && !_has_calibration_started)
        {
            _has_calibration_started = true;
            imu_enable_calibration();
            baro_enable_calibration();
        }
        // 10 Sec after beginning calibration switch mission state
        if (_state_duration >= 70 && _has_calibration_started)
        {
            set_mission_state(ground_locked);
        }
    break;
    case e_mission_state::ground_locked:
        // Waits until user is ready to change state. If is ready waits 90 seconds and changes state

        if (Booleans.sw_begin_pad_idle_ground_lock_exit_countdown && !Booleans.sw_begin_pad_idle_ground_lock_exit_countdown_prev)
        {
            _state_begin_time = micros();
            _state_duration = 0;
            Booleans.sw_begin_pad_idle_ground_lock_exit_countdown_prev = true;
        }
        else if (!Booleans.sw_begin_pad_idle_ground_lock_exit_countdown && Booleans.sw_begin_pad_idle_ground_lock_exit_countdown_prev)
        {
            Booleans.sw_begin_pad_idle_ground_lock_exit_countdown_prev = false;
        }

        if (Booleans.sw_begin_pad_idle_ground_lock_exit_countdown && Booleans.sw_begin_pad_idle_ground_lock_exit_countdown_prev && _state_duration >= 120)
        {
            set_mission_state(e_mission_state::pad_idle);
        }
    break;
    case e_mission_state::pad_idle:
        detect_launch();
    break;
    case e_mission_state::ascent:
        detect_apogee();
        detect_abort();
    break;
    case e_mission_state::ballistic_decent:
        detect_landed();
    break;
    case e_mission_state::chute_decent:
        detect_landed();
    break;
    case e_mission_state::landed:
        
    break;
    case e_mission_state::startup_failed:
        
    break;
    default:
        break;
    }

    // Saving data is the last step.
    update_recorder();
}



void detect_launch()
{
    Sensors.acceleration.z >= active_vehicle_config._st_detect_launch_accel_threshold ? _detect_state_counter_1++ : _detect_state_counter_1 = 0;

    if (_detect_state_counter_1 >= 10)
    {
        _detect_state_counter_1 = 0;

        // Create alert
        alerts_state_logic.create_alert(e_alert_type::success, "Launch detected");
        
        // Set new state
        set_mission_state(e_mission_state::ascent);
    }
}

void detect_apogee()
{
    Sensors.position.z < Sensors.apogee_altitude ? _detect_state_counter_1++ : _detect_state_counter_1 = 0;

    if (_detect_state_counter_1 >= 50 || has_time_elipsed(active_vehicle_config._st_max_ascent_time))
    {
        _detect_state_counter_1 = 0;

        // Create alert
        alerts_state_logic.create_alert(e_alert_type::success, "Apogee detected");

        // Set new state
        set_mission_state(e_mission_state::ballistic_decent);
    }
}

void detect_landed()
{
    Sensors.position.z <= active_vehicle_config._st_detect_landing_touchdown_altitude_threshold ? _detect_state_counter_1++ : _detect_state_counter_1 = 0;
    // @todo abs(Sensors.accelSum) >= touchdownAccelThreshold ? _detect_state_counter_2++ : _detect_state_counter_2 = 0;

    if (_detect_state_counter_1 >= 300 || has_time_elipsed(active_vehicle_config._st_max_mission_duration))
    {
        _detect_state_counter_1 = _detect_state_counter_2 = 0;

        // Create alert
        alerts_state_logic.create_alert(e_alert_type::success, "Touchdown detected");

        // Set new state
        set_mission_state(e_mission_state::landed);
    }
}

boolean detect_abort()
{
    // Orientation based abort
    (abs(Sensors.orientation.x > 65) || abs(Sensors.orientation.y) > 65) ? _detect_state_counter_1++ : _detect_state_counter_1 = 0;

    // Angular rate based abort
    (abs(Sensors.gyro_velocity_dps.x > 150) || abs(Sensors.gyro_velocity_dps.y > 150)) ? _detect_state_counter_2++ : _detect_state_counter_2 = 0;

    if (_detect_state_counter_1 >= 20 || _detect_state_counter_2 >= 5)
    {
        _detect_state_counter_1 = _detect_state_counter_2 = 0;
        alerts_state_logic.create_alert(e_alert_type::warning, "Abort detected. Deploying parachute");
    
        recovery_deploy_parachute();

        add_special_indicator_to_queue(e_event_options::event_mission_abort);
        set_mission_state(e_mission_state::chute_decent);
        return true;
    }
    return false;
}

boolean has_time_elipsed(float greater_than)
{
    return Clock.mission_duration >= greater_than ? 1 : 0;
}