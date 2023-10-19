#ifndef __STATE_ESTIMATION_H__
#define __STATE_ESTIMATION_H__

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <definitions.h>
#include <profiler.h>
#include <alerts.h>
#include <BMI088.h>
#include <BMP388_DEV.h>


extern uint64_t _imu_update_prev, _baro_update_prev, _gnss_update_prev, _v_divider_update_prev, _mag_update_prev; 
extern float _sensors_imu_accel_cal_x[], _sensors_imu_accel_cal_y[], _sensors_imu_accel_cal_z[];
extern float _sensors_imu_gyro_cal_x[], _sensors_imu_gyro_cal_y[], _sensors_imu_gyro_cal_z[];
extern float _sensors_baro_altitude_cal[];
extern uint16_t _sensors_imu_calibration_list_index, _sensors_baro_calibration_list_index;



// TODO: Function descriptions
//TODO: Kalman filter. 3D position one, for accel divider offsets
int _baro_begin();
int _imu_begin();
void _v_divider_begin();
void _imu_update();
void _accel_update();
void _gyro_update();
void _baro_update();
void _v_divider_update();
int sensors_init();
void update_sensors();

// @todo maybe log these values into flash
void baro_enable_calibration();
void imu_enable_calibration(); 
void _imu_calibrate();
void _imu_calibrate_update();
void _imu_calibrate_calculate_deviation();
void _baro_calibrate();
void _baro_calibrate_update();
void _baro_calibrate_calculate_deviation();
#endif  