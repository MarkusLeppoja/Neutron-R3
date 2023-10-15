#include "alerts.h"

String _alert_buffer[50];
uint8_t _alert_buffer_size;
String _alert_string;



void save_alert_to_buffer(String alert)
{
    _alert_string += alert;

    // @note If flash functionality isn't enabled or buffer is full then don't log to buffer anymore
    if (_alert_buffer_size >= 49 || !Booleans.sw_flash_chip_usability) return;
    _alert_buffer[_alert_buffer_size] = alert;
    _alert_buffer_size++;
}

void clear_alert_buffer()
{
    for (uint8_t i = 0; i < _alert_buffer_size; i++)
    {
        _alert_buffer[i] = "";
    }
    _alert_buffer_size = 0;
}

String get_all_alerts()
{
    return _alert_string;
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
    "[" + String(message) + "]" +
    "\n";
}
