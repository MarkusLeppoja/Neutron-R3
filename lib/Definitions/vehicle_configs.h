#ifndef __VEHICLE_CONFIGS_H__
#define __VEHICLE_CONFIGS_H__

#include <Arduino.h>

/* Vehicle config struct */
typedef struct {
/* Software info */
    const String software_name;
    const String software_version;
    const String software_compile_date;

/* Sensors */
    // Overall sensors functionality
    uint8_t enable_sensors : 1;
    // Individual sensor functionality status
    uint8_t enable_imu : 1;
    uint8_t enable_baro : 1;
    uint8_t enable_voltage_divider : 1;
    // Conversion ratios
    const float voltage_divider_ratio;
    const float gravity;

/* Recovery (Servo & Pyro) */
    // Overall recovery functionality
    uint8_t enable_recovery : 1; //TODO: Setup a check in recovery for this bool
    // Individual recovery method functionality
    uint8_t enable_servo : 1; //TODO: Setup a check in pyro for this bool
    uint8_t enable_pyro : 1; //TODO: Setup a check in recovery for this bool
    // Recovery servo lock & release orientation
    const int fds_servo_release_angle;
    const int fds_servo_lock_angle;
    // Pyro channel open duration (microseconds)
    const uint32_t pyro_1_fire_duration;
    const uint32_t pyro_2_fire_duration;
    // Pyro voltage divider ratio
    const float pyro_voltage_divider_ratio; 

/* Flash Logging & Serial Casting */
    // Overall flash functionality
    uint8_t enable_flash_log : 1;
    // Data that gets logged to flash
    uint8_t enable_flash_telemetry_log : 1;
    uint8_t enable_flash_notification_log : 1;
    // Interval in which data gets logged to flash (microseconds)
    const uint32_t flash_log_interval_mode_0;
    const uint32_t flash_log_interval_mode_1;
    const uint32_t flash_log_interval_mode_2;
    const uint32_t flash_log_interval_mode_3;
    const uint32_t flash_log_interval_mode_4;
    // Flash data file name & format
    const String flash_data_file_0_name;  // Important TLM
    const String flash_data_file_1_name;  // Less important TLM
    const String flash_data_file_2_name;  // Notifications
    const String flash_data_file_3_name;  // Current Config
    const String flash_data_file_0_format;
    const String flash_data_file_1_format;
    const String flash_data_file_2_format;
    const String flash_data_file_3_format;

    // Overall serial functionality
    uint8_t enable_serial_stream : 1;
    // Data that gets casted from serial
    uint8_t enable_serial_telemetry_stream : 1;
    uint8_t enable_serial_notification_stream : 1;
    // Interval in which data gets logged to serial (microseconds)
    const uint32_t serial_stream_interval_mode_0;
    const uint32_t serial_stream_interval_mode_1;
    const uint32_t serial_stream_interval_mode_2;
    const uint32_t serial_stream_interval_mode_3;
    const uint32_t serial_stream_interval_mode_4;

    // The data that gets logged &| casted
    // Basic (On time, Mission Time, State)
    uint8_t enable_datasave_basic : 2;

    // Pyro (Pyro fire status, Pyro voltage)
    uint8_t enable_datasave_pyro : 2;
    
    // Sensor (IMU, Baro, V-Divider) 
    uint8_t enable_datasave_imu_data : 2;
    uint8_t enable_datasave_baro_data : 2;
    uint8_t enable_datasave_v_divider_data : 2;
    uint8_t enable_datasave_performance : 2;

    /* Indicator */
    uint8_t enable_status_indicator : 1;
    uint8_t enable_led : 1;
    uint8_t enable_buzzer : 1;

    /* Communications */
    uint8_t enable_coms : 1;
    uint8_t enable_coms_message_echo : 1;
    const String coms_notification_start_char;
    const String coms_notification_end_char;
    const String coms_csv_string_layout_start_char;
    const String coms_csv_string_layout_end_char;


    /* State Transition */
    const float _st_detect_launch_accel_threshold;
    const float _st_detect_landing_touchdown_altitude_threshold;
    const float _st_max_mission_duration;

} Neutron_Vehicle_Config_t;                                       

#define Electron_Test_Config                                                              \
  {                                                                                       \
    /* Software info */                                                                   \
    .software_name = "Neutron R3 Software",                                               \
    .software_version = "1.1.0",                                                          \
    .software_compile_date = __DATE__,  /* Combile time */                                \
    /* Overall sensors functionality */                                                   \
    .enable_sensors = 1,                                                                  \
    /* Individual sensor functionality status */                                          \
    .enable_imu = 1,                                                                      \
    .enable_baro = 1,                                                                     \
    .enable_voltage_divider = 1,                                                          \
    /* Conversion ratios */                                                               \
    .voltage_divider_ratio = (3.3 / 4096 * 10), /* RP2040 has 12bit ADCs */               \
    .gravity = 9.82,                                                                      \
    /* Overall recovery functionality */                                                  \
    .enable_recovery = 1,                                                                 \
    /* Individual recovery method functionality */                                        \
    .enable_servo = 1,                                                                    \
    .enable_pyro = 1,                                                                     \
    /* Recovery servo lock & release orientation */                                       \
    .fds_servo_release_angle = 50,                                                        \
    .fds_servo_lock_angle = 125,                                                          \
    /* Pyro channel open duration (microseconds) */                                       \
    .pyro_1_fire_duration = 0,                                                            \
    .pyro_2_fire_duration = 0,                                                            \
    /* Pyro voltage divider ratio */                                                      \
    .pyro_voltage_divider_ratio = (3.3 / 4096 * 10),  /* RP2040 has 12bit ADC */          \
    /* Overall flash functionality */                                                     \
    .enable_flash_log = 0,                                                                \
    /* Data that gets logged to flash */                                                  \
    .enable_flash_telemetry_log = 1,                                                      \
    .enable_flash_notification_log = 1,                                                   \
    /* Interval in which data gets logged to flash (microseconds) */                      \
    .flash_log_interval_mode_0 = 250000,                                                  \
    .flash_log_interval_mode_1 = 100000,                                                  \
    .flash_log_interval_mode_2 = 50000,                                                   \
    .flash_log_interval_mode_3 = 20000,                                                   \
    .flash_log_interval_mode_4 = 10000,                                                   \
    /* Flash data file name & format */                                                   \
    .flash_data_file_0_name = "",   /* Important TLM */                                   \
    .flash_data_file_1_name = "",   /* Less important TLM */                              \
    .flash_data_file_2_name = "",   /* Notifications */                                   \
    .flash_data_file_3_name = "",   /* Current Config */                                  \
    .flash_data_file_0_format = ".csv",                                                   \
    .flash_data_file_1_format = ".csv",                                                   \
    .flash_data_file_2_format = ".txt",                                                   \
    .flash_data_file_3_format = ".txt",                                                   \
    /* Overall serial functionality */                                                    \
    .enable_serial_stream = 1,                                                            \
    /* Data that gets casted from serial */                                               \
    .enable_serial_telemetry_stream = 1,                                                  \
    .enable_serial_notification_stream = 1,                                               \
    /* Interval in which data gets logged to serial (microseconds) */                     \
    .serial_stream_interval_mode_0 = 500000,                                              \
    .serial_stream_interval_mode_1 = 200000,                                              \
    .serial_stream_interval_mode_2 = 50000,                                               \
    .serial_stream_interval_mode_3 = 20000,                                               \
    .serial_stream_interval_mode_4 = 5000,                                                \
    /* Where data gets saved (0 - Doesn't get saved, 1 - Main file, 2 - Second file) */   \
    .enable_datasave_basic = 1,                                                           \
    /* Pyro (Pyro fire status, Pyro voltage, pyro loop duration & rate) */                \
    .enable_datasave_pyro = 0,                                                            \
    /* Sensor (IMU, Baro, V-Divider) */                                                   \
    .enable_datasave_imu_data = 1,                                                        \
    .enable_datasave_baro_data = 1,                                                       \
    .enable_datasave_v_divider_data = 1,                                                  \
    .enable_datasave_performance = 1,                                                     \
    /* Indicator */                                                                       \
    .enable_status_indicator = 0,                                                         \
    .enable_led = 0,                                                                      \
    .enable_buzzer = 0,                                                                   \
    /* Communications */                                                                  \
    .enable_coms = 0,                                                                     \
    .enable_coms_message_echo = 0,                                                        \
    .coms_notification_start_char = "<",                                                  \
    .coms_notification_end_char = ">",                                                    \
    .coms_csv_string_layout_start_char = "<<",                                            \
    .coms_csv_string_layout_end_char = ">>",                                              \
    /* State Transition */                                                                \
    ._st_detect_launch_accel_threshold = 0,                                               \
    ._st_detect_landing_touchdown_altitude_threshold = 0,                                 \
    ._st_max_mission_duration = 0,                                                        \
  }                                                                                       

extern Neutron_Vehicle_Config_t active_vehicle_config;

// @brief Get current (readonly) instance of vehicle config
Neutron_Vehicle_Config_t get_active_config();


#endif
