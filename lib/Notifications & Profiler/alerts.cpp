#include "alerts.h"
#include "data_recorder.h"

String _alert_string;
String current_alert_instance;

String get_all_alerts()
{
    return _alert_string;
}

String get_most_recent_alert()
{
    return current_alert_instance;
}

Alerts::Alerts(String alert_orgin)
{
    alert_orgin_class = alert_orgin;
}

void Alerts::create_alert(e_alert_type alert_type, String message)
{
    calculate_mcu_on_time();
    create_full_alert_text(alert_type, message, alert_orgin_class, current_alert_instance);
    cast_alert_to_data_recorder(get_most_recent_alert());
}

void Alerts::calculate_mcu_on_time()
{
    uint64_t milliseconds = millis();

    uint32_t ms = milliseconds % 1000;
    uint8_t second = (milliseconds / 1000) % 60;
    uint8_t minute = (milliseconds / 60000) % 60;
    uint8_t hour = (milliseconds / 3600000) % 24;
    
    current_time_formatted = String(hour) + ":" + String(minute) + ":" + String(second) + "." + String(ms);
}

void Alerts::create_full_alert_text(e_alert_type alert_type, String message, String alert_orgin, String& alert_instance)
{
    alert_instance =
    "[" + String(current_time_formatted) + "]" +
    "[" + String(s_alert_type[alert_type]) + "]" +
    "[" + String(alert_orgin) + "]" + 
    "[" + String(message) + "]";
}
