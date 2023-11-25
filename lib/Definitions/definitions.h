#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <Arduino.h>
#include "vehicle_configs.h"

enum e_mission_state : const uint8_t {
    startup = 0,
    navigation_startup = 1,
    ground_locked = 2,
    pad_idle = 3,
    ascent = 4,
    ballistic_decent = 5,
    chute_decent = 6,
    landed = 7,
    startup_failed = 8
};
extern e_mission_state active_mission_state;

enum e_pins : const uint8_t {
    pin_uart_tx = 0,
    pin_uart_rx = 1,
    pin_i2c_sda = 2,
    pin_i2c_scl = 3,
    pin_servo_1 = 4,
    pin_servo_2 = 5,
    pin_servo_3 = 6,
    pin_led_rgb_r = 8,
    pin_led_rgb_b = 9,
    pin_led_rgb_g = 10,
    pin_pyro_1 = 11,
    pin_pyro_2 = 12,
    pin_buzzer = 13,
    pin_cs_flash = 16,
    pin_cs_gnss = 17,
    pin_spi_sck = 18,
    pin_spi_mosi = 19,
    pin_spi_miso = 20,
    pin_voltage_divider = 26,
    pin_pyro_2_voltage = 27,
    pin_pyro_1_voltage = 28
};

struct s_clock
{
    uint64_t microseconds, milliseconds;
    double seconds;

    double mission_duration, mission_begin_time;    // In seconds
};
extern s_clock Clock;

struct s_code_performance
{
    /* Sensors */
    double imu_loop, imu_function_duration;
    double baro_loop, baro_function_duration;
    double voltage_divider_loop, voltage_divider_function_duration;

    /* Data Recorder */ 
    double data_recorder_flash_loop, data_recorder_flash_function_duration;
    double data_recorder_serial_loop, data_recorder_serial_function_duration;
    double data_recorder_file_0_func_duration, data_recorder_file_1_func_duration;

    /* Other */
    double main_loop_loop, main_loop_function_duration;
    double pyro_loop, pyro_function_duration;
    double state_indicator_loop, state_indicator_function_duration;
};
extern s_code_performance Code_performance;

struct s_booleans 
{
    // Automatically updated variable that determines if sensors can be used or not
    uint8_t sw_sensors_usability : 1;
    uint8_t sw_sensors_imu_usability : 1;
    uint8_t sw_sensors_baro_usability : 1;
    uint8_t sw_sensors_imu_enable_calibration : 1;
    uint8_t sw_sensors_baro_enable_calibration : 1;

    // These get set manually inside coms class  
    uint8_t sw_begin_pad_idle_ground_lock_exit_countdown : 1;
    uint8_t sw_begin_pad_idle_ground_lock_exit_countdown_prev : 1;

};
extern s_booleans Booleans;

// @brief 3-Dimentional Vector
struct vector_3d {
    double x,y,z;
};

// @brief 2-Dimentional Vector
struct vector_2d {
    double x,y;
};

struct s_sensors
{
    double apogee_altitude;
    /* IMU (BMI088) */
    vector_3d raw_gyro_velocity_rps, raw_gyro_velocity_dps, raw_acceleration;
    double raw_acceleometer_temperature;
    vector_3d orientation, gyro_velocity_dps, gyro_velocity_rps, acceleration;

    /* Baro (BMP388) */
    float _f_raw_baro_pressure, _f_raw_baro_altitude, _f_raw_baro_temperature;
    double raw_baro_pressure, raw_baro_altitude, raw_baro_temperature;
    double raw_baro_altitude_wo_bias;

    /* Kalman filter */
    vector_3d position, velocity;

    /* Pyro */
    uint8_t pyro_1_fire_status : 1;
    uint8_t pyro_2_fire_status : 1;
    double pyro_1_voltage, pyro_2_voltage;

    /* Voltage divider */
    double internal_voltage;

    // Calibration (private variables)
    double _gyro_offset_x;
    double _gyro_offset_y;
    double _gyro_offset_z;
    double _accel_offset_x;
    double _accel_offset_y;
    double _accel_offset_z;
    double _baro_offset_altitude;
    double _gyro_standard_deviation_x;
    double _gyro_standard_deviation_y;
    double _gyro_standard_deviation_z;
    double _accel_standard_deviation_x;
    double _accel_standard_deviation_y;
    double _accel_standard_deviation_z;
    double _baro_standard_deviation_altitude;
};
extern s_sensors Sensors;

void update_mcu_clock();
boolean is_mission_active();
int get_mission_state();
#endif