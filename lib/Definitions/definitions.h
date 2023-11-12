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
    float seconds;

    // In seconds
    float mission_duration, mission_begin_time;
};
extern s_clock Clock;

struct s_code_performance
{
    /* CPU */
    int cpu_clock_frequency;
    int cpu_free_heap, cpu_used_heap;

    /* Sensors */
    float imu_loop, imu_function_duration;
    float baro_loop, baro_function_duration;
    float voltage_divider_loop, voltage_divider_function_duration;

    /* Data Recorder */ 
    float data_recorder_flash_loop, data_recorder_flash_function_duration;
    float data_recorder_serial_loop, data_recorder_serial_function_duration;

    /* Other */
    float main_loop_loop, main_loop_function_duration;
    float pyro_loop, pyro_function_duration;
    float state_indicator_loop, state_indicator_function_duration;
};
extern s_code_performance Code_performance;

struct s_booleans 
{
    // Automatically updated variable that determines if flash is working correctly to be used
    uint8_t sw_flash_chip_usability : 1;

    // Automatically updated variable that determines if sensors can be used or not
    uint8_t sw_sensors_usability : 1;
    uint8_t sw_sensors_imu_usability : 1;
    uint8_t sw_sensors_baro_usability : 1;
    uint8_t sw_sensors_imu_enable_calibration : 1;
    uint8_t sw_sensors_baro_enable_calibration : 1;

    // Manually set in coms to begin 
    uint8_t sw_begin_pad_idle_ground_lock_exit_countdown : 1;
    uint8_t sw_begin_pad_idle_ground_lock_exit_countdown_prev : 1;

};
extern s_booleans Booleans;

// @brief 3-Dimentional Vector
struct vector_3d {
    float x,y,z;
};

// @brief 2-Dimentional Vector
struct vector_2d {
    float x,y;
};

struct s_sensors
{
    float apogee_altitude;
    /* IMU (BMI088) */
    vector_3d raw_gyro_velocity_rps, raw_gyro_velocity_dps, raw_acceleration;
    float raw_acceleometer_temperature;
    vector_3d orientation, gyro_velocity_dps, gyro_velocity_rps, acceleration;

    /* Baro (BMP388) */
    float raw_baro_pressure, raw_baro_altitude, raw_baro_temperature;
    float raw_baro_altitude_wo_bias;

    /* Kalman filter */
    vector_3d position, velocity;

    /* Pyro */
    uint8_t pyro_1_fire_status : 1;
    uint8_t pyro_2_fire_status : 1;
    float pyro_1_voltage, pyro_2_voltage;

    /* Voltage divider */
    float internal_voltage;

    // Calibration (private variables)
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
};
extern s_sensors Sensors;

// @brief Updates multiple types of clocks
void update_mcu_clock();
// @brief Requests basic performance data from RP2040
void update_cpu_profiler();
// @brief Returns if mission is active
uint8_t is_mission_active();
// @brief Returns mission state
e_mission_state get_mission_state();
#endif