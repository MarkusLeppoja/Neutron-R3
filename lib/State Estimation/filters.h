#ifndef __FILTERS_H__
#define __FILTERS_H__

#include <Arduino.h>
#include <BasicLinearAlgebra.h>

// Low- / Highpass filter
class band_pass
{
private:
    float prev_value, gain;
public:
    band_pass(float t_gain);
    float update_estimate(float value);
};

// Simple version of kalman filter 
class kalman
{
private:
    float kalman_gain, measure_error, estimate_error, estimate, prev_estimate;
public:
    void init(float i_E_MEA, float EST);
    float update_estimate(float value);
    void reset();
};

// 2 Step advanced kalman filter
using namespace BLA;
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

    uint64_t kf_prev;
    float dt;
    boolean is_first_step = 1;
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

#endif  