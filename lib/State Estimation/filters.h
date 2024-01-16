#ifndef __FILTERS_H__
#define __FILTERS_H__

#include <Arduino.h>
#include "BasicLinearAlgebra.h"

using namespace BLA;

// 2 Step 6 element advanced kalman filter
class POSITION_KALMAN_FILTER_T;
extern POSITION_KALMAN_FILTER_T kf_position;

class POSITION_KALMAN_FILTER_T
{
private:
    Matrix<6, 1> state_vector = {
        0,  // X Position
        0,  // X Velocity
        0,  // Y Position
        0,  // Y Velocity
        0,  // Z Position
        0   // Z Velocity
    };

    Matrix<6, 6> P = {
        1, 0,
        0, 1
    };
    
    Matrix<6, 6> F;
    Matrix<6, 6> B;

    // Kalman gain
    Matrix<6, 6> K = {
        0.1,
        0.1,
        0.1,
        0.1,
        0.1,
        0.1
    };

    
    // Observation matrix
    Matrix<6, 6> H_baro = {
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0
    };

    Matrix<6, 6> H_gnss = {
        1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 1
    };

    Matrix<6, 6> I = {
        1, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 1
    };

    Matrix<6, 1> accel_measurement;
    Matrix<6, 1> baro_measurement;
    Matrix<1, 6> gnss_measurement;

    uint64_t kf_prev;
    double dt;
    boolean is_first_step = 1;

public:
    void predict_accel(float accel_x, float accel_y, float accel_z)
    {
        dt = (micros() - kf_prev) / 1000000.;
        kf_prev = micros();

        if (!is_first_step)
        { 
            F = {
                1, 0, 0, 0, 0, 0,
                0, dt, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0,
                0, 0, 0, dt, 0, 0,
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, dt
            };

            B = {
                (dt * dt) / 2.f, 0, 0, 0, 0, 0,
                0, dt, 0, 0, 0, 0,
                0, 0, (dt * dt) / 2.f, 0, 0,
                0, 0, 0, dt, 0, 0,
                0, 0, 0, 0, (dt * dt) / 2.f, 0,
                0, 0, 0, 0, 0, dt
            };

            accel_measurement = {accel_x, accel_x, accel_y, accel_y, accel_z, accel_z};
            state_vector = F * state_vector + B * accel_measurement;
            P = F * P * ~F + Q;
        }
        is_first_step = 0;
    }

    void update_baro(float baro_altitude)
    {
        // Use baro altitude only at Z axis position
        baro_measurement = {
            0,
            0,
            0,
            0,
            baro_altitude,
            0
        };

        // Calculate new kalman gain
        K = P * ~H_baro * Inverse(H_baro * P * ~H_baro + R);

        // Update state
        state_vector = state_vector + K * (baro_measurement - H_baro * state_vector);

        // Update estimate uncertanty
        P = (I - K * H_baro) * P * (~(I - K * H_baro)) + K * R * ~K;
    }    

    float get_position()
    {
        return state_vector(0);
    }

    float get_velocity()
    {
        return state_vector(1);
    }

    Matrix<6, 6> get_kalman_gain()
    {
        return K;
    }

    void reset()
    {
        // Resets the estimation to 0
        state_vector = {
            0,
            0
        };

        // Sets the uncertanty really high
        P = {
            0.01, 0,
            0, 0.01
        };
    }

    // Process Noise Covariance Matrix (Prediction noise)
    Matrix<6, 6> Q = {
        0.00000003125, 0, 0, 0, 0, 0,
        0, 0.000125, 0, 0, 0, 0,
        0, 0, 0.00000003125, 0, 0, 0,
        0, 0, 0, 0.000125, 0, 0,
        0, 0, 0, 0, 0.00000003125, 0,
        0, 0, 0, 0, 0, 000125
    };

    // Measurement Covariance (Baro error)
    Matrix<6, 6> R = {
        0.092
    };
};
POSITION_KALMAN_FILTER_T kf_position;

// 2 Step 2 element advanced kalman filter
class ALTITUDE_KALMAN_FILTER_T;
extern ALTITUDE_KALMAN_FILTER_T kf_altitude;

class ALTITUDE_KALMAN_FILTER_T
{
private:
    Matrix<2, 1> state_vector = {
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
    double dt;
    boolean is_first_step = 1;

public:
    void predict_accel(float accel_data)
    {
        dt = (micros() - kf_prev) / 1000000.;
        kf_prev = micros();

        if (!is_first_step)
        { 
            F = {
                1, dt,
                0, 1
            };

            B = {
                (dt * dt) / 2.f,
                dt
            };

            accel_measurement = {accel_data};
            state_vector = F * state_vector + B * accel_measurement;
            P = F * P * ~F + Q;
        }
        is_first_step = 0;
    }

    void update_baro(float baro_altitude)
    {
        // Use baro altitude only at Z axis position
        baro_measurement = {
            baro_altitude
        };

        // Calculate new kalman gain
        K = P * ~H_baro * Inverse(H_baro * P * ~H_baro + R);

        // Update state
        state_vector = state_vector + K * (baro_measurement - H_baro * state_vector);

        // Update estimate uncertanty
        P = (I - K * H_baro) * P * (~(I - K * H_baro)) + K * R * ~K;
    }    

    float get_altitude()
    {
        return state_vector(0);
    }

    float get_velocity()
    {
        return state_vector(1);
    }

    Matrix<2, 1> get_kalman_gain()
    {
        return K;
    }

    void reset()
    {
        // Resets the estimation to 0
        state_vector = {
            0,
            0
        };

        // Sets the uncertanty really high
        P = {
            0.01, 0,
            0, 0.01
        };
    }

    // Process Noise Covariance Matrix (Prediction noise)
    Matrix<2, 2> Q = {
        0.00000003125, 0,
        0, 0.000125,
    };

    // Measurement Covariance (Baro error)
    Matrix<1, 1> R = {
        0.092
    };
};
ALTITUDE_KALMAN_FILTER_T kf_altitude;

// Low- / Highpass filter
class band_pass
{
private:
    double prev_value, gain;
public:
    band_pass(float t_gain)
    {
        gain = t_gain;
        prev_value = 0;
    }

    double update_estimate(double value)
    {
        return prev_value * gain + value * (1 - gain);
    }
};

// Simple version of kalman filter 
class kalman
{
private:
    double kalman_gain, measure_error, estimate_error, estimate, prev_estimate;
public:
    void init(double i_E_MEA, double EST)
    {
        measure_error = estimate_error = i_E_MEA;
        estimate = EST;
    }

    double update_estimate(double value)
    {
        kalman_gain = estimate_error / (estimate_error + measure_error);

        estimate = estimate + kalman_gain * (value - estimate);
        estimate_error = (1 - kalman_gain) * estimate_error + fabsf((prev_estimate - estimate) * measure_error);

        prev_estimate = estimate;

        return estimate;
    }

    void reset()
    {
        estimate_error = measure_error;
        estimate = kalman_gain = 0.1;
    }
};
#endif  