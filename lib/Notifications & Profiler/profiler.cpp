#include "Profiler.h"

void Profiler::begin_function()
{
    function_start_time = micros();
}

void Profiler::begin_loop()
{
    if (!enable_loop_measure)
    {
        enable_loop_measure = 1;
        loop_start_time = micros();
    }
}

double Profiler::end_loop()
{
    enable_loop_measure = 0;

    end_loop_time = micros();

    return double((end_loop_time - loop_start_time) / 1000.);
}

double Profiler::end_function()
{
    end_function_time = micros();

    return double((end_function_time - function_start_time) / 1000.);
}
