#ifndef __RECOVERY_H__
#define __RECOVERY_H__

#include <Arduino.h>
#include <definitions.h>
#include <Servo.h>
#include <pyro.h>
#include <alerts.h>
    
//TODO: Create custom function to set the servo at specific degree, or have it rotate 360 degrees

// @brief Attaches servo / begins pyro
void recovery_begin();

// @brief Checks if servo is attached. Locks the servo at fds_servo_lock_angle.
void recovery_arm(int value = 0);

// @brief Checks if servo is attached. Locks the servo at fds_servo_release_angle.
void recovery_deploy_parachute();
#endif