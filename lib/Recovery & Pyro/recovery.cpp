#include "recovery.h"

Servo fds_servo;
Alerts alerts_recovery("Recovery");



void recovery_begin()
{
    if (!active_vehicle_config.enable_fds_servo) return;
    
    // Attach servo
    fds_servo.attach(e_pins::pin_servo_3);

    // Put the servo at release angle
    recovery_deploy_parachute();

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "Recovery startup successful");
    
}

void recovery_arm(int value)
{
    // Return if fds functionality isn't enabled
    if (!active_vehicle_config.enable_fds_servo) return;

    // If custom angle is selected 
    if (value != 0)
    {
        fds_servo.write(value);
        return;
    }
    
    // Turn servo to fds_servo_lock_angle
    fds_servo.write(active_vehicle_config.fds_servo_lock_angle);


    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "FDS (Fairing Deployment System) Servo locked");
}

void recovery_deploy_parachute()
{
    // Return if fds functionality isn't enabled
    if (!active_vehicle_config.enable_fds_servo) return;

    // Turn servo to fds_servo_release_angle
    fds_servo.write(active_vehicle_config.fds_servo_release_angle);

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "FDS (Fairing Deployment System) Servo released");
}
