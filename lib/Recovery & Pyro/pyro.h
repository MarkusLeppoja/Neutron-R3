#ifndef __PYRO_H__
#define __PYRO_H__

#include <Arduino.h>
#include <definitions.h>
#include <alerts.h>
#include <profiler.h>
    

extern uint64_t pyro_fire_start_time[];
extern uint64_t pyro_prev;


// @brief Initializes pyro channel pins
void pyro_begin();

// @brief Ensures that pyro channels get turned off after allowed on-time has exceeded
// @brief Here we don't check if pyro functionality is armed or not. Adding this check here can cause the pyro to be left ON in some cases
// @brief In example if a pyro is active while (boolean) pyro_armed_status was made false 
void update_pyro();

// @brief Activates pyro channel nr 1
void pyro_1_fire();

// @brief Activates pyro channel nr 2
void pyro_2_fire();

// @brief Deactivates pyro channel nr 1
void _pyro_1_deactivate();

// @brief Deactivates pyro channel nr 1
void _pyro_2_deactivate();
#endif