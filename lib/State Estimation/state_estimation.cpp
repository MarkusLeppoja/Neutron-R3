#include "state_estimation.h"

// Instances
SPIClassRP2040 spi_sensors_instance(spi0, e_pins::pin_spi_miso, e_pins::pin_cs_flash, e_pins::pin_spi_sck, e_pins::pin_spi_mosi);
TwoWire i2c_sensors_instance(&i2c1_inst, e_pins::pin_i2c_sda, e_pins::pin_i2c_scl);


Bmi088Accel accel_instance(i2c_sensors_instance, 0x18);
Bmi088Gyro gyro_instance(i2c_sensors_instance, 0x68);
BMP388_DEV baro_instance(i2c_sensors_instance);

Alerts alerts_sensors("State Estimation");

// Performance profilers (TODO:)
Profiler profiler_imu;
Profiler profiler_baro;
Profiler profiler_gnss;
Profiler profiler_mag;
Profiler profiler_v_divider;




uint64_t _imu_update_prev, _baro_update_prev, _gnss_update_prev, _v_divider_update_prev, _mag_update_prev;



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

    // Profiler
    profiler_imu.end_function(Sensors.profiler_imu_function_duration);
}

void _accel_update()
{
    accel_instance.readSensor();

    // Read sensors

    // Put them in correct cordinate frame (TODO:)
    Sensors.raw_accel.x = accel_instance.getAccelX_mss();
    Sensors.raw_accel.y = accel_instance.getAccelY_mss();
    Sensors.raw_accel.z = accel_instance.getAccelZ_mss();

    Sensors.raw_accel_temp = accel_instance.getTemperature_C();
    //TODO:
}

void _gyro_update()
{
    gyro_instance.readSensor();

    // Put them in correct cordinate frame (TODO:)
    Sensors.raw_gyro_velocity.x = gyro_instance.getGyroX_rads();
    Sensors.raw_gyro_velocity.y = gyro_instance.getGyroY_rads();
    Sensors.raw_gyro_velocity.z = gyro_instance.getGyroZ_rads();

    //TODO:
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
    // TODO:

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



int sensors_init()
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
    uint8_t gnss_status;// = _gnss_begin(active_vehicle_config.enable_gnss_debug);
    _v_divider_begin();

    //TODO: FINISH



    // If all sensors didn't initialize correctly create alert and disable sensors functionality
    if ((baro_status + imu_status + gnss_status) != 3)
    {
        alerts_sensors.create_alert(e_alert_type::error, "Sensors startup unsuccessful");
        Booleans.sw_sensors_usability = false;

        return 0; 
    }



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
    _v_divider_update();
}//dav, ma lahen ja loodin oma põrandat edasi nüüd.