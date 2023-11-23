#ifndef __STATE_LOGIC_H__
#define __STATE_LOGIC_H__

#include <definitions.h>
#include <state_estimation.h>
#include <data_recorder.h>
#include <indicator.h>
#include <recovery.h>
#include <profiler.h>
#include <alerts.h>
#include <pyro.h>
#include <coms.h>

void set_mission_state(e_mission_state state);
void _on_mission_state_change(e_mission_state new_state);

// @brief Basicly a main loop function
void update_state_machine();

// @brief State detect function
void detect_launch();
// @brief State detect function
void detect_apogee();
// @brief State detect function
void detect_landed();

boolean detect_abort();

// @brief Compares mission time against given value
// @note The value is compared against mission duration which is in seconds
boolean has_time_elipsed(float greater_than);

#endif  