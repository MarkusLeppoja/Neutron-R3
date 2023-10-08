#include "alerts.h"

String alert_buffer[50];
uint8_t alert_buffer_size;

void save_alert_to_buffer(String alert)
{
    if (alert_buffer_size > 48) return;
    alert_buffer[alert_buffer_size] = alert;
    alert_buffer_size++;
}

void clear_alert_buffer()
{
    for (uint8_t i = 0; i < alert_buffer_size; i++)
    {
        alert_buffer[i] = "";
    }
    alert_buffer_size = 0;
}

Alerts::Alerts(String alert_orgin)
{
    alert_orgin_class = alert_orgin;
}


void Alerts::create_alert(e_alert_type alert_type, String message)
{
    calculate_mcu_on_time();
    create_full_alert_text(alert_type, message, alert_orgin_class, current_alert_instance);
    save_alert_to_buffer(current_alert_instance);
}


void Alerts::calculate_mcu_on_time()
{
    uint64_t milliseconds = millis();

    uint16_t ms = milliseconds % 1000;
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
