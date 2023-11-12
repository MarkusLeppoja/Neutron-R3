#include "recovery.h"


Servo fds_servo;
Alerts alerts_recovery("Recovery");


void recovery_begin()
{
    if (!get_active_config().enable_recovery || !get_active_config().enable_servo) return;
    
    // Attach servo
    fds_servo.attach(e_pins::pin_servo_3);

    // Put the servo at release angle
    recovery_deploy_parachute();

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "Recovery startup successful");
}

void recovery_arm()
{
    // Return if fds functionality isn't enabled
    if (!get_active_config().enable_recovery || !get_active_config().enable_servo) return;
    
    // Turn servo to fds_servo_lock_angle
    fds_servo.write(get_active_config().fds_servo_lock_angle);

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "Fairing Deployment System Servo locked");
}

void recovery_deploy_parachute()
{
    // Return if fds functionality isn't enabled
    if (!get_active_config().enable_recovery || !get_active_config().enable_servo) return;
    // Turn servo to fds_servo_release_angle
    fds_servo.write(get_active_config().fds_servo_release_angle);

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "Fairing Deployment System Servo released");
}

void recovery_set_servo_angle(int angle)
{
    // Return if fds functionality isn't enabled
    if (!get_active_config().enable_recovery || !get_active_config().enable_servo) return;

    // Turn servo to angle
    fds_servo.write(angle);

    // Create alert
    alerts_recovery.create_alert(e_alert_type::success, "Servo angle set at: " + String(angle));
}