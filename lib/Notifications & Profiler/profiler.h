#ifndef __SYSTEM_PROFILER_H__
#define __SYSTEM_PROFILER_H__

#include <Arduino.h>

// @brief We measure in microseconds and output the result in milliseconds
// @brief For every result make a global variable that will be updated
class Profiler
{
private:
    uint64_t loop_start_time, function_start_time, end_function_time, end_loop_time;
    boolean enable_loop_measure;
public:
    // @brief Calling this will begin measuring the delta time of a loop in milliseconds.
    void begin_loop();
    // @brief Calling this will begin measuring the duration of a function in milliseconds.
    void begin_function();
    // @brief Calling this will measure and point the delta time of a function in milliseconds.
    // @note This should be called at the start of a loop
    void end_loop(float &t_loop_dt);
    // @brief Calling this will measure and point the duration of a function in milliseconds.
    // @note This should be called at the end of a function
    void end_function(float &t_function_duration);
};
#endif