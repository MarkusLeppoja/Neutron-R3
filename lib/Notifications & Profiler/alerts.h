#ifndef __ALERTS_H__
#define __ALERTS_H__

#include <Arduino.h>
#include <definitions.h>

// @brief Will contain all alerts from the start to end
extern String _alert_string;

// @brief Used to store the recent alert.
extern String current_alert_instance;

enum e_alert_type 
{
    none = 0,
    alert = 1,
    success = 2,
    warning = 3,
    error = 4
};

// @brief Returns a string containing all alerts from vehicle power on
String get_all_alerts();
String get_most_recent_alert();

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

    // @brief Used to store the current time formated in this pattern: [hours/minutes/seconds/milliseconds]
    String current_time_formatted;

    // @brief Used to store info about the orgin of an event.
    String alert_orgin_class;

    // @brief Divides MCU on time into this format [hours/minutes/seconds/milliseconds] 
    void calculate_mcu_on_time();

    // @brief Takes in parameters and returns the event string
    // @param type Type of an alert
    // @param message Description of the alert
    // @param alert_orgin Where is the alert originated
    void create_full_alert_text(e_alert_type alert_type, String message, String alert_orgin, String& alert_instance);
public:

    // @brief Create instance of alerts class. Define what file the alert is coming from
    Alerts(String alert_orgin);
    
    // @brief Save an event to the RP2040 onboard flash
    // @param type Type of an alert
    // @param message Description of the alert
    void create_alert(e_alert_type alert_type, String message);
};
#endif