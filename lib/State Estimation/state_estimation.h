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


// @todo Log calibration values to flash


#define RADS_TO_DEG 57.29578

using namespace BLA;
extern uint64_t _imu_update_prev, _baro_update_prev, _gnss_update_prev, _v_divider_update_prev, _mag_update_prev, _accel_prev, _gyro_prev; 
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
void reset_altitude_kalman();

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
// @brief Baro calibration data results calculating function
void _baro_calibrate_calculate_deviation();
#endif  