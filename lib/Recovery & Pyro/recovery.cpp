#include "recovery.h"

Servo fds_servo;
Alerts alerts_recovery("Recovery");


void init_recovery()
{
    if (active_vehicle_config.enable_fds_servo)
    {
        // Attach servo
        fds_servo.attach(e_pins::pin_servo_1);

        // Put the servo at release angle
        recovery_deploy_parachute();

        // Create alert
        alerts_recovery.create_alert(e_alert_type::success, "Recovery startup successful");
    }
}

void recovery_arm()
{
    // Return if fds functionality isn't enabled
    if (!active_vehicle_config.enable_fds_servo) return;

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

    //
    alerts_recovery.create_alert(e_alert_type::success, "FDS (Fairing Deployment System) Servo released");
}
