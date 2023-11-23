#ifndef __STORING_LOGIC_H__
#define __STORING_LOGIC_H__

#include <Arduino.h>

class STORING_LOGIC_T;
extern STORING_LOGIC_T storing_logic;

class STORING_LOGIC_T
{
public:
    void convert_variable(double& data, int8_t width, int8_t prec)
    {
        char buffer[sizeof(data) + 3];  // Size defined based on variable + comma size
        dtostrf(data, width, prec, buffer);
        strcat(buffer, ", ");
        push_to_main_buffer(buffer, main_buffer);
    }

    void convert_variable(int& data)
    {
        char buffer[sizeof(data) + 3];  // Size defined based on variable + comma size
        itoa(data, buffer, 10);
        strcat(buffer, ", ");
        push_to_main_buffer(buffer, main_buffer);
    }

    void convert_variable(int data)
    {
        char buffer[sizeof(data) + 3];  // Size defined based on variable + comma size
        itoa(data, buffer, 10);
        strcat(buffer, ", ");
        push_to_main_buffer(buffer, main_buffer);
    }

    void convert_variable(String data)
    {
        strcat(main_buffer, data.c_str());
        strcat(main_buffer, ", ");
    }

    void remove_previous_comma()
    {
        memcpy(main_buffer, String(main_buffer).substring(0, strlen(main_buffer) - 2).c_str(), sizeof(main_buffer));
    }

    // Empty buffer by setting the reference buffer memory to 0x00
    void clear_buffer()
    {
        memset(main_buffer, 0, sizeof(main_buffer));
    }

    String get_main_buffer_instance()
    {
        return String(main_buffer);
    }

private:
    char main_buffer[1024];
    
    // Combine primary and secondary buffers
    void push_to_main_buffer(char* ref_buffer, char* ref_main_buffer)
    {
        strcat(ref_main_buffer, ref_buffer);
    }
};
STORING_LOGIC_T storing_logic;

#endif