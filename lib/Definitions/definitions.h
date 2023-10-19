#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <Arduino.h>
#include "vehicle_configs.h"



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

void update_mcu_clock();



struct s_clock
{
    uint64_t microseconds, milliseconds;
    float seconds;

    // In seconds
    float mission_duration, mission_begin_time;
};
extern s_clock Clock;

struct s_booleans 
{
    // Automatically updated variable that determines if flash is working correctly to be used
    boolean sw_flash_chip_usability;

    // Automatically updated variable that determines if sensors can be used or not
    boolean sw_sensors_usability;
    boolean sw_sensors_imu_usability;
    boolean sw_sensors_baro_usability;
    boolean sw_sensors_gnss_usability;
    boolean sw_sensors_mag_usability;
    boolean sw_sensors_imu_enable_calibration;
    boolean sw_sensors_baro_enable_calibration;

};
extern s_booleans Booleans;



struct vector_3d {
    float x,y,z;
};

struct vector_2d {
    float x,y;
};

struct s_sensors
{
    const float gravity = -9.817823;


    /* IMU (Bmi088) */
    // Raw readings
    vector_3d raw_gyro_velocity;
    vector_3d raw_accel;
    float raw_accel_temp;

    /* Baro (BMP388) */
    float raw_baro_pressure;
    float raw_baro_altitude;
    float raw_baro_altitude_wo_bias;
    float raw_baro_temperature;


    /* Pyro */
    boolean pyro_1_fire_status, pyro_2_fire_status;
    float pyro_1_voltage, pyro_2_voltage;

    /* Voltage divider */
    float system_voltage;





    /* Profiler */
    // IMU
    float profiler_imu_function_duration;
    float profiler_imu_loop;

    // Baro
    float profiler_baro_function_duration;
    float profiler_baro_loop;

    // GNSS
    float profiler_gnss_function_duration;
    float profiler_gnss_loop;

    // Mag
    float profiler_mag_function_duration;
    float profiler_mag_loop;

    // Voltage Divider
    float profiler_voltage_divider_function_duration;
    float profiler_voltage_divider_loop;


    //TODO: FLASH, DATARECORDER, PYRO, STATE INDICATION, MAIN LOOP,

};
extern s_sensors Sensors;


#endif  