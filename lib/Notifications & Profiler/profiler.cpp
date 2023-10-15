#include "Profiler.h"

void Profiler::begin_function()
{
    function_start_time = micros();
}

void Profiler::begin_loop()
{
    if (!enable_loop_measure)
    {
        enable_loop_measure = true;
        loop_start_time = micros();
    }
}

void Profiler::end_loop(uint64_t &t_loop_dt)
{
    enable_loop_measure = false;

    end_loop_time = micros();
    if (loop_start_time != 0){
        t_loop_dt = (end_loop_time - loop_start_time) / 1000;
        loop_start_time = 0;
    }
}

void Profiler::end_function(uint64_t &t_function_duration)
{
    end_function_time = micros();
    if (function_start_time != 0){
        t_function_duration = (end_function_time - function_start_time) / 1000;
        function_start_time = 0;
    }
}
