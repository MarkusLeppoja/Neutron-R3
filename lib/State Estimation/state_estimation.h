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
#include <BasicLinearAlgebra.h>
#include <Orientation.h>


using namespace BLA;
extern uint64_t _imu_update_prev, _baro_update_prev, _gnss_update_prev, _v_divider_update_prev, _mag_update_prev, _accel_prev; 
extern float _sensors_imu_accel_cal_x[], _sensors_imu_accel_cal_y[], _sensors_imu_accel_cal_z[];
extern float _sensors_imu_gyro_cal_x[], _sensors_imu_gyro_cal_y[], _sensors_imu_gyro_cal_z[];
extern float _sensors_baro_altitude_cal[];
extern uint16_t _sensors_imu_calibration_list_index, _sensors_baro_calibration_list_index;

/* Filters */
class band_pass
{
private:
    float old_value, gain;
public:
    band_pass(float t_gain);
    float update_estimate(float value);
};

class kalman
{
private:
    float kg, err_mea, err_est, est, old_est;
public:
    void init(float i_E_MEA, float EST);
    float update_estimate(float value);
    void reset();
};

class position_kalman_filter
{
private:
    Matrix<2, 1> state_vector =
    {
        0,
        0
    };

    Matrix<2, 2> P = {
        1, 0,
        0, 1,
    };
    
    Matrix<2, 2> F;
    Matrix<2, 1> B;

    // Kalman gain
    Matrix<2, 1> K = {
        0.1,
        0.1
    };

    // Process Noise Covariance Matrix (Prediction noise)
    Matrix<2, 2> Q = {
        0.00000003125, 0,
        0, 0.000125,
    };
    
    // Observation matrix
    Matrix<1, 2> H_baro = {
        1, 0
    };

    Matrix<2, 2> I = {
        1, 0,
        0, 1,
    };

    Matrix<1, 1> accel_measurement;
    Matrix<1, 1> baro_measurement;

    //TODO: Sync with calibration
    uint64_t kf_prev;
    float dt;
    boolean is_first_step = true;
public:
    void predict_accel(float accel_data);
    void update_baro(float baro_altitude);

    float get_position();
    float get_velocity();
    Matrix<2, 1> get_kalman_gain();
    void reset();

    // Measurement Covariance (Baro error)
    Matrix<1, 1> R = {
        0.092
    };
};




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
int sensors_begin();
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