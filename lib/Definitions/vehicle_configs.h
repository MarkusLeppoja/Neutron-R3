#ifndef __VEHICLE_CONFIGS_H__
#define __VEHICLE_CONFIGS_H__

struct vehicle_config_template
{
    boolean enable_flash_chip = true;
    boolean enable_flight_telemetry_recording;
    boolean enable_system_notification_recording;
};

typedef struct {
    /* Vehicle config */
    String vehicle_name;


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
    uint32_t serial_stream_interval_mode_0;
    uint32_t serial_stream_interval_mode_1;
    uint32_t serial_stream_interval_mode_2;
    uint32_t flash_log_interval_mode_0;
    uint32_t flash_log_interval_mode_1;
    uint32_t flash_log_interval_mode_2;

  //file name, format, 



    /* */


} Neutron_Vehicle_Config_t;


#define Electron_Flight_5_Config                  \
  {                                               \
    .vehicle_name = "Electron",                   \
    .enable_flash_log = true,                     \
    .enable_serial_stream = true,                 \
    .enable_flash_telemetry_log = true,           \
    .enable_flash_notification_log = true,        \
    .enable_serial_telemetry_stream = true,       \
    .enable_serial_notification_stream = true,    \
    .serial_stream_interval_mode_0 = 0,           \
    .serial_stream_interval_mode_1 = 0,           \
    .serial_stream_interval_mode_2 = 0,           \
    .flash_log_interval_mode_0 = 0,               \
    .flash_log_interval_mode_1 = 0,               \
    .flash_log_interval_mode_2 = 0,               \
  }                                              

#define Electron_Test_Config                      \
  {                                               \
    .vehicle_name = "Electron",                   \
    .enable_flash_log = true,                     \
    .enable_serial_stream = true,                 \
    .enable_flash_telemetry_log = true,           \
    .enable_flash_notification_log = true,        \
    .enable_serial_telemetry_stream = true,       \
    .enable_serial_notification_stream = true,    \
    .serial_stream_interval_mode_0 = 10^6 * 2,           \
    .serial_stream_interval_mode_1 = 10^6 * 0.5,           \
    .serial_stream_interval_mode_2 = 0,           \
    .flash_log_interval_mode_0 = 10^6,               \
    .flash_log_interval_mode_1 = 10^6 * 2,               \
    .flash_log_interval_mode_2 = 0,               \
  }                                               

extern Neutron_Vehicle_Config_t active_vehicle_config;
#endif
