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
    const double voltage_divider_ratio;
    const double gravity;

/* Recovery (Servo & Pyro) */
    // Overall recovery functionality
    uint8_t enable_recovery : 1;
    // Individual recovery method functionality
    uint8_t enable_servo : 1; 
    uint8_t enable_pyro : 1;
    // Recovery servo lock & release orientation
    const int fds_servo_release_angle;
    const int fds_servo_lock_angle;
    // Pyro channel open duration (microseconds)
    const uint32_t pyro_1_fire_duration;
    const uint32_t pyro_2_fire_duration;
    // Pyro voltage divider ratio
    const double pyro_voltage_divider_ratio; 

/* Flash Logging & Serial Casting */
    // Overall flash functionality
    uint8_t enable_flash_log : 1;
    // Data that gets logged to flash
    uint8_t enable_flash_notification_log : 1;
    uint8_t enable_flash_file_0_log : 1;
    uint8_t enable_flash_file_1_log : 1;
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
    uint8_t enable_serial_notification_stream : 1;
    uint8_t enable_serial_file_0_stream : 1;
    uint8_t enable_serial_file_1_stream : 1;
    // Interval in which data gets logged to serial (microseconds)
    const uint32_t serial_stream_interval_mode_0;
    const uint32_t serial_stream_interval_mode_1;
    const uint32_t serial_stream_interval_mode_2;
    const uint32_t serial_stream_interval_mode_3;
    const uint32_t serial_stream_interval_mode_4;

    // The data that gets logged &| casted
    // Basic (On time, Mission Time, State)
    uint8_t enable_datasave_basic : 1;

    // Pyro (Pyro fire status, Pyro voltage)
    uint8_t enable_datasave_pyro : 1;
    
    // Sensor (IMU, Baro, V-Divider) 
    uint8_t enable_datasave_imu_data : 1;
    uint8_t enable_datasave_baro_data : 1;
    uint8_t enable_datasave_v_divider_data : 1;
    uint8_t enable_datasave_performance : 1;

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
    const double _st_detect_launch_accel_threshold;
    const double _st_detect_landing_touchdown_altitude_threshold;
    const double _st_max_mission_duration;
    const double _st_max_ascent_time;

} Neutron_Vehicle_Config_t;                                       

#define Electron_Test_Config                                                              \
  {                                                                                       \
    /* Software info */                                                                   \
    .software_name = "Neutron R3 Software",                                               \
    .software_version = "1.5.2",                                                          \
    .software_compile_date = __DATE__,  /* Combile time */                                \
    /* Overall sensors functionality */                                                   \
    .enable_sensors = 1,                                                                  \
    /* Individual sensor functionality status */                                          \
    .enable_imu = 1,                                                                      \
    .enable_baro = 1,                                                                     \
    .enable_voltage_divider = 1,                                                          \
    /* Conversion ratios */                                                               \
    .voltage_divider_ratio = (3.3 / 4096.0 * 10.0), /* RP2040 has 12bit ADCs */           \
    .gravity = 9.82,                                                                      \
    /* Overall recovery functionality */                                                  \
    .enable_recovery = 1,                                                                 \
    /* Individual recovery method functionality */                                        \
    .enable_servo = 1,                                                                    \
    .enable_pyro = 0,                                                                     \
    /* Recovery servo lock & release orientation */                                       \
    .fds_servo_release_angle = 50,                                                        \
    .fds_servo_lock_angle = 125,                                                          \
    /* Pyro channel open duration (microseconds) */                                       \
    .pyro_1_fire_duration = 0,                                                            \
    .pyro_2_fire_duration = 0,                                                            \
    /* Pyro voltage divider ratio */                                                      \
    .pyro_voltage_divider_ratio = (3.3 / 4096 * 10.),  /* RP2040 has 12bit ADC */         \
    /* Overall flash functionality */                                                     \
    .enable_flash_log = 1,                                                                \
    /* Data that gets logged to flash */                                                  \
    .enable_flash_notification_log = 1,                                                   \
    .enable_flash_file_0_log = 1,                                                         \
    .enable_flash_file_1_log = 1,                                                         \
    /* Interval in which data gets logged to flash (microseconds) */                      \
    .flash_log_interval_mode_0 = 200000,                                                  \
    .flash_log_interval_mode_1 = 100000,                                                  \
    .flash_log_interval_mode_2 = 50000,                                                   \
    .flash_log_interval_mode_3 = 20000,                                                   \
    .flash_log_interval_mode_4 = 10000,                                                   \
    /* Flash data file name & format */                                                   \
    .flash_data_file_0_name = "Neutron R3 F0",    /* Important TLM */                     \
    .flash_data_file_1_name = "Neutron R3 F1",    /* Less important TLM */                \
    .flash_data_file_2_name = "Neutron R3 F2",    /* Notifications */                     \
    .flash_data_file_3_name = "NONE",             /* Current Config */                    \
    .flash_data_file_0_format = ".csv",                                                   \
    .flash_data_file_1_format = ".csv",                                                   \
    .flash_data_file_2_format = ".txt",                                                   \
    .flash_data_file_3_format = "NONE",                                                   \
    /* Overall serial functionality */                                                    \
    .enable_serial_stream = 1,                                                            \
    /* Data that gets casted from serial */                                               \
    .enable_serial_notification_stream = 1,                                               \
    .enable_serial_file_0_stream = 0,                                                     \
    .enable_serial_file_1_stream = 0,                                                     \
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
    .enable_status_indicator = 1,                                                         \
    .enable_led = 1,                                                                      \
    .enable_buzzer = 1,                                                                   \
    /* Communications */                                                                  \
    .enable_coms = 1,                                                                     \
    .enable_coms_message_echo = 1,                                                        \
    .coms_notification_start_char = "<",                                                  \
    .coms_notification_end_char = ">",                                                    \
    .coms_csv_string_layout_start_char = "<<",                                            \
    .coms_csv_string_layout_end_char = ">>",                                              \
    /* State Transition */                                                                \
    ._st_detect_launch_accel_threshold = 10.2,                                            \
    ._st_detect_landing_touchdown_altitude_threshold = 3,                                 \
    ._st_max_mission_duration = 50,                                                       \
    ._st_max_ascent_time = 6,                                                             \
  }                                                                                       

extern Neutron_Vehicle_Config_t active_vehicle_config;

#endif
