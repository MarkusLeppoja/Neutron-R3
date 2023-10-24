#ifndef __RECOVERY_H__
#define __RECOVERY_H__

#include <Arduino.h>
#include <definitions.h>
#include <Servo.h>
#include <alerts.h>
    

// @brief Attaches servo / begins pyro
void recovery_begin();

// @brief Checks if servo is attached. Locks the servo at fds_servo_lock_angle.
void recovery_arm();

// @brief Checks if servo is attached. Locks the servo at fds_servo_release_angle.
void recovery_deploy_parachute();

// @brief Set servo at a specific angle 
void recovery_set_servo_angle(int angle);
#endif