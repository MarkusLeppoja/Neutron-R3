#ifndef __VEHICLE_CONFIGS_H__
#define __VEHICLE_CONFIGS_H__

/* Vehicle config struct */
typedef struct {
    /* General */
    const float voltage_divider_ratio;

    /* Sensors */
    bool enable_sensors;
    bool enable_imu;
    bool enable_baro;
    bool enable_gnss;
    bool enable_voltage_divider;
    bool enable_mag;

    // Debug features
    bool enable_gnss_debug;

    // Offsets & deviation
    float _gyro_offset_x;
    float _gyro_offset_y;
    float _gyro_offset_z;
    float _accel_offset_x;
    float _accel_offset_y;
    float _accel_offset_z;
    float _baro_offset_altitude;
    float _gyro_standard_deviation_x;
    float _gyro_standard_deviation_y;
    float _gyro_standard_deviation_z;
    float _accel_standard_deviation_x;
    float _accel_standard_deviation_y;
    float _accel_standard_deviation_z;
    float _baro_standard_deviation_altitude;

    /* Recovery */
    // Toggle fds (Fairing Deployment System) servo usability
    bool enable_fds_servo;

    // What angle should the servo be at during fds lock and release
    const int fds_servo_release_angle, fds_servo_lock_angle;


    /* Pyrotechnics */
    // Toggle wheather pyrotechnics channels can be fired or not
    bool enable_pyrotechnics;

    // Change the duration that certain pyrotechnics channels are on. Durations are in microseconds (1,000,000)
    const uint64_t pyro_1_fire_duration;
    const uint64_t pyro_2_fire_duration;

    // This * pyro_voltage_reading gives you the voltage going thru the pyro channel
    const float pyro_voltage_divider_ratio; 

    /* Data logging */
    // Toggle whether data gets streamed, logged
    bool enable_flash_log;
    bool enable_serial_stream;

    // Toggle what data gets logged to flash
    bool enable_flash_telemetry_log;
    bool enable_flash_notification_log;
    // Toggle what data gets streamed to serial
    bool enable_serial_telemetry_stream;
    bool enable_serial_notification_stream;

    // Change data stream, log mode intervals. Intervals are in microseconds (1,000,000)
    const uint32_t serial_stream_interval_mode_0;
    const uint32_t serial_stream_interval_mode_1;
    const uint32_t serial_stream_interval_mode_2;
    const uint32_t flash_log_interval_mode_0;
    const uint32_t flash_log_interval_mode_1;
    const uint32_t flash_log_interval_mode_2;

    // Loggable file name & format
    const String flash_data_file_name;
    const String flash_data_file_format;

    /* Toggle printing / saving different kind of data */
    // General
    bool enable_datasave_general;

    // Pyro
    bool enable_datasave_pyro;
    
    // Sensor data
    bool enable_datasave_imu_data;
    bool enable_datasave_baro_data;
    bool enable_datasave_gnss_data;
    bool enable_datasave_v_divider_data;
    bool enable_datasave_mag_data;

    // Profilers 
    bool enable_datasave_profiler_sensors_loop;
    bool enable_datasave_profiler_sensors_duration;

    /* Indicator */
    bool enable_led;
    bool enable_buzzer;
    bool enable_indicator;

    /* Communications */
    bool enable_coms;
    bool enable_coms_message_echo;


    /* State Transition */
    const float detect_launch_accel_threshold;
    const float detect_landing_touchdown_altitude_threshold;
    const float max_ascent_time;
    const float max_mission_duration;

} Neutron_Vehicle_Config_t;                                       

#define Electron_Flight_Config                                                    \
  {                                                                               \
    .voltage_divider_ratio = 0.00806,                                             \
    .enable_sensors = true,                                                       \
    .enable_imu = true,                                                           \
    .enable_baro = true,                                                          \
    .enable_gnss = false,                                                         \
    .enable_voltage_divider = true,                                               \
    .enable_mag = false,                                                          \
    .enable_gnss_debug = false,                                                   \
    /* Fairing Deployment System */                                               \
    .enable_fds_servo = true,                                                     \
    .fds_servo_release_angle = 50,                                                \
    .fds_servo_lock_angle = 125,                                                  \
    /* Pyrotechnics */                                                            \
    .enable_pyrotechnics = false,                                                 \
    .pyro_1_fire_duration = 250000,                                               \
    .pyro_2_fire_duration = 250000,                                               \
    .pyro_voltage_divider_ratio = 0.00806,                                        \
    /* Flash & Serial Logging */                                                  \
    .enable_flash_log = true,                                                     \
    .enable_serial_stream = true,                                                 \
    .enable_flash_telemetry_log = true,                                           \
    .enable_flash_notification_log = true,                                        \
    .enable_serial_telemetry_stream = false,                                      \
    .enable_serial_notification_stream = true,                                    \
    .serial_stream_interval_mode_0 = 1000000,                                     \
    .serial_stream_interval_mode_1 = 100000,                                      \
    .serial_stream_interval_mode_2 = 50000,                                       \
    .flash_log_interval_mode_0 = 1000000,                                         \
    .flash_log_interval_mode_1 = 50000,                                           \
    .flash_log_interval_mode_2 = 22000,                                           \
    .flash_data_file_name = "Electron Flight 5",                                  \
    .flash_data_file_format = ".csv",                                             \
    /* Select what data gets logged */                                            \
    .enable_datasave_general = true,                                              \
    .enable_datasave_pyro = false,                                                \
    .enable_datasave_imu_data = true,                                             \
    .enable_datasave_baro_data = true,                                            \
    .enable_datasave_gnss_data = false,                                           \
    .enable_datasave_v_divider_data = true,                                       \
    .enable_datasave_mag_data = false,                                            \
    .enable_datasave_profiler_sensors_loop = true,                                \
    .enable_datasave_profiler_sensors_duration = true,                            \
    /* Indicators */                                                              \
    .enable_led = true,                                                           \
    .enable_buzzer = true,                                                        \
    .enable_indicator = true,                                                     \
    /* Communications */                                                          \
    .enable_coms = true,                                                          \
    .enable_coms_message_echo = true,                                             \
    /* State Transition */                                                        \
    .detect_launch_accel_threshold = 11.2f,                                       \
    .detect_landing_touchdown_altitude_threshold = 5,                             \
    .max_ascent_time = 4,                                                         \
    .max_mission_duration = 30,                                                   \
  }

#define Electron_Test_Config                                                      \
  {                                                                               \
    .voltage_divider_ratio = 0.00806,                                             \
    .enable_sensors = true,                                                       \
    .enable_imu = true,                                                           \
    .enable_baro = true,                                                          \
    .enable_gnss = false,                                                         \
    .enable_voltage_divider = true,                                               \
    .enable_mag = false,                                                          \
    .enable_gnss_debug = false,                                                   \
    /* Fairing Deployment System */                                               \
    .enable_fds_servo = true,                                                     \
    .fds_servo_release_angle = 50,                                                \
    .fds_servo_lock_angle = 125,                                                  \
    /* Pyrotechnics */                                                            \
    .enable_pyrotechnics = false,                                                 \
    .pyro_1_fire_duration = 1000000,                                              \
    .pyro_2_fire_duration = 1000000,                                              \
    .pyro_voltage_divider_ratio = 0.00806,                                        \
    /* Flash & Serial Logging */                                                  \
    .enable_flash_log = true,                                                     \
    .enable_serial_stream = true,                                                 \
    .enable_flash_telemetry_log = true,                                           \
    .enable_flash_notification_log = true,                                        \
    .enable_serial_telemetry_stream = true,                                       \
    .enable_serial_notification_stream = true,                                    \
    .serial_stream_interval_mode_0 = 100000,                                      \
    .serial_stream_interval_mode_1 = 1000000,                                     \
    .serial_stream_interval_mode_2 = 50000,                                       \
    .flash_log_interval_mode_0 = 1000000,                                         \
    .flash_log_interval_mode_1 = 22000,                                           \
    .flash_log_interval_mode_2 = 50000,                                           \
    .flash_data_file_name = "Neutron Dev",                                        \
    .flash_data_file_format = ".csv",                                             \
    /* Select what data gets logged */                                            \
    .enable_datasave_general = true,                                              \
    .enable_datasave_pyro = false,                                                \
    .enable_datasave_imu_data = true,                                             \
    .enable_datasave_baro_data = true,                                            \
    .enable_datasave_gnss_data = false,                                           \
    .enable_datasave_v_divider_data = true,                                       \
    .enable_datasave_mag_data = false,                                            \
    .enable_datasave_profiler_sensors_loop = true,                                \
    .enable_datasave_profiler_sensors_duration = true,                            \
    /* Indicators */                                                              \
    .enable_led = true,                                                           \
    .enable_buzzer = false,                                                       \
    .enable_indicator = true,                                                     \
    /* Communications */                                                          \
    .enable_coms = true,                                                          \
    .enable_coms_message_echo = true,                                             \
  }                                                                               \

extern Neutron_Vehicle_Config_t active_vehicle_config;
#endif
