#ifndef __VEHICLE_CONFIGS_H__
#define __VEHICLE_CONFIGS_H__

typedef struct {
    /* Vehicle config */
    const String vehicle_name;

    const float voltage_divider_ratio; //TODO: 0.03206256F

    /* Recovery */
    // Toggle fds (Fairing Deployment System) servo usability
    bool enable_fds_servo;

    // What angle should the servo be at during fds lock and release
    const float fds_servo_release_angle, fds_servo_lock_angle;


    /* Pyrotechnics */
    // Toggle wheather pyrotechnics channels can be fired or not
    bool enable_pyrotechnics;

    // Change the duration that certain pyrotechnics channels are on. Durations are in microseconds (1,000,000)
    const uint64_t pyro_1_fire_duration;
    const uint64_t pyro_2_fire_duration;

    // This * pyro_voltage_reading gives you the voltage going thru the pyro channel
    const float pyro_voltage_divider_ratio; //TODO: 0.03206256F



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



    /* */


} Neutron_Vehicle_Config_t;


#define Electron_Test_Config                      \
  {                                               \
    .vehicle_name = "Electron",                   \
    .enable_flash_log = true,                    \
    .enable_serial_stream = true,                 \
    .enable_flash_telemetry_log = true,           \
    .enable_flash_notification_log = true,        \
    .enable_serial_telemetry_stream = true,       \
    .enable_serial_notification_stream = true,    \
    .serial_stream_interval_mode_0 = 1000000,     \
    .serial_stream_interval_mode_1 = 2000000,     \
    .serial_stream_interval_mode_2 = 250000,      \
    .flash_log_interval_mode_0 = 1000000,            \
    .flash_log_interval_mode_1 = 2000000,        \
    .flash_log_interval_mode_2 = 3000000,               \
  }                                               

extern Neutron_Vehicle_Config_t active_vehicle_config;
#endif
