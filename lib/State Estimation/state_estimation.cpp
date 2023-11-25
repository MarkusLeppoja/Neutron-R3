#include "state_estimation.h"
#include "filters.h"

#define RADS_TO_DEG 57.29578

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
Profiler profiler_v_divider;

// Local variables
uint64_t _imu_update_prev, _baro_update_prev, _v_divider_update_prev, _accel_prev, _gyro_prev;
double _sensors_imu_accel_cal_x[400], _sensors_imu_accel_cal_y[400], _sensors_imu_accel_cal_z[400];
double _sensors_imu_gyro_cal_x[400], _sensors_imu_gyro_cal_y[400], _sensors_imu_gyro_cal_z[400];
double _sensors_baro_altitude_cal[100];
int _sensors_imu_calibration_list_index, _sensors_baro_calibration_list_index;

//position_kalman_filter kf_position;
kalman kf_x_gyro_vel, kf_y_gyro_vel, kf_z_gyro_vel;
kalman kf_x_accel, kf_y_accel, kf_z_accel;

// Orientation
Orientation ori_quat, new_quat, quat_world_accel;
EulerAngles ori_euler;


int _baro_begin()
{
    if (!active_vehicle_config.enable_baro)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "Baro is disabled");
        Booleans.sw_sensors_baro_usability = 0;
        return 1;
    }

    uint8_t baro_init_status = baro_instance.begin(0x76);
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
    alerts_sensors.create_alert(e_alert_type::success, "Baro startup was successful");

    // Update prev to current time
    _baro_update_prev = micros();

    return 1;
}

int _imu_begin()
{
    // Disable functionality if enable_imu is false
    if (!active_vehicle_config.enable_imu)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "IMU is disabled");
        Booleans.sw_sensors_imu_usability = 0;
        return 0;
    }

    // Begin imu and save its status
    int imu_accel_init_status = accel_instance.begin();
    int imu_gyro_init_status = gyro_instance.begin();

    // If startup failed return, create alert and disable usability
    if (imu_accel_init_status + imu_gyro_init_status != 2)
    {
        alerts_sensors.create_alert(e_alert_type::error, "Failed to connect with BMI088. Accel status: " + String(imu_accel_init_status) + " Gyro status:" + String(imu_gyro_init_status) + ". Disabling IMU functionality");
        Booleans.sw_sensors_imu_usability = 0;
        return 0;
    }

    // Configure
    accel_instance.setRange(Bmi088Accel::RANGE_24G);
    accel_instance.setOdr(Bmi088Accel::ODR_200HZ_BW_80HZ);
    gyro_instance.setRange(Bmi088Gyro::RANGE_2000DPS);
    gyro_instance.setOdr(Bmi088Gyro::ODR_200HZ_BW_64HZ);

    kf_x_gyro_vel.init(0.1f, 0.1f);
    kf_y_gyro_vel.init(0.1f, 0.1f);
    kf_z_gyro_vel.init(0.1f, 0.1f);
    kf_x_accel.init(0.1f, 0.01f);
    kf_y_accel.init(0.1f, 0.01f);
    kf_z_accel.init(0.1f, 0.01f);

    // Enable imu functionality and create alert
    Booleans.sw_sensors_imu_usability = 1;
    alerts_sensors.create_alert(e_alert_type::success, "IMU startup was successful");

    // Update prev to current time
    _imu_update_prev = micros();

    return 1;
}

void _v_divider_begin()
{
    // Set the correct pinmode
    pinMode(e_pins::pin_voltage_divider, INPUT);

    alerts_sensors.create_alert(e_alert_type::success, "V-Divider startup was successful");

    // Update prev to current time
    _v_divider_update_prev = micros();
}

void _imu_update()
{
   update_mcu_clock();
   profiler_imu.begin_loop();

   if (Clock.microseconds - _imu_update_prev < 5000.f) return;
    _imu_update_prev = Clock.microseconds;

    // Check if sensor is supposed to be used
    if (!active_vehicle_config.enable_imu || !Booleans.sw_sensors_imu_usability) return;

    // Profiler
    Code_performance.imu_loop = profiler_imu.end_loop();
    profiler_imu.begin_function();

    // Update sensors seperately
    _accel_update();
    _gyro_update();

    // Update kalman filter
    kf_position.predict_accel(Sensors.acceleration.z - active_vehicle_config.gravity);
    
    // Get kalman filter data
    Sensors.position.z = kf_position.get_position();
    Sensors.velocity.z = kf_position.get_velocity();

    // Finds offset and standard deviation if enabled
    _imu_calibrate();

    // Profiler
    Code_performance.imu_function_duration = profiler_imu.end_function();
}

void _accel_update()
{
    accel_instance.readSensor();

    // Calculate dt
    double accel_dt = (micros() - _accel_prev) / 1000000.f;
    _accel_prev = micros();

    Sensors.raw_acceleration.x = accel_instance.getAccelY_mss();
    Sensors.raw_acceleration.y = -accel_instance.getAccelX_mss();
    Sensors.raw_acceleration.z = -accel_instance.getAccelZ_mss();

    Sensors.raw_acceleometer_temperature = accel_instance.getTemperature_C();
    
    Sensors.acceleration.x = kf_x_accel.update_estimate(Sensors.raw_acceleration.x - Sensors._accel_offset_x);
    Sensors.acceleration.y = kf_y_accel.update_estimate(Sensors.raw_acceleration.y - Sensors._accel_offset_y);
    Sensors.acceleration.z = kf_z_accel.update_estimate(Sensors.raw_acceleration.z - Sensors._accel_offset_z);


    //This portion of code is taken from AdamMarciniak Cygnus-X1-Software github repository (NOT USING THIS DUE TO LACK OF UNDERSTANDING HOW WORLD ACCELERATION CALCULATIONS EXACTLY WORKS)
    //https://github.com/AdamMarciniak/Cygnus-X1-Software/blob/master/src/Nav.cpp 

    // Calculates the world acceleration
    //quat_world_accel.orientation = quat.orientation.rotate(Quaternion(0, Sensors.localAccel.z, Sensors.localAccel.y, Sensors.localAccel.x));
    //quat_world_accel.orientation = Quaternion(0, Sensors.localAccel.z, Sensors.localAccel.x, -Sensors.localAccel.y).operator*=(quat.orientation);

    /*double norm = sqrt(sq(Sensors.gyro_velocity.x) + sq(Sensors.gyro_velocity.y) + sq(Sensors.gyro_velocity.z));
    norm = copysign(max(abs(norm), 1e-9), norm); 

    // De-biasing
    //new_quat.orientation.rotate(Quaternion(0.0,0, Sensors.accelOri.y, Sensors.accelOri.x));

    new_quat.orientation *= new_quat.orientation.from_axis_angle(accel_dt * norm, Sensors.gyro_velocity.x / norm, Sensors.gyro_velocity.y / norm, Sensors.gyro_velocity.z / norm);
    quat_world_accel.orientation = new_quat.orientation.rotate(Quaternion(0.0, -Sensors.raw_accel.y, Sensors.raw_accel.x, Sensors.raw_accel.z));

    Sensors.world_accel.x = quat_world_accel.orientation.c;
    Sensors.world_accel.y = quat_world_accel.orientation.b;
    Sensors.world_accel.z = quat_world_accel.orientation.d;*/
}

void _gyro_update()
{
    gyro_instance.readSensor();

    // Calculate dt
    double gyro_dt = (micros() - _gyro_prev) / 1000000.f;
    _gyro_prev = micros();

    Sensors.raw_gyro_velocity_rps.x = -gyro_instance.getGyroX_rads();
    Sensors.raw_gyro_velocity_rps.y = -gyro_instance.getGyroY_rads();
    Sensors.raw_gyro_velocity_rps.z = -gyro_instance.getGyroZ_rads();

    Sensors.gyro_velocity_rps.x = kf_x_gyro_vel.update_estimate(Sensors.raw_gyro_velocity_rps.x - Sensors._gyro_offset_x);
    Sensors.gyro_velocity_rps.y = kf_y_gyro_vel.update_estimate(Sensors.raw_gyro_velocity_rps.y - Sensors._gyro_offset_y);
    Sensors.gyro_velocity_rps.z = kf_z_gyro_vel.update_estimate(Sensors.raw_gyro_velocity_rps.z - Sensors._gyro_offset_z);

    Sensors.gyro_velocity_dps.x = Sensors.gyro_velocity_rps.x * RAD_TO_DEG;
    Sensors.gyro_velocity_dps.y = Sensors.gyro_velocity_rps.y * RAD_TO_DEG;
    Sensors.gyro_velocity_dps.z = Sensors.gyro_velocity_rps.z * RAD_TO_DEG;

    ori_quat.update(Sensors.gyro_velocity_rps.x, Sensors.gyro_velocity_rps.y, Sensors.gyro_velocity_rps.z, gyro_dt);
    
    // Convert quaternion to euler angle
    ori_euler = ori_quat.toEuler();
    Sensors.orientation.x = ori_euler.yaw * RAD_TO_DEG;
    Sensors.orientation.y = ori_euler.pitch * RAD_TO_DEG;
    Sensors.orientation.z = ori_euler.roll * RAD_TO_DEG;
}

void _baro_update()
{
    update_mcu_clock();
    profiler_baro.begin_loop();

    if (Clock.microseconds - _baro_update_prev < 20000.f) return;
    _baro_update_prev = Clock.microseconds;

    // Check is sensor is supposed to be used
    if (!active_vehicle_config.enable_baro || !Booleans.sw_sensors_baro_usability) return;

    // Profiler
    Code_performance.baro_loop = profiler_baro.end_loop();
    profiler_baro.begin_function();

    // Call for new data
    baro_instance.getMeasurements(Sensors._f_raw_baro_temperature, Sensors._f_raw_baro_pressure, Sensors._f_raw_baro_altitude);
    Sensors.raw_baro_temperature = Sensors._f_raw_baro_temperature;
    Sensors.raw_baro_pressure = Sensors._f_raw_baro_pressure;
    Sensors.raw_baro_altitude = Sensors._f_raw_baro_altitude;

    // Create a variable which has the altitude without sensor bias
    Sensors.raw_baro_altitude_wo_bias = Sensors.raw_baro_altitude - Sensors._baro_offset_altitude;

    // Update apogee altitude only during mission
    if (is_mission_active() && Sensors.position.z - Sensors.apogee_altitude >= 0.20){
        Sensors.apogee_altitude = Sensors.position.z;
    }

    // Update kalman filter
    kf_position.update_baro(Sensors.raw_baro_altitude_wo_bias);

    // Finds offset and standard deviation if enabled
    _baro_calibrate();

    // Profiler
    Code_performance.baro_function_duration = profiler_baro.end_function();
}

void _v_divider_update()
{
    update_mcu_clock();
    profiler_v_divider.begin_loop();

    if (Clock.microseconds - _v_divider_update_prev < 50000.f || !active_vehicle_config.enable_voltage_divider) return;
    _v_divider_update_prev = Clock.microseconds;

    // Profiler
    Code_performance.voltage_divider_loop = profiler_v_divider.end_loop();
    profiler_v_divider.begin_function();

    Sensors.internal_voltage = (double) analogRead(e_pins::pin_voltage_divider) * active_vehicle_config.voltage_divider_ratio;

    // Profiler
    Code_performance.voltage_divider_function_duration = profiler_v_divider.end_function();
}

int sensors_begin()
{
    if (!active_vehicle_config.enable_sensors)
    {
        alerts_sensors.create_alert(e_alert_type::warning, "Sensors are disabled");
        Booleans.sw_sensors_usability = 0;
        return 0;
    }
    
    // Sets the i2c clock rate to around 400 kHz 
    i2c_sensors_instance.setClock(1000000);

    // Begin sensors and save their status
    boolean baro_status = _baro_begin();
    boolean imu_status = _imu_begin();
    _v_divider_begin();

    // If all sensors didn't initialize correctly create alert and disable sensors functionality
    if ((baro_status + imu_status) != 2)
    {
        alerts_sensors.create_alert(e_alert_type::error, "Sensors startup was unsuccessful");
        Booleans.sw_sensors_usability = 0;
        return 0; 
    }

    // Indicate successful startup and enable sensors usability
    alerts_sensors.create_alert(e_alert_type::success, "Sensors startup was successful");
    Booleans.sw_sensors_usability = 1;

    return 1;
}

void update_sensors()
{
    if (!active_vehicle_config.enable_sensors || !Booleans.sw_sensors_usability) return;
    
    _imu_update();
    _baro_update();
    _v_divider_update();
    //dav, ma lahen ja loodin nüüd oma põrandat edasi.
} 

void reset_ori()
{
    ori_quat.reset();
    alerts_sensors.create_alert(e_alert_type::alert, "Orientation quaternion was reset");
}


void reset_position_kalman()
{
    kf_position.reset();
    alerts_sensors.create_alert(e_alert_type::alert, "Position Kalman Filter was reset");
}

void set_kf_position_R(float value)
{
    kf_position.R = {value};
}


void imu_enable_calibration()
{
    Booleans.sw_sensors_imu_enable_calibration = 1;
}

void baro_enable_calibration()
{
    Booleans.sw_sensors_baro_enable_calibration = 1;
}

void _imu_calibrate()
{
    if (!Booleans.sw_sensors_imu_enable_calibration) return;
    _sensors_imu_calibration_list_index > 399 ? _imu_calibrate_calculate_deviation() : _imu_calibrate_update();
}

void _imu_calibrate_calculate_deviation()
{
    // Disable calibration function endicating the end of the  
    Booleans.sw_sensors_imu_enable_calibration = 0;

    vector_3d accel_sum, gyro_sum;

    for (int i = 0; i < 400; i++)
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
    Sensors._gyro_offset_x = gyro_sum.x / 400.f;
    Sensors._gyro_offset_y = gyro_sum.y / 400.f;
    Sensors._gyro_offset_z = gyro_sum.z / 400.f;
    Sensors._accel_offset_x = accel_sum.x / 400.f;
    Sensors._accel_offset_y = accel_sum.y / 400.f;
    Sensors._accel_offset_z = accel_sum.z / 400.f;

    // Reset variables
    accel_sum.x = accel_sum.y = accel_sum.z = 0;
    gyro_sum.x = gyro_sum.y = gyro_sum.z = 0;

    for (int i = 0; i < 400; i++)
    {
        // Accel
        accel_sum.x += _sensors_imu_accel_cal_x[i] = _sensors_imu_accel_cal_x[i] - Sensors._accel_offset_x;
        accel_sum.y += _sensors_imu_accel_cal_y[i] = _sensors_imu_accel_cal_y[i] - Sensors._accel_offset_y;
        accel_sum.z += _sensors_imu_accel_cal_z[i] = _sensors_imu_accel_cal_z[i] - Sensors._accel_offset_z;
        // Gyro
        gyro_sum.x += _sensors_imu_gyro_cal_x[i] = _sensors_imu_gyro_cal_x[i] - Sensors._gyro_offset_x;
        gyro_sum.y += _sensors_imu_gyro_cal_y[i] = _sensors_imu_gyro_cal_y[i] - Sensors._gyro_offset_y;
        gyro_sum.z += _sensors_imu_gyro_cal_z[i] = _sensors_imu_gyro_cal_z[i] - Sensors._gyro_offset_z;
    }

    double accel_avg_x = accel_sum.x / 400.f;
    double accel_avg_y = accel_sum.y / 400.f;
    double accel_avg_z = accel_sum.z / 400.f;
    double gyro_avg_x = gyro_sum.x / 400.f;
    double gyro_avg_y = gyro_sum.y / 400.f;
    double gyro_avg_z = gyro_sum.z / 400.f;

    // Reset variables
    accel_sum.x = accel_sum.y = accel_sum.z = 0;
    gyro_sum.x = gyro_sum.y = gyro_sum.z = 0;

    // Calculate deviation
    for (int i = 0; i < 400; i++)
    {
        // Accel
        accel_sum.x += sq(_sensors_imu_accel_cal_x[i] - accel_avg_x);
        accel_sum.y += sq(_sensors_imu_accel_cal_y[i] - accel_avg_y);
        accel_sum.z += sq(_sensors_imu_accel_cal_z[i] - accel_avg_z);
        // Gyro
        gyro_sum.x += sq(_sensors_imu_gyro_cal_x[i] - gyro_avg_x);
        gyro_sum.y += sq(_sensors_imu_gyro_cal_y[i] - gyro_avg_y);
        gyro_sum.z += sq(_sensors_imu_gyro_cal_z[i] - gyro_avg_z);
    }

    Sensors._gyro_standard_deviation_x = sqrtf(gyro_sum.x / 400.f);
    Sensors._gyro_standard_deviation_y = sqrtf(gyro_sum.y / 400.f);
    Sensors._gyro_standard_deviation_z = sqrtf(gyro_sum.z / 400.f);

    Sensors._accel_standard_deviation_x = sqrtf(accel_sum.x / 400.f);
    Sensors._accel_standard_deviation_y = sqrtf(accel_sum.y / 400.f);
    Sensors._accel_standard_deviation_z = sqrtf(accel_sum.z / 400.f);

    // Create notification
    String offsets_string = 
    String(Sensors._gyro_offset_x) + " Y: " +
    String(Sensors._gyro_offset_y) + " Z: " +
    String(Sensors._gyro_offset_z) + " Accel X: " + 
    String(Sensors._accel_offset_x) + " Y: " +
    String(Sensors._accel_offset_y) + " Z: " +
    String(Sensors._accel_offset_z);

    String deviations_string = 
    String(Sensors._gyro_standard_deviation_x) + " Y: " +
    String(Sensors._gyro_standard_deviation_y) + " Z: " +
    String(Sensors._gyro_standard_deviation_z) + " Accel X: " + 
    String(Sensors._accel_standard_deviation_x) + " Y: " +
    String(Sensors._accel_standard_deviation_y) + " Z: " +
    String(Sensors._accel_standard_deviation_z);

    // Reset orientation after calibrating
    reset_ori();

    alerts_sensors.create_alert(e_alert_type::alert, "IMU calibration complete. Offsets Gyro X: " + offsets_string + " Standard Deviation Gyro X: " + deviations_string);
}

void _imu_calibrate_update()
{
    // Update the calibration list
    // Accel
    _sensors_imu_accel_cal_x[_sensors_imu_calibration_list_index] = Sensors.raw_acceleration.x;
    _sensors_imu_accel_cal_y[_sensors_imu_calibration_list_index] = Sensors.raw_acceleration.y;
    _sensors_imu_accel_cal_z[_sensors_imu_calibration_list_index] = Sensors.raw_acceleration.z - active_vehicle_config.gravity;
    // Gyro
    _sensors_imu_gyro_cal_x[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity_rps.x;
    _sensors_imu_gyro_cal_y[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity_rps.y;
    _sensors_imu_gyro_cal_z[_sensors_imu_calibration_list_index] = Sensors.raw_gyro_velocity_rps.z;

    // Integrate the calibration index
    _sensors_imu_calibration_list_index++;
}



void _baro_calibrate()
{
    if (!Booleans.sw_sensors_baro_enable_calibration) return;

    _sensors_baro_calibration_list_index > 99 ? _baro_calibrate_calculate_offset() : _baro_calibrate_update();
}

void _baro_calibrate_update()
{
    // Update the calibration list
    _sensors_baro_altitude_cal[_sensors_baro_calibration_list_index] = Sensors.raw_baro_altitude_wo_bias;

    // Integrate the calibration index
    _sensors_baro_calibration_list_index++;
}

void _baro_calibrate_calculate_offset()
{
    // Calculate average bias
    double baro_sum, baro_average;
    for (int i = 0; i < 100; i++)
    {
        baro_sum += _sensors_baro_altitude_cal[i];
    }
    
    Sensors._baro_offset_altitude = baro_sum / 100.f; 
    
    // Calculate average error without bias
    baro_sum = 0;
    for (int i = 0; i < 100; i++)
    {
        baro_sum += _sensors_baro_altitude_cal[i] = _sensors_baro_altitude_cal[i] - Sensors._baro_offset_altitude;
    }

    baro_average = baro_sum / 100.f;
    baro_sum = 0;
    for (int i = 0; i < 100; i++)
    {
        baro_sum += sq(_sensors_baro_altitude_cal[i] - baro_average);
    }

    Sensors._baro_standard_deviation_altitude = sqrt(baro_sum / 100.f);
    
    // Set standard deviation in kalman filter
    kf_position.R = {
        (float)sq(Sensors._baro_standard_deviation_altitude) //TODO: remove SQ. It was for a test
    };

    // After calibrating reset position kalman filter
    reset_position_kalman();

    // Create alert
    alerts_sensors.create_alert(e_alert_type::alert, "Baro calibration complete. Offsets altitude: " + String(Sensors._baro_offset_altitude) + " Standard deviation: " + String(Sensors._baro_standard_deviation_altitude));

    // Disable calibration
    Booleans.sw_sensors_baro_enable_calibration = 0;
}
