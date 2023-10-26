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


extern uint64_t _state_machine_prev, _state_begin_time;
extern uint16_t _detect_state_counter_1, _detect_state_counter_2;
extern float _state_duration;
extern boolean _has_calibration_started;


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

// @brief Compares mission time against given value
// @note The value is compared against mission duration which is in seconds
boolean has_time_elipsed(float greater_than);

#endif  