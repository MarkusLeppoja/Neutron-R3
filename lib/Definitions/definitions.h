#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <Arduino.h>
#include "vehicle_configs.h"

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


};
extern s_booleans Booleans;

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

struct s_sensors
{

    // Pyro
    boolean pyro_1_fire_status, pyro_2_fire_status;
    float pyro_1_voltage, pyro_2_voltage;
};
extern s_sensors Sensors;

#endif  