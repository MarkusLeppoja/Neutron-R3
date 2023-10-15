#ifndef __RECOVERY_H__
#define __RECOVERY_H__

#include <Arduino.h>
#include <Servo.h>

#include <definitions.h>
#include <alerts.h>
    


// @brief Attaches servo / begins pyro
void init_recovery();

// @brief Checks if servo is attached. Locks the servo at fds_servo_lock_angle.
void recovery_arm();

// @brief Checks if servo is attached. Locks the servo at fds_servo_release_angle.
void recovery_deploy_parachute();
#endif