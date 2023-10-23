#include "indicator.h"

e_event_options _indicator_event_queue[3];
boolean _indicator_indication_power_on_status;
float _indicator_counter;
uint64_t indicator_prev;



void indicator_begin()
{
    pinMode(e_pins::pin_led_rgb_r, OUTPUT);
    pinMode(e_pins::pin_led_rgb_g, OUTPUT);
    pinMode(e_pins::pin_led_rgb_b, OUTPUT);
}

e_event_options *get_event_queue()
{
    return _indicator_event_queue;
}

boolean has_power_on_indication_ended()
{
    return _indicator_indication_power_on_status;
}

void reset_power_on_indication()
{
    _indicator_indication_power_on_status = true;
}

boolean is_event_queue_empty()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        if (_indicator_event_queue[i] != e_event_options::null) 
        {
            update_special_indicator_queue();
            return false;
        }
    }
    return true;
}

void update_special_indicator_queue()
{
    // Checks if current slot is empty, if so then move the indicator queue position 
    for (uint8_t i = 0; i < 2; i++)
    {
        if (_indicator_event_queue[i] == e_event_options::null)
        {
            _indicator_event_queue[i] = _indicator_event_queue[i+1];
            _indicator_event_queue[i+1] = e_event_options::null;
        }
    }
}

void add_special_indicator_to_queue(e_event_options event)
{
    update_special_indicator_queue();

    // Add given event to the queue if empty slot is present
    for (uint8_t i = 0; i < 3; i++)
    {
        if (_indicator_event_queue[i] == e_event_options::null)
        {
            // Reset the indicators counter. (Assures that the indication starts at the beginning)
            if (i == 0) _indicator_counter = 0; 

            _indicator_event_queue[i] = event;
            break;
        }
    }
}

void remove_latest_special_indicator_from_queue()
{
    _indicator_event_queue[0] = e_event_options::null;
    update_special_indicator_queue();
}

void update_indicator()
{
    update_mcu_clock();

    if (Clock.microseconds - indicator_prev < 50000 || !active_vehicle_config.enable_indicator) return;
    indicator_prev = Clock.microseconds; 
    _indicator_counter++;

    // Indicates mission state unless event queue contains special events
    is_event_queue_empty ? _indicate_mission_state() : _indicate_special_events();
}









void  _indicate_special_events()
{
    switch (get_event_queue()[0])
    {
    case e_event_options::event_command_received:
        _indicator_event_command_received();
        break;
    case e_event_options::event_usb_connection_changed:
        _indicator_event_usb_connection_changed();
        break;
    case e_event_options::event_mission_abort:
        _indicator_event_mission_abort();
        break;
    case e_event_options::event_error:
        _indicator_event_error();
        break;
    default:
        break;
    }
}

void _indicate_mission_state()
{
    switch (get_mission_state())
    {
    case e_mission_state::startup:
        _indicator_startup();
        break;
    case e_mission_state::navigation_startup:
        _indicator_navigation_startup();
        break;
    case e_mission_state::ground_locked:
        _indicator_ground_lock();
        break;
    case e_mission_state::pad_idle:
        _indicator_pad_idle();
        break;
        break;
    case e_mission_state::chute_decent:
        _indicator_chute_decent();
        break;
    case e_mission_state::landed:
        _indicator_landed();
        break;
    case e_mission_state::startup_failed:
        _indicator_startup_fail();
        break;
    default:
        break;
    }
}

boolean _indicator_is_value_between(float value, float min, float max)
{
    return (value > min && value <= max) ? true : false;
}

void set_buzzer_tone(uint16_t freq, boolean power_status)
{
    if (!active_vehicle_config.enable_buzzer || !power_status)
    {
        noTone(e_pins::pin_buzzer);
        return;
    }
    tone(e_pins::pin_buzzer, freq);
}

void set_rgb_led_color(boolean red, boolean green, boolean blue)
{
    if (!active_vehicle_config.enable_led)
    {
        digitalWrite(e_pins::pin_led_rgb_r, 1);
        digitalWrite(e_pins::pin_led_rgb_g, 1);
        digitalWrite(e_pins::pin_led_rgb_b, 1);
        return;
    }
    digitalWrite(e_pins::pin_led_rgb_r, red);
    digitalWrite(e_pins::pin_led_rgb_g, green);
    digitalWrite(e_pins::pin_led_rgb_b, blue);
}




/* Indications */




void _indicator_startup()
{
    if (has_power_on_indication_ended()) return;

    if (_indicator_is_value_between(_indicator_counter, 8, 12))
    {
        set_rgb_led_color(0, 1, 1);
        set_buzzer_tone(2000, true);
    }
    else if (_indicator_is_value_between(_indicator_counter, 12, 18))
    {
        set_buzzer_tone(2200, true);
    }
    else if (_indicator_is_value_between(_indicator_counter, 16, 30))
    {
        set_buzzer_tone(2550, true);
    }
    else if (_indicator_counter >= 30)
    {
        _indicator_counter = 0;
        _indicator_indication_power_on_status = true;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_navigation_startup()
{
    if (_indicator_is_value_between(_indicator_counter, 8, 14))
    {
        set_rgb_led_color(1, 1, 0);
        set_buzzer_tone(2300, true);
    }
    else if (_indicator_is_value_between(_indicator_counter, 14, 18))
    {
        set_buzzer_tone(0, false);
    }
    else if (_indicator_is_value_between(_indicator_counter, 18, 30))
    {
        set_rgb_led_color(0, 0, 0);
        set_buzzer_tone(2400, true);
    }
    else if (_indicator_counter >= 30)
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_ground_lock()
{
    if (_indicator_is_value_between(_indicator_counter,9,16))
    {
        set_rgb_led_color(0, 0, 0);
        set_buzzer_tone(3400, true);
    }
    else if  (_indicator_is_value_between(_indicator_counter,15,18))
    {
        set_rgb_led_color(0, 1, 0);
        set_buzzer_tone(3200, true);
    }
    else if (_indicator_is_value_between(_indicator_counter,17,20))
    {
        set_rgb_led_color(1,1,1);
        set_buzzer_tone(3600, true);
    }
    else if (_indicator_counter >= 20)
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_pad_idle()
{
    if (_indicator_is_value_between(_indicator_counter,9,16))
    {
        set_buzzer_tone(1800, true);
        set_rgb_led_color(1, 0, 0);
    }
    else if (_indicator_is_value_between(_indicator_counter,16,100))
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_ascent()
{
    if (_indicator_is_value_between(_indicator_counter,4,11))
    {
        set_rgb_led_color(1, 1, 0);
    }
    else if (_indicator_is_value_between(_indicator_counter,10,100))
    {
        _indicator_counter = 0;
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_ballistic_decent()
{
    if (_indicator_is_value_between(_indicator_counter,9,16))
    {
        set_buzzer_tone(2600, true);
        set_rgb_led_color(1, 0, 0);
    }
    else if (_indicator_is_value_between(_indicator_counter,14,100))
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_chute_decent()
{
    if (_indicator_is_value_between(_indicator_counter,4,11))
    {
        set_buzzer_tone(2800, true);
        set_rgb_led_color(1, 1, 0);
    }
    else if (_indicator_is_value_between(_indicator_counter,9,100))
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_landed()
{
    if (_indicator_is_value_between(_indicator_counter,4,20))
    {
        set_buzzer_tone(3000, true);
        set_rgb_led_color(0, 1, 0);
    }
    else if (_indicator_is_value_between(_indicator_counter, 19, 44))
    {
        set_buzzer_tone(3600, true);
        set_rgb_led_color(1, 0, 1);
    }
    else if (_indicator_counter >= 43)
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_startup_fail()
{
    if (_indicator_is_value_between(_indicator_counter, 9, 21))
    {
        set_buzzer_tone(5000, true);
        set_rgb_led_color(0, 1, 1);
    }
    else if (_indicator_is_value_between(_indicator_counter, 20, 100))
    {
        _indicator_counter = 0;
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_event_command_received()
{
    if (_indicator_is_value_between(_indicator_counter, 0, 2))
    {
        set_buzzer_tone(3100, true);
        set_rgb_led_color(0,0,0);
    }
    else if (_indicator_is_value_between(_indicator_counter, 2, 4))
    {
        set_buzzer_tone(0, false);
        set_rgb_led_color(1,1,1);
    }
    else if (_indicator_is_value_between(_indicator_counter, 4, 6))
    {
        set_buzzer_tone(3100, true);
        set_rgb_led_color(0,0,0);
    }
    else if (_indicator_is_value_between(_indicator_counter, 6, 8))
    {
        set_buzzer_tone(0, false);
        set_rgb_led_color(1,1,1);
    }
    else if (_indicator_is_value_between(_indicator_counter, 8, 10))
    {
        set_buzzer_tone(3100, true);
        set_rgb_led_color(0,1,0);
    }
    else if (_indicator_counter >= 12)
    {
        set_buzzer_tone(0, false);
        set_rgb_led_color(1,1,1);
        remove_latest_special_indicator_from_queue();
        _indicator_counter = 0;
    }
}

void _indicator_event_usb_connection_changed()
{
    if (_indicator_is_value_between(_indicator_counter, 0, 7))
    {
        set_buzzer_tone(2600, true);
        set_rgb_led_color(0, 0, 1);
    }
    else if (_indicator_is_value_between(_indicator_counter, 7, 11))
    {
        set_buzzer_tone(3100, true);
        set_rgb_led_color(0, 0, 0);
    }
    else if (_indicator_counter >= 11)
    {
        _indicator_counter = 0;
        remove_latest_special_indicator_from_queue();
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_event_mission_abort()
{
   if (_indicator_is_value_between(_indicator_counter, 0, 11))
    {
        set_buzzer_tone(3500, true);
        set_rgb_led_color(0, 1, 1);
    }
    else if (_indicator_counter >= 11)
    {
        _indicator_counter = 0;
        remove_latest_special_indicator_from_queue();
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}

void _indicator_event_error()
{
    if (_indicator_is_value_between(_indicator_counter, 0, 7))
    {
        set_buzzer_tone(3300, true);
        set_rgb_led_color(0, 1, 1);
    }
    else if (_indicator_is_value_between(_indicator_counter, 7, 11))
    {
        set_buzzer_tone(3700, true);
        set_rgb_led_color(1, 0, 0);
    }
    else if (_indicator_counter >= 11)
    {
        _indicator_counter = 0;
        remove_latest_special_indicator_from_queue();
        set_buzzer_tone(0, false);
        set_rgb_led_color(1, 1, 1);
    }
}