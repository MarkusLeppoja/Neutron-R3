#ifndef __ALERTS_H__
#define __ALERTS_H__

#include <Arduino.h>

//TODO: Integration with flash chip.
//Implement a data logging queue that has all the non logged events inside it.
//If flash logging begins then empty the queue first and then begin logging.

// 1. At the beginning of the program we aren't logging to flash until we enter ground lock. 
// That means we can't print data to flash until ground lock so instead we store it inside a buffer.


/* TODO List
 * Implement a buffer system for data recorder.
 * Using buffer system save all events to a string to be used later if needed. (the string should be in variables file with a get function)
 * 
 * 
*/



// @brief Stores temporarely new alerts, until they get logged to flash. 
// @note Has a max size of 48 alerts
extern String _alert_buffer[];

// @brief Returns the fullness of the _alert_buffer. If 0 then buffer is empty
extern uint8_t _alert_buffer_size;

// @brief Will contain all alerts from the start to end
extern String _alert_string;

void clear_alert_buffer();

// @brief Logs a given alert to buffer if free space is present. Also has overflow protection
void save_alert_to_buffer(String alert);

// @brief Returns the alert string
String get_all_alerts();

enum e_alert_type 
{
    none = 0,
    alert = 1,
    success = 2,
    warning = 3,
    error = 4
};

class Alerts {
private:

    String s_alert_type[5] = 
    {
        "None",
        "Log",
        "Success",
        "Warning",
        "Error"
    };

    // Used to store the current time formated in this pattern: [hours/minutes/seconds/milliseconds]
    String current_time_formatted;

    // Used to store info about the orgin of the event.
    String alert_orgin_class;

    // This stores the recent alert init. Also used for creating new alerts.
    String current_alert_instance;

    // @brief Divides MCU on time into this format [hours/minutes/seconds/milliseconds] 
    void calculate_mcu_on_time();

    // @brief Takes in parameters and returns the event string
    // @param type Type of an alert
    // @param message Description of the alert
    // @param alert_orgin Where is the alert originated
    void create_full_alert_text(e_alert_type alert_type, String message, String alert_orgin, String& alert_instance);
public:

    Alerts(String alert_orgin);
    
    // @brief Save an event to the RP2040 onboard flash
    // @param type Type of an alert
    // @param message Description of the alert
    void create_alert(e_alert_type alert_type, String message);
};
#endif