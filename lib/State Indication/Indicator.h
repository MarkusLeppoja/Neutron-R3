#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#include <Arduino.h>
#include <definitions.h>


// @brief This class contains all code for visual / audio indication for Neutron R3
// @note This mostly legacy code from Neutron R2 & Neutron R3 DEV rename & improved

//TODO: Test if special indicators work
enum e_event_options {
    null,
    event_command_received,
    event_usb_connection_changed,
    event_mission_abort,
    event_error
};
extern e_event_options _indicator_event_queue[];
extern boolean _indicator_indication_power_on_status;
extern float _indicator_counter;
extern uint64_t indicator_prev;

// @brief Initializes the led & buzzer pins
void indicator_begin();

// @brief Returns the event queue instance
e_event_options *get_event_queue();

// @brief Returns true if power on indication has ended
boolean has_power_on_indication_ended();

// @brief Enables the power on indication to be played again
void reset_power_on_indication();

// @brief Returns true if event queue is empty
boolean is_event_queue_empty();

// @brief Updates special indication position in the queue
void update_special_indicator_queue();

// @brief If free space is present adds given event to the queue
void add_special_indicator_to_queue(e_event_options event);

// @brief Removes the first indication from queue and updates it
void remove_latest_special_indicator_from_queue();

// @brief General function, controls logic behind indicator & special events
void update_indicator();

// @brief Handels indication the correct special indication
void _indicate_special_events();

// @brief Handels indication the correct indication
void _indicate_mission_state();

// @brief Checks if value is equal or between the given value
boolean _indicator_is_value_between(float value, float min, float max);

// @brief Change the tone of the internal buzzer
// @note If enable_buzzer or power_status is false, the buzzer will be turned off
void set_buzzer_tone(uint16_t freq, boolean power_status);

// @brief Change the color of the internal rgb led
void set_rgb_led_color(boolean red, boolean green, boolean blue);

/* Indications */

void _indicator_startup();
void _indicator_navigation_startup();
void _indicator_ground_lock();
void _indicator_pad_idle();
void _indicator_ascent();
void _indicator_ballistic_decent();
void _indicator_chute_decent();
void _indicator_landed();
void _indicator_startup_fail();
void _indicator_event_command_received();
void _indicator_event_usb_connection_changed();
void _indicator_event_mission_abort();
void _indicator_event_error();
#endif  