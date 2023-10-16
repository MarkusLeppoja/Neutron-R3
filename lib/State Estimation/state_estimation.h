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



#endif  