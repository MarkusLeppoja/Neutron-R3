#include "filters.h"

/* Filters */
void kalman::init(float i_E_MEA, float EST)
{
    measure_error = estimate_error = i_E_MEA;
    estimate = EST;
}

void kalman::reset()
{
    estimate_error = measure_error;
    estimate = kalman_gain = 0.1f;
}

float kalman::update_estimate(float value)
{
    kalman_gain = estimate_error / (estimate_error + measure_error);

    estimate = estimate + kalman_gain * (value - estimate);
    estimate_error = (1 - kalman_gain) * estimate_error + fabsf((prev_estimate - estimate) * measure_error);

    prev_estimate = estimate;

    return estimate;
}

band_pass::band_pass(float t_gain)
{
    gain = t_gain;
    prev_value = 0;
}

float band_pass::update_estimate(float value)
{
    return prev_value * gain + value * (1 - gain);
}

void position_kalman_filter::predict_accel(float accel_data)
{
    dt = (micros() - kf_prev) / 1000000.f;
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

void position_kalman_filter::update_baro(float baro_altitude)
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

float position_kalman_filter::get_position()
{
    return state_vector(0);
}

float position_kalman_filter::get_velocity()
{
    return state_vector(1);
}

Matrix<2, 1> position_kalman_filter::get_kalman_gain()
{
    return K;
}

void position_kalman_filter::reset()
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