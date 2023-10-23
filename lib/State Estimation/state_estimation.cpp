#include "state_estimation.h"

// Instances
// Communication protocol custom instance
SPIClassRP2040 spi_sensors_instance(spi0, e_pins::pin_spi_miso, e_pins::pin_cs_flash, e_pins::pin_spi_sck, e_pins::pin_spi_mosi);
TwoWire i2c_sensors_instance(&i2c1_inst, e_pins::pin_i2c_sda, e_pins::pin_i2c_scl);

// Sensors class instances
Bmi088Accel accel_instance(i2c_sensors_instance, 0x18);
Bmi088Gyro gyro_instance(i2c_sensors_instance, 0x68);
BMP388_DEV baro_instance(i2c_sensors_instance);

// Alerts class instance
Alerts alerts_sensors("State Estimation");

// Performance profilers instances
Profiler profiler_imu;
Profiler profiler_baro;
Profiler profiler_gnss;         // @todo create
Profiler profiler_mag;          // @todo create
Profiler profiler_v_divider;

// Local variables
uint64_t _imu_update_prev, _baro_update_prev, _gnss_update_prev, _v_divider_update_prev, _mag_update_prev, _accel_prev;
float _sensors_imu_accel_cal_x[400], _sensors_imu_accel_cal_y[400], _sensors_imu_accel_cal_z[400];
float _sensors_imu_gyro_cal_x[400], _sensors_imu_gyro_cal_y[400], _sensors_imu_gyro_cal_z[400];
float _sensors_baro_altitude_cal[100];
uint16_t _sensors_imu_calibration_list_index, _sensors_baro_calibration_list_index;

// Kalman filter
position_kalman_filter position_kalman;
kalman kalman_x_dps, kalman_y_dps, kalman_z_dps;
kalman kalman_x_accel, kalman_y_accel, kalman_z_accel;

// Orientation
Orientation quat, quat_world_accel, new_quat;
EulerAngles euler_ori;







/* Filters */
void kalman::init(float i_E_MEA, float EST)
{
    err_mea = err_est = i_E_MEA;
    est = EST;
}

void kalman::reset()
{
    err_est = err_mea;
    est = kg = 1.f;
}

float kalman::update_estimate(float value)
{
    kg = err_est / (err_est + err_mea);

    est = est + kg * (value - est);
    err_est = (1 - kg) * err_est + fabsf((old_est - est) * err_mea);

    old_est = est;

    return est;
}

band_pass::band_pass(float t_gain)
{
    gain = t_gain;
    old_value = 0;
}

float band_pass::update_estimate(float value)
{
    return old_value * gain + value * (1 - gain);
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
    is_first_step = false;
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



int _baro_begin()
{
    if (!active_vehicle_config.enable_baro)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "Baro disabled");
        Booleans.sw_sensors_baro_usability = false;
        return 0;
    }

    uint8_t baro_init_status;
    baro_init_status = baro_instance.begin(0x76);

    if (!baro_init_status)
    {
        alerts_sensors.create_alert(e_alert_type::error, "Failed to connect with baro. Disabling baro functionality");
        Booleans.sw_sensors_baro_usability = 0;
        return 0;
    }

    // Configure
    baro_instance.setTimeStandby(TIME_STANDBY_20MS);
    baro_instance.setPresOversampling(OVERSAMPLING_X4);
    baro_instance.setTempOversampling(OVERSAMPLING_X4);
    baro_instance.setIIRFilter(IIR_FILTER_OFF);

    baro_instance.startNormalConversion();

    // Enable baro functionality and create alert
    Booleans.sw_sensors_baro_usability = 1;
    alerts_sensors.create_alert(e_alert_type::success, "Baro startup successful");

    // Update prev to current time
    _baro_update_prev = micros();

    return 1;
}

int _imu_begin()
{
    // Disable functionality if enable_imu is false
    if (!active_vehicle_config.enable_imu)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "IMU disabled");
        Booleans.sw_sensors_imu_usability = false;
        return 0;
    }

    // Begin imu and capture its status
    int imu_gyro_init_status, imu_accel_init_status;
    imu_accel_init_status = accel_instance.begin();
    imu_gyro_init_status = gyro_instance.begin();

    // If startup failed return, create alert and disable usability
    if (imu_accel_init_status + imu_gyro_init_status != 2)
    {
        alerts_sensors.create_alert(e_alert_type::error, "Failed to connect with BMI088. Accel status: " + String(imu_accel_init_status) + " Gyro status:" + String(imu_gyro_init_status));
        Booleans.sw_sensors_imu_usability = 0;
        return 0;
    }

    // Configure
    accel_instance.setRange(Bmi088Accel::RANGE_24G);
    accel_instance.setOdr(Bmi088Accel::ODR_400HZ_BW_145HZ);
    gyro_instance.setRange(Bmi088Gyro::RANGE_2000DPS);
    gyro_instance.setOdr(Bmi088Gyro::ODR_400HZ_BW_47HZ);

    kalman_x_dps.init(0.1f, 0.1f);
    kalman_y_dps.init(0.1f, 0.1f);
    kalman_z_dps.init(0.1f, 0.1f);
    kalman_x_accel.init(0.1f, 0.01f);
    kalman_y_accel.init(0.1f, 0.01f);
    kalman_z_accel.init(0.1f, -9.81f);

    // Enable imu functionality and create alert
    Booleans.sw_sensors_imu_usability = 1;
    alerts_sensors.create_alert(e_alert_type::success, "IMU startup successful");

    // Update prev to current time
    _imu_update_prev = micros();

    return 1;
}

void _v_divider_begin()
{
    // Set the correct pinmode
    pinMode(e_pins::pin_voltage_divider, INPUT);

    // Update prev to current time
    _v_divider_update_prev = micros();
}









void _imu_update()
{
   update_mcu_clock();
   profiler_imu.begin_loop();

   if (Clock.microseconds - _imu_update_prev < 2500 || !active_vehicle_config.enable_imu || !Booleans.sw_sensors_imu_usability) return;
    _imu_update_prev = Clock.microseconds;

    // Profiler
    profiler_imu.end_loop(Sensors.profiler_imu_loop);
    profiler_imu.begin_function();

    // Update sensors seperately
    _accel_update();
    _gyro_update();

    // Update kalman filter
    position_kalman.predict_accel(-(Sensors.accel.z - Sensors.gravity));

    // Finds offset and standard deviation if enabled
    _imu_calibrate();

    // Profiler
    profiler_imu.end_function(Sensors.profiler_imu_function_duration);
}

void _accel_update()
{
    accel_instance.readSensor();

    float dt = (micros() - _accel_prev) / 1000000.f;
    _accel_prev = micros();

    Sensors.raw_accel.x = accel_instance.getAccelY_mss();
    Sensors.raw_accel.y = -accel_instance.getAccelX_mss();
    Sensors.raw_accel.z = accel_instance.getAccelZ_mss();

    Sensors.raw_accel_temp = accel_instance.getTemperature_C();
    
    Sensors.accel.x = kalman_x_accel.update_estimate(Sensors.raw_accel.x - active_vehicle_config._accel_offset_x);
    Sensors.accel.y = kalman_y_accel.update_estimate(Sensors.raw_accel.y - active_vehicle_config._accel_offset_y);
    Sensors.accel.z = kalman_z_accel.update_estimate(Sensors.raw_accel.z - active_vehicle_config._accel_offset_z);
}

    /* This portion of code is taken from AdamMarciniak Cygnus-X1-Software github repository 
     https://github.com/AdamMarciniak/Cygnus-X1-Software/blob/master/src/Nav.cpp TODO:

    // Calculates the world acceleration
    //quat_world_accel.orientation = quat.orientation.rotate(Quaternion(0, Sensors.localAccel.z, Sensors.localAccel.y, Sensors.localAccel.x));
    //quat_world_accel.orientation = Quaternion(0, Sensors.localAccel.z, Sensors.localAccel.x, -Sensors.localAccel.y).operator*=(quat.orientation);

    double norm = sqrt(sq(Sensors.gyro_velocity.x) + sq(Sensors.gyro_velocity.y) + sq(Sensors.gyro_velocity.z));
    norm = copysign(max(abs(norm), 1e-9), norm); 

    // De-biasing
    //new_quat.orientation.rotate(Quaternion(0.0,0, Sensors.accelOri.y, Sensors.accelOri.x));

    new_quat.orientation *= new_quat.orientation.from_axis_angle(dt * norm, Sensors.gyro_velocity.x / norm, Sensors.gyro_velocity.y / norm, Sensors.gyro_velocity.z / norm);
    quat_world_accel.orientation = new_quat.orientation.rotate(Quaternion(0.0, -Sensors.raw_accel.y, Sensors.raw_accel.x, Sensors.raw_accel.z));


    Sensors.world_accel.x = quat_world_accel.orientation.c;
    Sensors.world_accel.y = quat_world_accel.orientation.b;
    Sensors.world_accel.z = quat_world_accel.orientation.d;*/

void _gyro_update()
{
    gyro_instance.readSensor();

    Sensors.raw_gyro_velocity.x = -gyro_instance.getGyroX_rads();
    Sensors.raw_gyro_velocity.y = -gyro_instance.getGyroY_rads();
    Sensors.raw_gyro_velocity.z = gyro_instance.getGyroZ_rads();

    Sensors.gyro_velocity.x = kalman_x_dps.update_estimate(Sensors.raw_gyro_velocity.x - active_vehicle_config._gyro_offset_x);
    Sensors.gyro_velocity.y = kalman_y_dps.update_estimate(Sensors.raw_gyro_velocity.y - active_vehicle_config._gyro_offset_y);
    Sensors.gyro_velocity.z = kalman_z_dps.update_estimate(Sensors.raw_gyro_velocity.z - active_vehicle_config._gyro_offset_z);

    //TODO: orientation
}

void _baro_update()
{
    update_mcu_clock();
    profiler_baro.begin_loop();


    if (Clock.microseconds - _baro_update_prev < 20000 || !active_vehicle_config.enable_baro || !Booleans.sw_sensors_baro_usability) return;
    _baro_update_prev = Clock.microseconds;

    // Profiler
    profiler_baro.end_loop(Sensors.profiler_baro_loop);
    profiler_baro.begin_function();


    baro_instance.getMeasurements(Sensors.raw_baro_temperature, Sensors.raw_baro_pressure, Sensors.raw_baro_altitude);
    
    Sensors.raw_baro_altitude_wo_bias = Sensors.raw_baro_altitude - active_vehicle_config._baro_offset_altitude;

    // Update kalman filter
    position_kalman.update_baro(Sensors.raw_baro_altitude_wo_bias);
    Sensors.altitude = position_kalman.get_position();
    Sensors.velocity = position_kalman.get_velocity();

    // Finds offset and standard deviation if enabled
    _baro_calibrate();

    // Profiler
    profiler_baro.end_function(Sensors.profiler_baro_function_duration);
}

void _v_divider_update()
{
    update_mcu_clock();
    profiler_v_divider.begin_loop();

    if (Clock.microseconds - _v_divider_update_prev < 50000 || !active_vehicle_config.enable_voltage_divider) return;
    _v_divider_update_prev = Clock.microseconds;

    // Profiler
    profiler_v_divider.end_loop(Sensors.profiler_voltage_divider_loop);
    profiler_v_divider.begin_function();

    Sensors.system_voltage = analogRead(e_pins::pin_voltage_divider) * active_vehicle_config.voltage_divider_ratio;

    // Profiler
    profiler_v_divider.end_function(Sensors.profiler_voltage_divider_function_duration);
}

int sensors_begin()
{
    if (!active_vehicle_config.enable_sensors)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "Sensors disabled");
        Booleans.sw_sensors_usability = false;
        return 0;
    }

    // Begin sensors and save their status
    uint8_t baro_status = _baro_begin();
    uint8_t imu_status = _imu_begin();
    //uint8_t gnss_status;// = _gnss_begin(active_vehicle_config.enable_gnss_debug);
    //uint8_t mag_status
    _v_divider_begin();

    // If all sensors didn't initialize correctly create alert and disable sensors functionality
    if ((baro_status + imu_status) != 2) // @todo if using gnss add gnss_status and change this to 3 
    {
        alerts_sensors.create_alert(e_alert_type::error, "Sensors startup unsuccessful");
        Booleans.sw_sensors_usability = false;
        return 0; 
    }

    // Indicate successful startup and enable sensors usability
    alerts_sensors.create_alert(e_alert_type::success, "Sensors startup successful");
    Booleans.sw_sensors_usability = true;

    return 1;
}

void update_sensors()
{
    if (!active_vehicle_config.enable_sensors || !Booleans.sw_sensors_usability) return;
    
    _imu_update();
    _baro_update();
    //_gnss_update();
    //_mag_update();
    _v_divider_update();
} //dav, ma lahen ja loodin nüüd oma põrandat edasi.









void imu_enable_calibration()
{
    Booleans.sw_sensors_imu_enable_calibration = true;
}

void baro_enable_calibration()
{
    Booleans.sw_sensors_baro_enable_calibration = true;
}

void _imu_calibrate()
{
    if (!Booleans.sw_sensors_imu_enable_calibration) return;
    _sensors_imu_calibration_list_index >= 399 ? _imu_calibrate_calculate_deviation() : _imu_calibrate_update();
}

void _imu_calibrate_calculate_deviation()
{
    // Disable calibration function endicating the end of the  
    Booleans.sw_sensors_imu_enable_calibration = false;

    vector_3d accel_sum, gyro_sum;

    for (uint16_t i = 0; i < 400; i++)
    {
        // Accel
        accel_sum.x += _sensors_imu_accel_cal_x[i];
        accel_sum.y += _sensors_imu_accel_cal_y[i];
        accel_sum.z += _sensors_imu_accel_cal_z[i];
        // Gyro
        gyro_sum.x += _sensors_imu_gyro_cal_x[i];
        gyro_sum.y += _sensors_imu_gyro_cal_y[i];
        gyro_sum.z += _sensors_imu_gyro_cal_z[i];
    }
    
    // Calculate average deviation
    active_vehicle_config._gyro_offset_x = gyro_sum.x / 399.f;
    active_vehicle_config._gyro_offset_y = gyro_sum.y / 399.f;
    active_vehicle_config._gyro_offset_z = gyro_sum.z / 399.f;
    active_vehicle_config._accel_offset_x = accel_sum.x / 399.f;
    active_vehicle_config._accel_offset_y = accel_sum.y / 399.f;
    active_vehicle_config._accel_offset_z = accel_sum.z / 399.f;

    // Create alert, logging offsets
    String offsets_string = 
    String(active_vehicle_config._gyro_offset_x) + " Y: " +
    String(active_vehicle_config._gyro_offset_y) + " Z: " +
    String(active_vehicle_config._gyro_offset_z) + " Accel X: " + 
    String(active_vehicle_config._accel_offset_x) + " Y: " +
    String(active_vehicle_config._accel_offset_y) + " Z: " +
    String(active_vehicle_config._accel_offset_x);

    // Create alert
    alerts_sensors.create_alert(e_alert_type::alert, "Calibration complete. Offsets Gyro X: " + offsets_string);


    accel_sum.x = 0;
    accel_sum.y = 0;
    accel_sum.z = 0;

    gyro_sum.x = 0;
    gyro_sum.y = 0;
    gyro_sum.z = 0;

    // Calculate deviation
    for (uint16_t i = 0; i < 400; i++)
    {
        // Accel
        accel_sum.x += sq(_sensors_imu_accel_cal_x[i] - active_vehicle_config._accel_offset_x);
        accel_sum.y += sq(_sensors_imu_accel_cal_y[i] - active_vehicle_config._accel_offset_y);
        accel_sum.z += sq(_sensors_imu_accel_cal_z[i] - active_vehicle_config._accel_offset_z);

        // Gyro
        gyro_sum.x += sq(_sensors_imu_gyro_cal_x[i] - active_vehicle_config._gyro_offset_x);
        gyro_sum.y += sq(_sensors_imu_gyro_cal_y[i] - active_vehicle_config._gyro_offset_y);
        gyro_sum.z += sq(_sensors_imu_gyro_cal_z[i] - active_vehicle_config._gyro_offset_z);
    }

    active_vehicle_config._gyro_standard_deviation_x = sqrtf(gyro_sum.x / 399.f);
    active_vehicle_config._gyro_standard_deviation_y = sqrtf(gyro_sum.y / 399.f);
    active_vehicle_config._gyro_standard_deviation_z = sqrtf(gyro_sum.z / 399.f);

    active_vehicle_config._accel_standard_deviation_x = sqrtf(accel_sum.x / 399.f);
    active_vehicle_config._accel_standard_deviation_y = sqrtf(accel_sum.y / 399.f);
    active_vehicle_config._accel_standard_deviation_z = sqrtf(accel_sum.z / 399.f);
}

void _imu_calibrate_update()
{
    // Update the calibration list
    // Accel
    _sensors_imu_accel_cal_x[_sensors_imu_calibration_list_index] = Sensors.raw_accel.x;
    _sensors_imu_accel_cal_y[_sensors_imu_calibration_list_index] = Sensors.raw_accel.y;
    _sensors_imu_accel_cal_z[_sensors_imu_calibration_list_index] = Sensors.raw_accel.z - Sensors.gravity;
    // Gyro
    _sensors_imu_gyro_cal_x[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity.x;
    _sensors_imu_gyro_cal_y[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity.y;
    _sensors_imu_gyro_cal_z[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity.z;

    // Integrate the calibration index
    _sensors_imu_calibration_list_index++;
}









void _baro_calibrate()
{
    if (!Booleans.sw_sensors_baro_enable_calibration) return;

    _sensors_baro_calibration_list_index >= 99 ? _baro_calibrate_calculate_deviation() : _baro_calibrate_update();
}

void _baro_calibrate_update()
{
    // Update the calibration list
    _sensors_baro_altitude_cal[_sensors_baro_calibration_list_index] = Sensors.raw_baro_altitude;

    // Integrate the calibration index
    _sensors_baro_calibration_list_index++;
}

void _baro_calibrate_calculate_deviation()
{
    // Disable calibration function endicating the end of the  
    Booleans.sw_sensors_baro_enable_calibration = false;

    float baro_sum;

    for (uint16_t i = 0; i < 100; i++)
    {
        baro_sum += _sensors_baro_altitude_cal[i];
    }
    
    // Calculate average deviation
    active_vehicle_config._baro_offset_altitude = baro_sum / 100.f;

    baro_sum = 0;
    // Calculate deviation
    for (uint16_t i = 0; i < 100; i++)
    {
        baro_sum += sq(_sensors_baro_altitude_cal[i] - active_vehicle_config._baro_offset_altitude);
    }

    active_vehicle_config._baro_standard_deviation_altitude = sqrtf(baro_sum / 99.f);

    // Create alert
    alerts_sensors.create_alert(e_alert_type::alert, "Calibration complete. Offsets altitude: " + String(active_vehicle_config._baro_offset_altitude) + " Standard deviation: " + String(active_vehicle_config._baro_standard_deviation_altitude));

    position_kalman.R = {
        active_vehicle_config._baro_standard_deviation_altitude
    };

    position_kalman.reset();
}
