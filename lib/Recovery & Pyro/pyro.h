#ifndef __PYRO_H__
#define __PYRO_H__

#include <Arduino.h>
#include <definitions.h>
#include <alerts.h>
    
extern uint64_t pyro_fire_start_time[];
extern uint64_t pyro_prev;

void update_pyro();
void pyro_1_fire();
void pyro_2_fire();
void _pyro_1_deactivate();
void _pyro_2_deactivate();
#endif