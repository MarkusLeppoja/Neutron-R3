#ifndef __STATE_ESTIMATION_H__
#define __STATE_ESTIMATION_H__

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <definitions.h>
#include <profiler.h>
#include <alerts.h>
#include <filters.h>
#include <BMI088.h>
#include <BMP388_DEV.h>
#include <Orientation.h>

#define RADS_TO_DEG 57.29578

// @todo Log calibration values to flash
// @todo Define beginning values for kalman filters
// @todo remove useless function descriptions

// @brief Begins and configures Baro sensor
int _baro_begin();
// @brief Begins and configures IMU sensor
int _imu_begin();
// @brief Begins and configures Voltage Divider sensor
void _v_divider_begin();
// @brief Calls for IMU data and processes it
void _imu_update();
// @brief Calls for acceleration data and processes it
void _accel_update();
// @brief Calls for gyroscope data and processes it
void _gyro_update();
// @brief Calls for barometric data and processes it
void _baro_update();
// @brief Calls for voltage divider data and processes it
void _v_divider_update();
// @brief Overall function for beginning and configuring the sensors
int sensors_begin();
// @brief Overall function for updating the sensors
void update_sensors();

// @brief Zeroes out orientation
void reset_ori();
// @brief Resests kalman filter altitude and trust
void reset_position_kalman();

// @brief Enables Baro calibration
void baro_enable_calibration();
// @brief Enables IMU calibration
void imu_enable_calibration(); 

// @brief IMU calibration function
void _imu_calibrate();
// @brief IMU calibration info gathering function
void _imu_calibrate_update();
// @brief IMU calibration data results calculating function
void _imu_calibrate_calculate_deviation();
// @brief Baro calibration function
void _baro_calibrate();
// @brief Baro calibration info gathering function
void _baro_calibrate_update();
// @brief For finding the true offset of baro
void _baro_calibrate_calculate_offset();
#endif  